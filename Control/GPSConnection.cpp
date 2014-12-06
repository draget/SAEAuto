//
//  GPSConnection.cpp
//
//
//  Created by Ruvan Muthu-Krishna on 15/09/2014.
//


#include "GPSConnection.h"
#include "Logger.h"
#include "Control.h"
#include "Fusion.h"

#include <iostream>
#include <boost/thread.hpp> 
#include <boost/lexical_cast.hpp>
#include <exception>
#include <vector>

std::string msgType;
int sockfd, n;
int portno = 50007;
struct sockaddr_in serv_addr;
struct hostent *server;
char buffer[250];

using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

/**
 * Purpose: Creates a new instance of the GPSConnection object.
 * Inputs : None.
 * Outputs: None.
 */
GPSConnection::GPSConnection(Control* CarController, Logger* Logger) {

	CarControl = CarController;
 	Log = Logger;	

	GPSState = false;
	Run = false;
	baselinePosNEDOldTime = 0;

}

/**
 * Purpose: Creates a new instance of the GPSConnection object.
 * Inputs : An Network object.
 * Outputs: None.
 */
GPSConnection::GPSConnection(const GPSConnection& orig) {
}

/**
 * Purpose: Destroys the instance of the GPSConnection object.
 * Inputs : None.
 * Outputs: None.
 */
GPSConnection::~GPSConnection() {
	close(sockfd); // Might error if a sockfd doesn't exist
}

bool GPSConnection::Open() {

	std::string LogPath = CarControl->LogDir + "/gps.log";

	GPSLog = new Logger(LogPath.c_str());
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
		Log->WriteLogLine("GPS - Error opening socket.");
		CarControl->Trip(7);
		return false;
	}
	
    server = gethostbyname("127.0.0.1");
    
    if (server == NULL) {
		Log->WriteLogLine("GPS - Error no such host.");
		CarControl->Trip(7);
		return false;
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		Log->WriteLogLine("GPS - Error connecting.");
		CarControl->Trip(7);
		return false;
    }
		
	GPSState = true;
	return true;

}

/**
 * Purpose: Start the GPS listening and monitoring threads
 * Inputs : None.
 * Outputs: None.
 */
void GPSConnection::Start() {
    
	if(GPSState) {	

		Run = true;

		m_Thread = boost::thread(&GPSConnection::ProcessMessages, this);
		m_Thread.detach();

		s_Thread = boost::thread(&GPSConnection::Monitor, this);
		s_Thread.detach();

	}
        
}

/**
 * Purpose: Stop the GPS listening and monitoring threads as well as close the log file
 * Inputs : None.
 * Outputs: None.
 */
void GPSConnection::Stop() {

	Run = false;
	GPSLog->CloseLog();
	sleep(1);

}

/**
 * Purpose: Read and parse the messages sent from the GPS deamon.
 * Inputs : None.
 * Outputs: None.
 */
void GPSConnection::ProcessMessages() {
	
	while(Run)
    {
		
		bzero(buffer,250); // zero out the buffer
        n = read(sockfd,buffer,250); // read from the socket
        if (n < 0) {
            Log->WriteLogLine("GPS - Error reading from socket.");
			CarControl->Trip(7);
			return;
        }
        std::istringstream iss (buffer);
        iss >> msgType;
        // Figure out what type of message was sent and update the appropriate struct
        if (msgType == "blNED") { // Baseline NED solution
			iss >> baselinePosNED.tow;
            iss >> baselinePosNED.n;
            iss >> baselinePosNED.e;
            iss >> baselinePosNED.d;
            iss >> baselinePosNED.h_accuracy;
            iss >> baselinePosNED.v_accuracy;
            iss >> baselinePosNED.n_sats;
            iss >> baselinePosNED.flags;
            
            Log->WriteLogLine("GPS - Baseline Pos - N:" + boost::lexical_cast<std::string>(baselinePosNED.n) + " E:" + boost::lexical_cast<std::string>(baselinePosNED.e) + " D:" + boost::lexical_cast<std::string>(baselinePosNED.d) + " -- FixMode: " + boost::lexical_cast<std::string>(baselinePosNED.flags) + " \n");
			CarControl->Fuser->GPSUpdate(baselinePosNED, baselineVelNED); // This line is repeated in the velNED message section
			
			// Work out lat long based on relative position
			VECTOR_2D blPosNED = CarControl->XYToLatLong(baselinePosNED.e,baselinePosNED.n);
			Latitude = blPosNED.x;
			Longitude = blPosNED.y;
			
        } else if (msgType == "velNED") { // Velocity NED solution
			iss >> baselineVelNED.tow;
            iss >> baselineVelNED.n;
            iss >> baselineVelNED.e;
            iss >> baselineVelNED.d;
            iss >> baselineVelNED.h_accuracy;
            iss >> baselineVelNED.v_accuracy;
            iss >> baselineVelNED.n_sats;
            iss >> baselineVelNED.flags;
            
	        // Work out track angle based on NED velocity
	        if (baselineVelNED.n >= 0) {
				if (baselineVelNED.e >= 0) {
					TrackAngle = (atan(baselineVelNED.e/baselineVelNED.n)* 180 / PI);
				} else {
					TrackAngle = 360.0 - (atan(abs(baselineVelNED.e)/baselineVelNED.n)* 180 / PI);
				}
			} else {
				if (baselineVelNED.e >= 0) {
					TrackAngle = 180.0 - (atan(baselineVelNED.e/abs(baselineVelNED.n))* 180 / PI);
				} else {
					TrackAngle = 180.0 + (atan(abs(baselineVelNED.e)/abs(baselineVelNED.n))* 180 / PI);
				}
			}
			
			Log->WriteLogLine("GPS Vel - N:" + boost::lexical_cast<std::string>(baselineVelNED.n) + " E:" + boost::lexical_cast<std::string>(baselineVelNED.e) + " T:" + boost::lexical_cast<std::string>(TrackAngle) + " \n");
			NewTrack();
			Speed = sqrt(baselineVelNED.e*baselineVelNED.e + baselineVelNED.n*baselineVelNED.n)*0.0036; // convert to km/h
			CarControl->Fuser->GPSUpdate(baselinePosNED, baselineVelNED); // Send position and velocity update to fuser
			
        } else if (msgType == "posLLH") { // Position in Lat Lon Height format. Not using RTK solution.
            iss >> llhPos.tow;
            iss >> llhPos.lat;
            iss >> llhPos.lon;
            iss >> llhPos.height;
            iss >> llhPos.h_accuracy;
            iss >> llhPos.v_accuracy;
            iss >> llhPos.n_sats;
            iss >> llhPos.flags;
            Log->WriteLogLine("GPS - LLH Pos - lat:" + boost::lexical_cast<std::string>(llhPos.lat) + " Lon:" + boost::lexical_cast<std::string>(llhPos.lon) + " \n");
        
			
		} else if (msgType == "GPS_TIME") { // Ignoring time stuff at the moment
			iss >> gpsTime.wn;
			iss >> gpsTime.tow;
			iss >> gpsTime.ns;
			iss >> gpsTime.flags;
		}
    }
}

/**
 * Purpose: Send new track angle to fuser
 * Inputs : None.
 * Outputs: None.
 */
void GPSConnection::NewTrack() {

	CarControl->Fuser->GPSTrackAngleUpdate(TrackAngle);

	if(CarControl->ExtLogging) {
		GPSLog->WriteLogLine("T," + boost::lexical_cast<std::string>(CarControl->TimeStamp()) + "," + boost::lexical_cast<std::string>(TrackAngle), true);
	}
}

/**
 * Purpose: Poll last GPS timestamp every 2 seconds, if it's the same as the one seen previously then enter a trip state.
 * Inputs : None.
 * Outputs: None.
 */
void GPSConnection::Monitor() {

	while(Run) {
		if(CarControl->AutoRun) {
			if(gpsTime.tow == oldTime) { // Check that we're still getting messages from the GPS
				Log->WriteLogLine("GPS - GPS Fix is old."); 
				CarControl->Trip(7); 
			} 
			if(baselinePosNED.tow == baselinePosNEDOldTime || baselinePosNEDOldTime == 0 ) { // Check that if in auto, we've recieved a baseline update in the last n seconds
				Log->WriteLogLine("GPS - Baseline data is old or doesn't exist"); 
				CarControl->Trip(7); 
			}
		}

		oldTime = gpsTime.tow;
		baselinePosNEDOldTime = baselinePosNED.tow;
		boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
	}

}


