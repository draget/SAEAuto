/* 
 * File:   IBEO.cpp
 * Author: Timothy Black (20373081), modifications Thomas Drage (20510505).
 * 
 * Created on 18 April 2012, 7:34 PM
 * Modified 3 March 2013, 23 July 2013 (T.Drage).
 *
 * Road finding and object projection T. Drage Sep 2013.
 */


#include "IBEO.h"

#include <boost/thread.hpp> 
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "Logger.h"
#include "Control.h"
#include "Fusion.h"
#include "Xsens.h"

#include "linear.h"

#include "BoeingKalman.cpp"

using namespace std;

static char ibeo_magic_word[] = {(char)0xAF, (char)0xFE, (char)0xC0, (char)0xC2};   // The magic word to look for for beginning of message.

IBEONetwork *connection;        // The connection to communication to the ibeo.

SCAN_DATA_HEADER    scan_data_header[MSG_BUFFERS];                     // Header for the scan data.
SCAN_DATA_POINT     scan_data_points[MSG_BUFFERS][MAX_SCAN_POINTS];    // Data points for scans.
OBJECT_DATA_HEADER  object_data_header[MSG_BUFFERS];                   // Object data header.
OBJECT_DATA         object_data[MSG_BUFFERS][MAX_OBJECTS];             // Data for objects found by ibeo.
ERROR_DATA          error_data[MSG_BUFFERS];                           // Error data.

int curScanDataSource = 0;      // Current scan data source/buffer.
int curObjectDataSource = 0;    // Current object data source/buffer.
int curErrorDataSource = 0;     // Current error data source/buffer.

bool gotObject;
bool gotScan;

bool verbose = false;

double pitchLayer[] = {2.9, 1.58, 0.65, -0.29}; //Pitch of scan layers (0,1,2,3) relative to the LiDAR's pitch (+ve => angle (more) below horizontal,-ve => angle (more) above horizontal), as reported in Calvin Yapp's thesis

float MeasNoise[3] = {1,0,0};
float ProcNoise[3] = {0.1,0.1,0};

double *Edges;

double LHS[4] = {-2, -2, -2, -2};
double RHS[4] = {2, 2, 2, 2};

std::vector<double> xCoords;
std::vector<double> yCoords;

int dips = 0;
int reallocs = 0;
int peaked = 0;
int failed = 0;

KalmanPVA KlmL[4] = {KalmanPVA(2,K_P,LHS[0],0,0,0.2,ProcNoise,MeasNoise), KalmanPVA(2,K_P,LHS[0],0,0,0.2,ProcNoise,MeasNoise), KalmanPVA(2,K_P,LHS[0],0,0,0.2,ProcNoise,MeasNoise), KalmanPVA(2,K_P,LHS[0],0,0,0.2,ProcNoise,MeasNoise)};
KalmanPVA KlmR[4] = {KalmanPVA(2,K_P,RHS[0],0,0,0.2,ProcNoise,MeasNoise), KalmanPVA(2,K_P,RHS[0],0,0,0.2,ProcNoise,MeasNoise), KalmanPVA(2,K_P,RHS[0],0,0,0.2,ProcNoise,MeasNoise), KalmanPVA(2,K_P,RHS[0],0,0,0.2,ProcNoise,MeasNoise)};

/**
 * Purpose: Creates a new instance of the IBEO object.
 * Inputs : None.
 * Outputs: None.
 */
IBEO::IBEO(Control* CarController, Logger* Logger) {

	gettimeofday(&lastwrite,NULL);
	layersToScan = LAYERS;
	CarControl = CarController;
 	Log = Logger;
    connection = new IBEONetwork();
    for (int i=0; i<MSG_BUFFERS; i++) {
        scan_data_header[i].scan_points = 0;
        object_data_header[i].number_of_objects = 0;
    }
    inUse = false;
}

/**
 * Purpose: Creates a new instance of the IBEO object.
 * Inputs : An IBEO object.
 * Outputs: None.
 */
IBEO::IBEO(const IBEO& orig) {
}

/**
 * Purpose: Destroys the instance of the IBEO object.
 * Inputs : None.
 * Outputs: None.
 */
IBEO::~IBEO() {
    if (connection->IsConnected()) connection->Disconnect();
}

/**
 * Purpose: Starts the communication between device and ibeo scanner.
 * Inputs : IP Address and port to connect to.
 * Outputs: true if successful, false otherwise.
 */
bool IBEO::Open(char * ip_addr, int port) {
    Log->WriteLogLine("IBEO scanner - attempting to connect to " + boost::lexical_cast<std::string>(ip_addr) + ":" + boost::lexical_cast<std::string>(port) + ".");
    if (!connection->Connect(ip_addr, port)) {
	Log->WriteLogLine("IBEO scanner - Connecting failed...");
        return false;
    }
    inUse = true;
    Log->WriteLogLine("IBEO scanner - connected to scanner successfully.");

    return true;
}

/**
 * Purpose: Connects to the ibeo scanner using the default IP Address and port.
 * Inputs : None.
 * Outputs: true if successful, false otherwise.
 */
bool IBEO::Open() {
    return Open((char *)IBEO_IP_ADDRESS, IBEO_PORT);
}

/**
 * Purpose: Disconnects from the ibeo scanner.
 * Inputs : None.
 * Outputs: None.
 */
void IBEO::StopScanner() {
    connection->Disconnect();
    inUse = false;
    Run = false;
    sleep(1);
    if(verbose) { cout << "ibeo scanner: disconnected from scanner." << endl; }
}

/**
 * Purpose: Finds the ibeo header from the data buffer.
 * Inputs : None.
 * Outputs: The IBEO_HEADER object.
 */
IBEO_HEADER IBEO::FindHeader() {

    bool    foundHeader;
    IBEO_HEADER headerIn;
    char    byteIn;
    int     i;

    foundHeader = false;
    while (!foundHeader) {
        for (i=0; i<4; i++) {
            if (recv(connection->socketFD, &byteIn, 1, 0) > 0) {
                if (byteIn != ibeo_magic_word[i]) break;
            } else {
                throw 1;
            }
        }
        if (i == 4) foundHeader = true;
    }

    if (recv(connection->socketFD, &headerIn.size_prev_msg, sizeof(headerIn.size_prev_msg), 0) != sizeof(headerIn.size_prev_msg)) throw 1;
    if (recv(connection->socketFD, &headerIn.size_cur_msg, sizeof(headerIn.size_cur_msg), 0) != sizeof(headerIn.size_cur_msg)) throw 1;
    if (recv(connection->socketFD, &headerIn.res, sizeof(headerIn.res), 0) != sizeof(headerIn.res)) throw 1;
    if (recv(connection->socketFD, &headerIn.dev_id, sizeof(headerIn.dev_id), 0) != sizeof(headerIn.dev_id)) throw 1;
    if (recv(connection->socketFD, &headerIn.data_type, sizeof(headerIn.data_type), 0) != sizeof(headerIn.data_type)) throw 1;
    if (recv(connection->socketFD, &headerIn.ntp_time, sizeof(headerIn.ntp_time), 0) != sizeof(headerIn.ntp_time)) throw 1;

    if (verbose) {
        cout << "ibeo scanner: found header for device id " << htons(headerIn.dev_id) << "." << endl;
        cout << "              data type for next message is " << htons(headerIn.data_type) << "." << endl;
        cout << "              size of next message is " << htons(headerIn.size_cur_msg) << "." << endl;
    }
    return headerIn;

}

/**
 * Purpose: Reads messages from the ibeo.
 * Inputs : None.
 * Outputs: None.
 */
void IBEO::ReadMessages() {



    IBEO_HEADER header;

	gotObject = false;
	gotScan = false;
    
    while(! (gotObject == true && gotScan == true)) {	// Scan until we have one of each set of data.
        try {
            header = this->FindHeader();
            switch (htons(header.data_type)) {
                case 0x2202:
                    if (verbose) Log->WriteLogLine( "ibeo scanner: found scan data." );
                    if (!Read_Scan_Data()) return;
                    break;
                case 0x2221:
                    if (verbose) Log->WriteLogLine( "ibeo scanner: found object data.");
                    if (!Read_Object_Data()) return;
                    break;
                case 0x2030:
                    if (verbose) Log->WriteLogLine( "ibeo scanner: found error data.");
                    if (!Read_Errors()) return;
                    break;
                default:
                    if (verbose) Log->WriteLogLine("ibeo scanner: received unknown message with id "); //+ htons(header.data_type));
            }

        } catch (int e) {
		Log->WriteLogLine("IBEO - caught error!");
            return;
        }
    }

    read(connection->socketFD, NULL, 1000000);
}

/**
 * Purpose: Reads the scan data from the ibeo scanner.
 * Inputs : None.
 * Outputs: true if successful, false otherwise.
 */
bool IBEO::Read_Scan_Data() {

    int next_scan_source;
    int i;

    next_scan_source = (this->curScanDataSource + 1) % 2;
    if (recv(connection->socketFD, &(scan_data_header[next_scan_source].scan_number), sizeof(scan_data_header[next_scan_source].scan_number), 0) != sizeof(scan_data_header[next_scan_source].scan_number)) return false;
    if (recv(connection->socketFD, &(scan_data_header[next_scan_source].scanner_status), sizeof(scan_data_header[next_scan_source].scanner_status), 0) != sizeof(scan_data_header[next_scan_source].scanner_status)) return false;
    if (recv(connection->socketFD, &(scan_data_header[next_scan_source].sync_phase_offset), sizeof(scan_data_header[next_scan_source].sync_phase_offset), 0) != sizeof(scan_data_header[next_scan_source].sync_phase_offset)) return false;
    if (recv(connection->socketFD, &(scan_data_header[next_scan_source].scan_start_time), sizeof(scan_data_header[next_scan_source].scan_start_time), 0) != sizeof(scan_data_header[next_scan_source].scan_start_time)) return false;
    if (recv(connection->socketFD, &(scan_data_header[next_scan_source].scan_stop_time), sizeof(scan_data_header[next_scan_source].scan_stop_time), 0) != sizeof(scan_data_header[next_scan_source].scan_stop_time)) return false;
    if (recv(connection->socketFD, &(scan_data_header[next_scan_source].angle_ticks_per_rotation), sizeof(scan_data_header[next_scan_source].angle_ticks_per_rotation), 0) != sizeof(scan_data_header[next_scan_source].angle_ticks_per_rotation)) return false;
    if (recv(connection->socketFD, &(scan_data_header[next_scan_source].start_angle), sizeof(scan_data_header[next_scan_source].start_angle), 0) != sizeof(scan_data_header[next_scan_source].start_angle)) return false;
    if (recv(connection->socketFD, &(scan_data_header[next_scan_source].stop_angle), sizeof(scan_data_header[next_scan_source].stop_angle), 0) != sizeof(scan_data_header[next_scan_source].stop_angle)) return false;
    if (recv(connection->socketFD, &(scan_data_header[next_scan_source].scan_points), sizeof(scan_data_header[next_scan_source].scan_points), 0) != sizeof(scan_data_header[next_scan_source].scan_points)) return false;
    if (recv(connection->socketFD, &(scan_data_header[next_scan_source].mp_yaw), sizeof(scan_data_header[next_scan_source].mp_yaw), 0) != sizeof(scan_data_header[next_scan_source].mp_yaw)) return false;
    if (recv(connection->socketFD, &(scan_data_header[next_scan_source].mp_pitch), sizeof(scan_data_header[next_scan_source].mp_pitch), 0) != sizeof(scan_data_header[next_scan_source].mp_pitch)) return false;
    if (recv(connection->socketFD, &(scan_data_header[next_scan_source].mp_roll), sizeof(scan_data_header[next_scan_source].mp_roll), 0) != sizeof(scan_data_header[next_scan_source].mp_roll)) return false;
    if (recv(connection->socketFD, &(scan_data_header[next_scan_source].mp_x), sizeof(scan_data_header[next_scan_source].mp_x), 0) != sizeof(scan_data_header[next_scan_source].mp_x)) return false;
    if (recv(connection->socketFD, &(scan_data_header[next_scan_source].mp_y), sizeof(scan_data_header[next_scan_source].mp_y), 0) != sizeof(scan_data_header[next_scan_source].mp_y)) return false;
    if (recv(connection->socketFD, &(scan_data_header[next_scan_source].mp_z), sizeof(scan_data_header[next_scan_source].mp_z), 0) != sizeof(scan_data_header[next_scan_source].mp_z)) return false;
    if (recv(connection->socketFD, &(scan_data_header[next_scan_source].flags), sizeof(scan_data_header[next_scan_source].flags), 0) != sizeof(scan_data_header[next_scan_source].flags)) return false;

    if (verbose) cout << "ibeo scanner: reading " << scan_data_header[next_scan_source].scan_points << " points from ibeo." << endl;
    if (verbose) cout << "ibeo scanner: start angle " << scan_data_header[next_scan_source].start_angle << " degrees." << endl;
    if (verbose) cout << "ibeo scanner: stop angle " << scan_data_header[next_scan_source].stop_angle << " degrees." << endl;

    for (i=0; i<scan_data_header[next_scan_source].scan_points; i++) {
        if (recv(connection->socketFD, &scan_data_points[next_scan_source][i].layer_echo, sizeof(scan_data_points[next_scan_source][i].layer_echo), 0) != sizeof(scan_data_points[next_scan_source][i].layer_echo)) return false;
        if (recv(connection->socketFD, &scan_data_points[next_scan_source][i].flags, sizeof(scan_data_points[next_scan_source][i].flags), 0) != sizeof(scan_data_points[next_scan_source][i].flags)) return false;
        if (recv(connection->socketFD, &scan_data_points[next_scan_source][i].horiz_angle, sizeof(scan_data_points[next_scan_source][i].horiz_angle), 0) != sizeof(scan_data_points[next_scan_source][i].horiz_angle)) return false;
        if (recv(connection->socketFD, &scan_data_points[next_scan_source][i].radial_dist, sizeof(scan_data_points[next_scan_source][i].radial_dist), 0) != sizeof(scan_data_points[next_scan_source][i].radial_dist)) return false;
        if (recv(connection->socketFD, &scan_data_points[next_scan_source][i].echo_pulse_width, sizeof(scan_data_points[next_scan_source][i].echo_pulse_width), 0) != sizeof(scan_data_points[next_scan_source][i].echo_pulse_width)) return false;
        if (recv(connection->socketFD, &scan_data_points[next_scan_source][i].res, sizeof(scan_data_points[next_scan_source][i].res), 0) != sizeof(scan_data_points[next_scan_source][i].res)) return false;
    }

    curScanDataSource = next_scan_source;
    gotScan = true;
    return true;

}

/**
 * Purpose: Reads object data from the ibeo scanner.
 * Inputs : None.
 * Outputs: true if successful, false otherwise.
 */
bool IBEO::Read_Object_Data() {

    int next_object_source;
    int i, j;

    next_object_source = (this->curObjectDataSource + 1) % 2;
    if (recv(connection->socketFD, &object_data_header[next_object_source].scan_start_time, sizeof(object_data_header[next_object_source].scan_start_time), 0) != sizeof(object_data_header[next_object_source].scan_start_time)) return false;
    if (recv(connection->socketFD, &object_data_header[next_object_source].number_of_objects, sizeof(object_data_header[next_object_source].number_of_objects), 0) != sizeof(object_data_header[next_object_source].number_of_objects)) return false;

    if (verbose) cout << "ibeo scanner: reading " << object_data_header[next_object_source].number_of_objects << " objects from ibeo." << endl;

    if (object_data_header[next_object_source].number_of_objects > MAX_OBJECTS) object_data_header[next_object_source].number_of_objects = MAX_OBJECTS;
    for (i=0; i<object_data_header[next_object_source].number_of_objects; i++) {
        if (recv(connection->socketFD, &object_data[next_object_source][i].object_id, sizeof(object_data[next_object_source][i].object_id), 0) != sizeof(object_data[next_object_source][i].object_id)) return false;
        if (recv(connection->socketFD, &object_data[next_object_source][i].object_age, sizeof(object_data[next_object_source][i].object_age), 0) != sizeof(object_data[next_object_source][i].object_age)) return false;
        if (recv(connection->socketFD, &object_data[next_object_source][i].object_prediction_age, sizeof(object_data[next_object_source][i].object_prediction_age), 0) != sizeof(object_data[next_object_source][i].object_prediction_age)) return false;
        if (recv(connection->socketFD, &object_data[next_object_source][i].relative_timestamp, sizeof(object_data[next_object_source][i].relative_timestamp), 0) != sizeof(object_data[next_object_source][i].relative_timestamp)) return false;

        if (!Read_Point2D(&object_data[next_object_source][i].reference_point)) return false;
        if (!Read_Point2D(&object_data[next_object_source][i].reference_point_sigma)) return false;
        if (!Read_Point2D(&object_data[next_object_source][i].closest_point)) return false;
        if (!Read_Point2D(&object_data[next_object_source][i].bounding_box_center)) return false;
        
        if (recv(connection->socketFD, &object_data[next_object_source][i].bounding_box_width, sizeof(object_data[next_object_source][i].bounding_box_width), 0) != sizeof(object_data[next_object_source][i].bounding_box_width)) return false;
        if (recv(connection->socketFD, &object_data[next_object_source][i].bounding_box_length, sizeof(object_data[next_object_source][i].bounding_box_length), 0) != sizeof(object_data[next_object_source][i].bounding_box_length)) return false;

        if (!Read_Point2D(&object_data[next_object_source][i].object_box_center)) return false;
        if (!Read_Size2D(&object_data[next_object_source][i].object_box_size)) return false;

        if (recv(connection->socketFD, &object_data[next_object_source][i].object_box_orientation, sizeof(object_data[next_object_source][i].object_box_orientation), 0) != sizeof(object_data[next_object_source][i].object_box_orientation)) return false;

        if (!Read_Point2D(&object_data[next_object_source][i].absolute_velocity)) return false;
        if (!Read_Size2D(&object_data[next_object_source][i].absolute_velocity_sigma)) return false;
        if (!Read_Point2D(&object_data[next_object_source][i].relative_velocity)) return false;

        if (recv(connection->socketFD, &object_data[next_object_source][i].classification, sizeof(object_data[next_object_source][i].classification), 0) != sizeof(object_data[next_object_source][i].classification)) return false;
        if (recv(connection->socketFD, &object_data[next_object_source][i].classification_age, sizeof(object_data[next_object_source][i].classification_age), 0) != sizeof(object_data[next_object_source][i].classification_age)) return false;
        if (recv(connection->socketFD, &object_data[next_object_source][i].classification_certainty, sizeof(object_data[next_object_source][i].classification_certainty), 0) != sizeof(object_data[next_object_source][i].classification_certainty)) return false;
        if (recv(connection->socketFD, &object_data[next_object_source][i].number_contour_points, sizeof(object_data[next_object_source][i].number_contour_points), 0) != sizeof(object_data[next_object_source][i].number_contour_points)) return false;

        if (object_data[next_object_source][i].number_contour_points > MAX_CONTOUR_POINTS) object_data[next_object_source][i].number_contour_points = MAX_CONTOUR_POINTS;
        for (j=0; j<object_data[next_object_source][i].number_contour_points; j++) {
            if (!Read_Point2D(&object_data[next_object_source][i].contour_points[j])) return false;
        }
    }

    curObjectDataSource = next_object_source;
    gotObject = true;
    return true;

}

/**
 * Purpose: Reads the errors from the ibeo scanner.
 * Inputs : None.
 * Outputs: true if successful, false otherwise.
 */
bool IBEO::Read_Errors() {

    int next_error_source;

    if (verbose) cout << "ibeo scanner: receiving errors from the ibeo." << endl;

    next_error_source = (this->curErrorDataSource + 1) % 2;
    if (recv(connection->socketFD, &error_data[next_error_source].error_register_1, sizeof(error_data[next_error_source].error_register_1), 0) != sizeof(error_data[next_error_source].error_register_1)) return false;
    if (recv(connection->socketFD, &error_data[next_error_source].error_register_2, sizeof(error_data[next_error_source].error_register_2), 0) != sizeof(error_data[next_error_source].error_register_2)) return false;
    if (recv(connection->socketFD, &error_data[next_error_source].warning_register_1, sizeof(error_data[next_error_source].warning_register_1), 0) != sizeof(error_data[next_error_source].warning_register_1)) return false;
    if (recv(connection->socketFD, &error_data[next_error_source].warning_register_2, sizeof(error_data[next_error_source].warning_register_2), 0) != sizeof(error_data[next_error_source].warning_register_2)) return false;
    if (recv(connection->socketFD, &error_data[next_error_source].res, sizeof(error_data[next_error_source].res), 0) != sizeof(error_data[next_error_source].res)) return false;

    return true;

}

/**
 * Purpose: Reads the POINT_2D from the ibeo scanner.
 * Inputs : The POINT_2D storage location.
 * Outputs: true if successful, false otherwise.
 */
bool IBEO::Read_Point2D(POINT_2D *pointIn) {
    if (recv(connection->socketFD, &pointIn->x, sizeof(short), 0) != sizeof(short)) return false;
    if (recv(connection->socketFD, &pointIn->y, sizeof(short), 0) != sizeof(short)) return false;
    return true;
}

/**
 * Purpose: Reads the SIZE_2D from the ibeo scanner.
 * Inputs : The SIZE_2D storage location.
 * Outputs: true if successful, false otherwise.
 */
bool IBEO::Read_Size2D(SIZE_2D *sizeIn) {
    if (recv(connection->socketFD, &sizeIn->x, sizeof(short), 0) != sizeof(short)) return false;
    if (recv(connection->socketFD, &sizeIn->y, sizeof(short), 0) != sizeof(short)) return false;
    return true;
}


/*
*	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*	All code after this point was written by T. Drage, Sep 2013.
*	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

/**
 * Purpose: Start the IBEO sensor processing incoming messages.
 * Inputs : None.
 * Outputs: None.
 */
void IBEO::Start() {

	if(inUse) {	

		Run = true;

		m_Thread = boost::thread(&IBEO::ProcessMessages, this);
		m_Thread.detach();

	}

}


/**
 * Purpose: Contains the main loop that deals with IBEO data.
 * Inputs : None.
 * Outputs: None.
 */
void IBEO::ProcessMessages() {

	while(Run) {

		ReadMessages();

		timeval current;
		gettimeofday(&current,NULL);
		
		// Save CPU time by not acting upon every set of data.
		if((current.tv_sec + ((double)current.tv_usec)/1000000) > (lastwrite.tv_sec + ((double)lastwrite.tv_usec)/1000000) + IBEO_PERIOD) { 

			//PolarToXY(); //Called individually for each scan layer in FindRoads()

			FindRoads();

			WriteFiles(current); 
			
			CarControl->CurrentMap.DetectedFenceposts.clear(); //Clears detected objects between scans
	
			ProjectObjectsToMap();

			gettimeofday(&lastwrite,NULL);

		}
		

	}

}

/**
 * Purpose: Convert the various object contour points detected into fenceposts on the map.
 * Inputs : None.
 * Outputs: None.
 */
void IBEO::ProjectObjectsToMap() {

	VECTOR_2D Object;
	VECTOR_2D TransformedObject;

	for(int i = 0; i < object_data_header[curObjectDataSource].number_of_objects; i++) {

		for(int j = 0; j < object_data[curObjectDataSource][i].number_contour_points; j++) {	

			Object.x = (double)object_data[curObjectDataSource][i].contour_points[j].x/100.0; 
			Object.y = (double)object_data[curObjectDataSource][i].contour_points[j].y/100.0;

			// Ignore objects that make up the road.
			if(CarControl->VectorMagnitude(Object) > OBJECT_THRESHOLD) { continue; }
		
			TransformedObject.x = -cos(CarControl->Fuser->CurrentHeading) * Object.y + sin(CarControl->Fuser->CurrentHeading) * Object.x + CarControl->Fuser->CurrentPosition.x;
			TransformedObject.y = -1*sin(CarControl->Fuser->CurrentHeading) * Object.y + cos(CarControl->Fuser->CurrentHeading) * Object.x + CarControl->Fuser->CurrentPosition.y;

			bool Found = false;

			// Only put fenceposts down that are at least a half radius away from existing ones.
			BOOST_FOREACH( VECTOR_2D MapPoint, CarControl->CurrentMap.DetectedFenceposts ) {
				if(CarControl->VectorMagnitude(CarControl->SubtractVector(MapPoint,TransformedObject)) < CarControl->FencepostRadius/2) { Found = true; }
			}

			if(! Found) { 
				CarControl->CurrentMap.DetectedFenceposts.push_back(TransformedObject);
				CarControl->RespondToObject((unsigned short)object_data[curObjectDataSource][i].classification);
			}
		}

	}

}


/**
 * Purpose: Convert the polar scan data to Cartesian coordinates..
 * Inputs : None.
 * Outputs: None.
 */
void IBEO::PolarToXY(int scanLayer) {

	CurrentXYScan.xvalues.clear();
	CurrentXYScan.yvalues.clear();
	CurrentXYScan.zvalues.clear();

	double horizangle, vertang, planarDist;

	for(int i = 0; i < scan_data_header[curScanDataSource].scan_points; i++) {
		if(scan_data_points[curScanDataSource][i].layer_echo != scanLayer) { continue; }
		vertang = (PITCHDIFFIMU + pitchLayer[scanLayer])*CarControl->TwoPi/360;
		horizangle = (scan_data_points[curScanDataSource][i].horiz_angle+2880)*CarControl->TwoPi/11520; //I believe this is horizontal angle
		double absdist = scan_data_points[curScanDataSource][i].radial_dist/100.0;
		planarDist = cos(vertang)*absdist;
		
		if(planarDist < sqrt(pow(MAX_Y,2) + pow(MAX_X,2))) { 
			CurrentXYScan.xvalues.push_back(cos(horizangle)*planarDist);
			CurrentXYScan.yvalues.push_back(sin(horizangle)*planarDist); 
			CurrentXYScan.zvalues.push_back(LIDARHEIGHT - sin(vertang)*absdist); //Sets origin to approximately the point on the ground directly under the IMU and LiDAR scanner, on the same plane as the bottom of the tyres.
		}
	}
	correctForPitch(CurrentXYScan.xvalues, CurrentXYScan.yvalues, CurrentXYScan.zvalues);
	correctForRoll(CurrentXYScan.xvalues, CurrentXYScan.yvalues, CurrentXYScan.zvalues);
	
	xCoords.insert(xCoords.end(), CurrentXYScan.xvalues.begin(), CurrentXYScan.xvalues.end());
	yCoords.insert(yCoords.end(), CurrentXYScan.yvalues.begin(), CurrentXYScan.yvalues.end());
}


/**
 * Purpose: Write out the IBEO results files.
 * Inputs : timeval associated with this scan.
 * Outputs: None.
 */
void IBEO::WriteFiles(timeval current) {

		if(CarControl->ExtLogging) {
		  	system(("cp ./ramdisk/current.lux " + CarControl->LogDir + "/luxscan/" + boost::lexical_cast<std::string>(current.tv_sec + ((double)current.tv_usec)/1000000) + ".lux").c_str());
			system(("cp ./ramdisk/current.luxobj " + CarControl->LogDir + "/luxobj/" + boost::lexical_cast<std::string>(current.tv_sec + ((double)current.tv_usec)/1000000) + ".luxobj").c_str());		
		}

		ofstream lockfile("./ramdisk/ibeo.lck");
		
		lockfile.close();

		ofstream outfile_scan;

		std::string FileName = "./ramdisk/current.lux";

		outfile_scan.open(FileName.c_str(), ios::trunc);
		

		for(int i = 0; i < scan_data_header[curScanDataSource].scan_points; i++) {
				int layer = (int)scan_data_points[curScanDataSource][i].layer_echo & 15;
			outfile_scan << layer << "," 
				<< (int)scan_data_points[curScanDataSource][i].flags << "," 
				<< scan_data_points[curScanDataSource][i].horiz_angle << "," 
				<< scan_data_points[curScanDataSource][i].radial_dist << "," 
				<< scan_data_points[curScanDataSource][i].echo_pulse_width << "," 
				<< scan_data_points[curScanDataSource][i].res << "\n";
		}
		
		for(int i = 0; i < layersToScan; i++) {
			if (LHEdge[i] != 0 && RHEdge[i] != 0) {
				outfile_scan << "R," << boost::lexical_cast<std::string>(LHEdge[i]) << "," << boost::lexical_cast<std::string>(RHEdge[i]) << "," << boost::lexical_cast<std::string>(RoadSlope[i]) << "," << boost::lexical_cast<std::string>(RoadIntercept[i]) << "\n";
			}
		}

		outfile_scan.close();

		ofstream outfile_obj;

		FileName = "./ramdisk/current.luxobj";

		outfile_obj.open(FileName.c_str(), ios::trunc);

		for(int i = 0; i < object_data_header[curObjectDataSource].number_of_objects; i++) {
			outfile_obj << (int)object_data[curObjectDataSource][i].object_id << "," 
				<< (int)object_data[curObjectDataSource][i].reference_point.x << "," 
				<< object_data[curObjectDataSource][i].reference_point.y << "," 
				<< (int)object_data[curObjectDataSource][i].closest_point.x << "," 
				<< object_data[curObjectDataSource][i].closest_point.y << "," 
				<< object_data[curObjectDataSource][i].classification << "," 
				<< object_data[curObjectDataSource][i].bounding_box_center.x << "," 
				<< object_data[curObjectDataSource][i].bounding_box_center.y << "," 
				<< object_data[curObjectDataSource][i].bounding_box_width << "," 
				<< object_data[curObjectDataSource][i].bounding_box_length << ","
				<< object_data[curObjectDataSource][i].object_age << "\n";
		}
		
		outfile_obj.close();
		
		remove("./ramdisk/ibeo.lck");

}

int IBEO::findrpeak(double AvgRHS, std::vector<RPOINT> rightr) {

	std::vector<RPOINT> Maxs;
	std::vector<RPOINT> Mins;

	// Locates local maximum
	for (std::vector<RPOINT>::iterator it=rightr.begin()+floor((GROUPSIZE-1)/2); it!=rightr.end(); it++) { // Loop through all points

		bool max = true;

		double this_r = (*it).r;

		std::vector<RPOINT>::iterator groupend;

		// Deal with hitting the end.
		if(it + floor((GROUPSIZE-1)/2) > rightr.end()) { groupend = rightr.end(); }
		else { groupend = it + floor((GROUPSIZE-1)/2); }

		for(std::vector<RPOINT>::iterator groupit=it-floor((GROUPSIZE-1)/2); groupit<groupend; groupit++) { // Look to either side to see if point is a max.
			if((*groupit).r > this_r) { max = false; }
		}

		if(max == true) { 
			Maxs.push_back(*it);		
		}

	}


	// Look for (deep) local minima.
	for (std::vector<RPOINT>::iterator it=rightr.begin()+floor((GROUPSIZE-1)/2); it!=rightr.end(); it++) { // Loop through all points

		bool min = true;

		double this_r = (*it).r;

		if(this_r > MIN_R) { continue; }

		std::vector<RPOINT>::iterator groupend;

		// Deal with hitting the end.
		if(it + floor((GROUPSIZE-1)/2) > rightr.end()) { groupend = rightr.end(); }
		else { groupend = it + floor((GROUPSIZE-1)/2); }

		for(std::vector<RPOINT>::iterator groupit=it-floor((GROUPSIZE-1)/2); groupit<groupend; groupit++) { // Look to either side to see if point is a max.
			if((*groupit).r < this_r) { min = false; }
		}

		if(min == true) { 
			Mins.push_back(*it);		
		}

	}

	double Max_r = 0;
	int Peak_i = 0;
	double Peak_x;

	double Min_r = 1;
	int Dip_i = 0;
	double Dip_x;

	// Find the absolute max
	for (std::vector<RPOINT>::iterator it=Maxs.begin(); it!=Maxs.end(); it++) {
		if((*it).r > Max_r*0.9 && (*it).slope < MAX_SLOPE) { Max_r = (*it).r; Peak_i = (*it).i; Peak_x = (*it).x; } // If there are close peaks (90%), prefer the outer one.
	}

	// Find the absolute min within reason
	for (std::vector<RPOINT>::iterator it=Mins.begin(); it!=Mins.end(); it++) {
		if(((*it).r*0.8 < Min_r || (*it).r < 0.05) && fabs((*it).x) < fabs(AvgRHS) + ALLOWED_VARIATION) { // If they are almost zero, prefer the outer one.
			Min_r = (*it).r; Dip_i = (*it).i; Dip_x = (*it).x; 		
		} 
	}


		bool CheckDip = false;

	//std::cout << Peak_x << "," << AvgRHS << "," << Dip_x << "\n";
		if(fabs(Peak_x - AvgRHS) > ALLOWED_VARIATION) { // If we are way off look for the peak near the average!

			double ClosestDelta = 1000;
			int Closest_Peak_i;
			double Closest_Max_r;

			for (std::vector<RPOINT>::iterator it=Maxs.begin(); it!=Maxs.end(); it++) {
				//std::cout << (*it).x << " " << fabs((*it).x - AvgRHS) << "\n";
				if(fabs((*it).x - AvgRHS) < ClosestDelta || (fabs((*it).x - AvgRHS) < ClosestDelta*1.5 && (*it).r > Closest_Max_r*0.9)) { 
					Closest_Peak_i = (*it).i; Closest_Max_r = (*it).r; ClosestDelta = fabs((*it).x - AvgRHS); 
				}
			}

			if(Closest_Max_r > MIN_R && ClosestDelta < fabs(Dip_x - AvgRHS)) { reallocs++; return Closest_Peak_i; }	// Only return if the peak is good enough, else dip might be better.
			else { Peak_x = MIN_X; CheckDip = true; } // If we are going to seek a dip next, check conditions based on a reasonable value.
		}

		// If we are off average, but not by too far its profitable to seek the dip.
		if((fabs(Peak_x - AvgRHS) > VARIATION_THRESHOLD || CheckDip) && fabs(Dip_x) > fabs(Peak_x) && fabs(Dip_x - AvgRHS) < ALLOWED_VARIATION) {
			dips++; 
			return Dip_i;

		}

		if(Max_r > MIN_R) {  peaked++; return Peak_i; }

		else { failed++; return 0; }
	
}

std::vector<double> IBEO::XSlopeIntToXY(double x1, double x2, double slope, double inter) {
	double y1 = inter + slope*x1;
	double y2 = inter + slope*x2;
	
	std::vector<double> ret;
	ret.push_back(x1);
	ret.push_back(x2);
	ret.push_back(y1);
	ret.push_back(y2);
	return ret;
}

void IBEO::correctForPitch(std::vector<double> &xvalue, std::vector<double> &yvalue, std::vector<double> &zvalue) {
	double pitchCos = cosl(CarControl->IMU->pitch*CarControl->TwoPi/360); //Convert to Radians, loop uses a +ve pitch = counterclocwise rotation to correct, ie: +ve pitch is forward
	double pitchSin = sinl(CarControl->IMU->pitch*CarControl->TwoPi/360);
	int index = 0;
	while (index < (yvalue).size()) {
		double tempy = (yvalue)[index];
		(yvalue)[index] = pitchCos*tempy + pitchSin*(zvalue)[index];
		(zvalue)[index] = -1*pitchSin*tempy + pitchCos*(zvalue)[index];
		index++;
	}
}

void IBEO::correctForRoll(std::vector<double> &xvalue, std::vector<double> &yvalue, std::vector<double> &zvalue) {
	int index = 0;
	double rollCos = cosl(CarControl->IMU->roll*CarControl->TwoPi/360); //Convert to Radians, +ve roll => roll is CW, +ve roll = ccw rotation to correct
	double rollSin = sinl(CarControl->IMU->roll*CarControl->TwoPi/360);
	while (index < (xvalue).size()) {
		double tempx = (xvalue)[index];
		(xvalue)[index] = rollCos*tempx - rollSin*(zvalue)[index];
		(zvalue)[index] = rollSin*tempx + rollCos*(zvalue)[index];
		index++;
	}
}

void IBEO::rotateForHeading(std::vector<double> &xvalue, std::vector<double> &yvalue, bool edges) {
	int index = 0;
	long double radHeading = CarControl->TwoPi - (CarControl->Fuser)->CurrentHeading*CarControl->TwoPi/360; // matrix rotates counterclockwise, so 360 degrees - heading is the angle to be rotated by, also converts to radians.
	if (!edges) {
		xCoords.clear();
		yCoords.clear();
	}
	while (index < xvalue.size()) {
		double tempx = xvalue[index];
		xvalue[index] = cosl(radHeading)*tempx - sinl(radHeading)*(yvalue)[index]+(CarControl->Fuser)->CurrentPosition.x;
		yvalue[index] = sinl(radHeading)*tempx + cosl(radHeading)*(yvalue)[index]+(CarControl->Fuser)->CurrentPosition.y;
		if (!edges) {
			xCoords.push_back(xvalue[index]);
			yCoords.push_back(yvalue[index]);
		}
		index++;
	}
}

std::vector<double> IBEO::rotateEdges(double x1, double x2, double slope, double inter) {
	std::vector<double> temp = XSlopeIntToXY(x1, x2, slope, inter);
	std::vector<double> xvals(temp.begin(), temp.begin() + 2);
	std::vector<double> yvals(temp.end() - 2, temp.end());
	
	rotateForHeading(xvals, yvals, true);
	
	std::vector<double> ret(xvals);
	ret.insert(ret.end(), yvals.begin(), yvals.end());
	
	return ret;
	
}

/**
 * Purpose: Perform road edge detection.
 * Inputs : None.
 * Outputs: None.
 */
double* IBEO::FindRoad(double AvgLHS, double AvgRHS) {

	int FoundStart = 0;			// State variable.
	int InitialSearchDirn = 0;		// Used to tell which direction the loop is currently searching.
	double Max_r = 0.0;			// Stores peak correlation value during fitting.
	double Peak_i = 0.0;			// Stores i of above.
	int LBoundaryPriorToRSearch = 0;	// Stores L bracket after initial search.

	std::vector<RPOINT> rightr,leftr;

	// Iterators used for bracketing the section being examined.
	std::vector<double>::iterator xfirst;
	std::vector<double>::iterator xlast;
	std::vector<double>::iterator yfirst;
	std::vector<double>::iterator ylast;

	std::vector<double>::iterator RHS_i = CurrentXYScan.xvalues.begin();	// Stores RH bracket after finding R.
	std::vector<double>::iterator LHS_i = CurrentXYScan.xvalues.begin(); 	// Stores final LH bracket after finding L.

	// Number of points included in intial search and minimum for a fit.
	int GroupSize = GROUPSIZE;

	// Index that is iterated over.
	int i = CurrentXYScan.xvalues.size() / 2;


	while(i > 0 && i < CurrentXYScan.xvalues.size()) {

		if((! FoundStart) && InitialSearchDirn == 0) { // Initial search left

			xfirst = CurrentXYScan.xvalues.begin() + i;
			xlast = CurrentXYScan.xvalues.begin() + i-GroupSize;
			yfirst = CurrentXYScan.yvalues.begin() + i;
			ylast = CurrentXYScan.yvalues.begin() + i-GroupSize;

			std::vector<double> groupx(xlast,xfirst);
			std::vector<double> groupy(ylast,yfirst);



			Maths::Regression::Linear fit(GroupSize,groupx.data(), groupy.data());

			if(fabs(fit.getSlope()) < MAX_SLOPE) {

				// Points were found directly to the left so we need to increase the fit right
				// otherwise we want to increase the fit left
				if(i == (CurrentXYScan.xvalues.size() / 2)) { 
					FoundStart = 1; 
					LBoundaryPriorToRSearch = i; 
					i = i + GroupSize + 1; 
				} 
				else { 
					FoundStart = 2; 
					RHS_i = CurrentXYScan.xvalues.begin() + i; 
					i = i - GroupSize - 1; 
				}
	
			}
			else { i--; InitialSearchDirn = 1; } // Interleave left and right searching

		}
		else if((! FoundStart) && InitialSearchDirn == 1) { // Initial search right

			int j = CurrentXYScan.xvalues.size() - i; // j and i are symmetric about the centre

			xfirst = CurrentXYScan.xvalues.begin() + j + GroupSize;
			xlast = CurrentXYScan.xvalues.begin() + j;
			yfirst = CurrentXYScan.yvalues.begin() + j + GroupSize;
			ylast = CurrentXYScan.yvalues.begin() + j;

			std::vector<double> groupx(xlast,xfirst);
			std::vector<double> groupy(ylast,yfirst);


			Maths::Regression::Linear fit(GroupSize,groupx.data(), groupy.data());

			if(fabs(fit.getSlope()) < MAX_SLOPE) {

				// If the points lie directly to the right of the centre then we go straight to increasing the fit right
				// Otherwise we do so starting in the right spot.
				if(j == (CurrentXYScan.xvalues.size() / 2)) { FoundStart = 1; }
				else { FoundStart = 1; i = j + GroupSize + 1; }
	
				LBoundaryPriorToRSearch = j; // Save this so that we can go back and increase the fit left later.

			}
			else { InitialSearchDirn = 0; } // Go back to looking left

		}
		else if(FoundStart == 1) { // Increase fit right

			xfirst = CurrentXYScan.xvalues.begin() + i;
			yfirst = CurrentXYScan.yvalues.begin() + i;

			std::vector<double> groupx(xlast,xfirst);
			std::vector<double> groupy(ylast,yfirst);


			Maths::Regression::Linear fit(groupx.size(),groupx.data(), groupy.data());

			double r = pow(fit.getCoefficient(),2);
							//std::cout << i << "," << CurrentXYScan.xvalues[i] << "," << r << "," << fit.getSlope() << "\n";

			RPOINT fitpoint;
			fitpoint.i = i;
			fitpoint.x = CurrentXYScan.xvalues[i];
			fitpoint.r = r;
			fitpoint.slope = fabs(fit.getSlope());
			if(fitpoint.slope < CUTOFF_SLOPE && fabs(fitpoint.x) < MAX_X && fabs(fitpoint.x) > MIN_X && CurrentXYScan.yvalues[i] < MAX_Y) { rightr.push_back(fitpoint); }


			i++;

			if(i == CurrentXYScan.xvalues.size() || fitpoint.slope > CUTOFF_SLOPE) { // We've reached the RHS edge

				if(rightr.size() == 0) { Peak_i = CurrentXYScan.xvalues.size()/2; }
				else if(rightr.size() > floor((GROUPSIZE-1)/2) + 1) { Peak_i = findrpeak(AvgRHS, rightr); }
				else { Peak_i = rightr[rightr.size()-1].i; }
				
				i = LBoundaryPriorToRSearch; // Restore i
				if(Peak_i != 0) { 	// We found something, set the RHS bracket.
					RHS_i = CurrentXYScan.xvalues.begin() + Peak_i; 
				}
				else {			// We didn't...
					RHS_i = CurrentXYScan.xvalues.begin() + i;					
				}
				xfirst = CurrentXYScan.xvalues.begin() + i;
				yfirst = CurrentXYScan.yvalues.begin() + i;
				i = i - GroupSize;
				FoundStart = 2;		// Increase fit left next
				Max_r = 0; Peak_i = 0;
			}

			

		}
		else if(FoundStart == 2) { // Increase fit left

			xlast = CurrentXYScan.xvalues.begin() + i;
			ylast = CurrentXYScan.yvalues.begin() + i;	

			std::vector<double> groupx(xlast,xfirst);
			std::vector<double> groupy(ylast,yfirst);

			Maths::Regression::Linear fit(groupx.size(),groupx.data(), groupy.data());

			double r = pow(fit.getCoefficient(),2);
						//	std::cout << i << "," << CurrentXYScan.xvalues[i] << "," << r << "," << fit.getSlope() << "\n";
			RPOINT fitpoint;
			fitpoint.i = i;
			fitpoint.x = CurrentXYScan.xvalues[i]; 
			fitpoint.r = r;
			fitpoint.slope = fabs(fit.getSlope());
			if(fitpoint.slope < CUTOFF_SLOPE && fabs(fitpoint.x) < MAX_X && fabs(fitpoint.x) > MIN_X && CurrentXYScan.yvalues[i] < MAX_Y) { leftr.push_back(fitpoint); }

			i--; // Decrement (until we hit the LHS edge and break the while loop)
			if(fitpoint.slope > CUTOFF_SLOPE) { break; }

		}
		

	}

	if(leftr.size() == 0) { Peak_i = CurrentXYScan.xvalues.size()/2 - 1; }
	else if(leftr.size() > floor((GROUPSIZE-1)/2) + 1) { Peak_i = findrpeak(AvgLHS, leftr); }
	else { Peak_i = leftr[0].i;  }

	// Check if we found a better LHS edge, otherwise its the one found in the initial search.
	if(Peak_i != 0) {
		LHS_i = CurrentXYScan.xvalues.begin() + Peak_i; 
	}
	else {
		LHS_i = CurrentXYScan.xvalues.begin() + LBoundaryPriorToRSearch; 
	} 

	if(LHS_i >= RHS_i) { LHS_i = RHS_i - 1; }
	
	std::vector<double>::iterator LHS_y = CurrentXYScan.yvalues.begin() + (LHS_i - CurrentXYScan.xvalues.begin());
	std::vector<double>::iterator RHS_y = CurrentXYScan.yvalues.begin() + (RHS_i - CurrentXYScan.xvalues.begin());

	// Find and fit the entire point from edge to edge.
	std::vector<double> groupx(LHS_i,RHS_i);
	std::vector<double> groupy(CurrentXYScan.yvalues.begin() + (LHS_i - CurrentXYScan.xvalues.begin()),CurrentXYScan.yvalues.begin() + (RHS_i - CurrentXYScan.xvalues.begin()));

	Maths::Regression::Linear fit(groupx.size(),groupx.data(), groupy.data());

	//std::cout << file << "," << *(LHS_i) << "," << *(RHS_i) << "," << fit.getSlope() << "," << fit.getIntercept() << '\n'; 
	
	
	double *tempEdges = new double[2];
    
	tempEdges[0] = (*LHS_i);
	tempEdges[1] = (*RHS_i);
	
	LHSEdgeTemp = *(LHS_i);
	RHSEdgeTemp = *(RHS_i);
	RoadSlopeTemp = fit.getSlope();
	RoadInterceptTemp = fit.getIntercept();

	std::vector<double> temp = rotateEdges(*(LHS_i), *(RHS_i), fit.getSlope(), fit.getIntercept());
	
	rotateForHeading(CurrentXYScan.xvalues, CurrentXYScan.yvalues, false);
	
	while (edgeXs.size() > 1000) { edgeXs.erase(edgeXs.front); }
	while (edgeYs.size() > 1000) { edgeYs.erase(edgeYs.front); }
	edgeXs.push_back(temp.at(0));
	edgeXs.push_back(temp.at(2));
	edgeYs.push_back(temp.at(1));
	edgeYs.push_back(temp.at(3));
	
	return tempEdges;
}

void IBEO::FindRoads() {
	xCoords.clear();
	yCoords.clear();
	
	for (int i = 0; i < layersToScan; i++) {
		PolarToXY(i);
		Edges = FindRoad(LHS[i],RHS[i]);
		if (LHEdge !=0 && RHEdge != 0) {
			LHEdge[i] = LHSEdgeTemp;
			RHEdge[i] = RHSEdgeTemp;
			RoadSlope[i] = RoadSlopeTemp;
			RoadIntercept[i] = RoadInterceptTemp;
		}
		//Log->WriteLogLine(boost::lexical_cast<std::string>(LHSEdgeTemp));
		//Log->WriteLogLine(boost::lexical_cast<std::string>(RHSEdgeTemp));
		if(Edges[0] < MIN_X) { KlmL[i].SetMeasurements(Edges[0], 0,0); }
		KlmL[i].Update();
		if(Edges[1] > MIN_X) { KlmR[i].SetMeasurements(Edges[1], 0,0); }
		KlmR[i].Update();
		LHS[i] = KlmL[i].GetPositionEstimate();
		RHS[i] = KlmR[i].GetPositionEstimate();
	}
}
