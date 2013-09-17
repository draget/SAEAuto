/* 
 * File:   IBEO.cpp
 * Author: Timothy Black (20373081), modifications Thomas Drage (20510505)
 * 
 * Created on 18 April 2012, 7:34 PM
 * Modified 3 March 2013, 23 July 2013 (T.Drage).
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

#include "linear.h"

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

/**
 * Purpose: Creates a new instance of the IBEO object.
 * Inputs : None.
 * Outputs: None.
 */
IBEO::IBEO(Control* CarController, Logger* Logger) {

	gettimeofday(&lastwrite,NULL);

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
	Log->WriteLogLine("IBEO scanner - Connecting failed.");
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
                    if (verbose) Log->WriteLogLine("ibeo scanner: received unknown message with id " + htons(header.data_type));
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

void IBEO::Start() {

	if(inUse) {	

		Run = true;

		m_Thread = boost::thread(&IBEO::ProcessMessages, this);
		m_Thread.detach();

	}

}

void IBEO::ProcessMessages() {

	VECTOR_2D Object;
	VECTOR_2D TransformedObject;

	while(Run) {

		ReadMessages();


		timeval current;
		gettimeofday(&current,NULL);
		
		if((current.tv_sec + ((double)current.tv_usec)/1000000) > (lastwrite.tv_sec + ((double)lastwrite.tv_usec)/1000000) + 0.2) { 

			CurrentXYScan.xvalues.clear();
			CurrentXYScan.yvalues.clear();

			double angle;
			double dist;

			for(int i = 0; i < scan_data_header[curScanDataSource].scan_points; i++) {
				angle = (scan_data_points[curScanDataSource][i].horiz_angle+2880)*CarControl->TwoPi/11520;
				dist = scan_data_points[curScanDataSource][i].radial_dist/100.0;
				CurrentXYScan.xvalues.push_back(cos(angle)*dist);
				CurrentXYScan.yvalues.push_back(sin(angle)*dist);
			}

			FindRoad();

			WriteFiles(current); 
	
			for(int i = 0; i < object_data_header[curObjectDataSource].number_of_objects; i++) {
			
				Object.x = (double)object_data[curObjectDataSource][i].reference_point.x/100.0; 
				Object.y = (double)object_data[curObjectDataSource][i].reference_point.y/100.0;

				// Ignore objects that make up the road.
				if(CarControl->VectorMagnitude(Object) > OBJECT_THRESHOLD) { continue; }
			
				TransformedObject.x = -cos(-1*CarControl->Fuser->CurrentHeading) * Object.y + sin(-1*CarControl->Fuser->CurrentHeading) * Object.x + CarControl->Fuser->CurrentPosition.x;
				TransformedObject.y = sin(-1*CarControl->Fuser->CurrentHeading) * Object.y + cos(-1*CarControl->Fuser->CurrentHeading) * Object.x + CarControl->Fuser->CurrentPosition.y;

				bool Found = false;

				BOOST_FOREACH( VECTOR_2D MapPoint, CarControl->CurrentMap.DetectedFenceposts ) {
					if(CarControl->VectorMagnitude(CarControl->SubtractVector(MapPoint,TransformedObject)) < 0.2) { Found = true; break; }
				}

				if(! Found) { CarControl->CurrentMap.DetectedFenceposts.push_back(TransformedObject); }

			}




		}
		

	}

}


void IBEO::WriteFiles(timeval current) {

		if(CarControl->ExtLogging) {
			rename("./ramdisk/current.lux", (CarControl->LogDir + "/lux/" + boost::lexical_cast<std::string>(current.tv_sec + ((double)current.tv_usec)/1000000) + ".lux").c_str());
			rename("./ramdisk/current.luxobj", (CarControl->LogDir + "/luxobj/" + boost::lexical_cast<std::string>(current.tv_sec + ((double)current.tv_usec)/1000000) + ".luxobj").c_str());
		}


		ofstream outfile_scan;

		std::string FileName = "./ramdisk/current.lux";

		outfile_scan.open(FileName.c_str(), ios::trunc);

		for(int i = 0; i < scan_data_header[curScanDataSource].scan_points; i++) {
			outfile_scan << (int)scan_data_points[curScanDataSource][i].layer_echo << "," << (int)scan_data_points[curScanDataSource][i].flags << "," << scan_data_points[curScanDataSource][i].horiz_angle << "," << scan_data_points[curScanDataSource][i].radial_dist << "," << scan_data_points[curScanDataSource][i].echo_pulse_width << "," << scan_data_points[curScanDataSource][i].res << "\n";
		}

		outfile_scan << "R," << LHEdge << "," << RHEdge << "," << RoadSlope << "," << RoadIntercept;
		
		outfile_scan.close();

		ofstream outfile_obj;

		FileName = "./ramdisk/current.luxobj";

		outfile_obj.open(FileName.c_str(), ios::trunc);

		for(int i = 0; i < object_data_header[curObjectDataSource].number_of_objects; i++) {
			outfile_obj << (int)object_data[curObjectDataSource][i].object_id << "," << (int)object_data[curObjectDataSource][i].reference_point.x << "," << object_data[curObjectDataSource][i].reference_point.y << "," << (int)object_data[curObjectDataSource][i].closest_point.x << "," << object_data[curObjectDataSource][i].closest_point.y << "," << object_data[curObjectDataSource][i].classification << "\n";
		}
		
		outfile_obj.close();

}


void IBEO::FindRoad() {

	int FoundStart = 0;
	int InitialSearchDirn = 0;
	double Max_r = 0.0;
	double Peak_i = 0.0;
	int LBoundaryPriorToRSearch = 0;

	double Slope = 0;
	double Intercept = 0;

	std::vector<double>::iterator xfirst;
	std::vector<double>::iterator xlast;
	std::vector<double>::iterator yfirst;
	std::vector<double>::iterator ylast;


	int GroupSize = 8;

	unsigned int i = CurrentXYScan.xvalues.size() / 2;
	while(i > 0 && i < CurrentXYScan.xvalues.size()) {

		if((! FoundStart) && InitialSearchDirn == 0) { // Initial search left

			xfirst = CurrentXYScan.xvalues.begin() + i;
			xlast = CurrentXYScan.xvalues.begin() + i-GroupSize;
			yfirst = CurrentXYScan.yvalues.begin() + i;
			ylast = CurrentXYScan.yvalues.begin() + i-GroupSize;

			std::vector<double> groupx(xlast,xfirst);
			std::vector<double> groupy(ylast,yfirst);

			Maths::Regression::Linear fit(GroupSize,groupx.data(), groupy.data());

			if(fabs(fit.getSlope()) < 0.5) {

				// Points were found directly to the left so we need to increase the fit right
				// otherwise we want to increase the fit left
				if(i == (CurrentXYScan.xvalues.size() / 2)) { FoundStart = 1; LBoundaryPriorToRSearch = i; } 
				else { FoundStart = 2; i = i - GroupSize - 1; }
	
			}
			else { i--; InitialSearchDirn = 1; } // Interleave left and right searching

		}
		else if((! FoundStart) && InitialSearchDirn == 1) { // Initial search right

			unsigned int j = CurrentXYScan.xvalues.size() - i; // j and i are symmetric about the centre

			xfirst = CurrentXYScan.xvalues.begin() + j + GroupSize;
			xlast = CurrentXYScan.xvalues.begin() + j;
			yfirst = CurrentXYScan.yvalues.begin() + j + GroupSize;
			ylast = CurrentXYScan.yvalues.begin() + j;

			std::vector<double> groupx(xlast,xfirst);
			std::vector<double> groupy(ylast,yfirst);

			Maths::Regression::Linear fit(GroupSize,groupx.data(), groupy.data());

			if(fabs(fit.getSlope()) < 0.5) {

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


			if(r > Max_r) { Max_r = r; Peak_i = i; };

			i++;

			if(i == CurrentXYScan.xvalues.size()) { // We've reached the RHS edge
				i = LBoundaryPriorToRSearch; // Restore i
				if(Max_r > 0.4) { 	// We found something, set the LHS bracket.
					xfirst = CurrentXYScan.xvalues.begin() + Peak_i;
					yfirst = CurrentXYScan.yvalues.begin() + Peak_i;
				}
				else {			// We didn't...
					xfirst = CurrentXYScan.xvalues.begin() + i;
					yfirst = CurrentXYScan.yvalues.begin() + i;
				}
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

			if(r > Max_r) { 
				Max_r = r; 
				Peak_i = i; 			
				Slope = fit.getSlope();
				Intercept = fit.getIntercept(); 
			}

			i--; // Decrement (until we hit the LHS edge and break the while loop)

		}
		

	}

	if(Max_r > 0.4) {  
		LHEdge =  *(xlast + Peak_i-1); 
		RHEdge = *(xfirst-1); 
		RoadSlope = Slope; 
		RoadIntercept = Intercept; 
	}
	else {
		LHEdge =  0; 
		RHEdge = 0; 
		RoadSlope = 0; 
		RoadIntercept = 0; 
	}


}
