/* 
 * File:   GPSConnection.cpp
 * Author: T. Drage
 * 
 * A wrapper for the GPSD C++ API.
 *
 * Created on 23/7/13
 */
extern "C" {
#include "Piksi_Read.h"
}

#include <iostream>
#include <boost/thread.hpp> 
#include <boost/lexical_cast.hpp>
#include <exception>
#include <vector>

#include <string>

#include "libgpsmm.h"

#include "GPSConnection.h"
#include "Logger.h"
#include "Control.h"
#include "Fusion.h"

using namespace std;

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
	PI = boost::math::constants::pi<double>();

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
}

bool GPSConnection::Open() {

	std::string LogPath = CarControl->LogDir + "/gps.log";

	GPSLog = new Logger(LogPath.c_str());

	fd = OpenPiksi(PIKSI_PATH, PIKSI_BAUD);

	if (fd < 0){
	Log->WriteLogLine("Piksi - GPS failed to open...");
	GPSState = false;
	} else {
	Log->WriteLogLine("Piksi - GPS successfully opened and initialized");
	GPSState = true;
	}

	return true;

}

void GPSConnection::Start() {
    
	if(GPSState) {	

		Run = true;

		m_Thread = boost::thread(&GPSConnection::ProcessMessages, this);
		m_Thread.detach();

		s_Thread = boost::thread(&GPSConnection::Monitor, this);
		s_Thread.detach();

	}
        
}



void GPSConnection::ProcessMessages() {
	int nullcount = 0;

	while(Run) {

    		ProcessPiksi();
    		
		if (pos_llh.lat == NULL) {
			nullcount += 1;
			if (nullcount == 1000) {
	    		Log->WriteLogLine("GPS - Read error! No Fix.");
			}
		} else {
			nullcount = 0;
			Time = gps_time.tow;
			if (vel_ned.n > 0) {
				if (vel_ned.e > 0) {
					TrackAngle = (atan(vel_ned.e/vel_ned.n)* 180 / PI);
				} else {
					TrackAngle = 360.0 + (atan(vel_ned.e/vel_ned.n)* 180 / PI);
				}
			} else if (vel_ned.n < 0) {
				if (vel_ned.e > 0) {
					TrackAngle = 180.0 + (atan(vel_ned.e/vel_ned.n)* 180 / PI);
				} else {
					TrackAngle = 180.0 + (atan(vel_ned.e/vel_ned.n)* 180 / PI);
				}
			}
			
			NewTrack();
			Speed = sqrt(vel_ned.e*vel_ned.e + vel_ned.n*vel_ned.n)*0.001; // convert to m/s

			Latitude = pos_llh.lat - CarControl->LatOffset;
			Longitude = pos_llh.lon - CarControl->LongOffset;
			NewSpeedAndPosition();	
		}
    	}

}

void GPSConnection::Stop() {

	Run = false;
	GPSLog->CloseLog();
	sleep(1);

}

void GPSConnection::NewSpeedAndPosition() {

	VECTOR_2D Position = CarControl->LatLongToXY(Latitude, Longitude);

	if(CarControl->ExtLogging) {
		GPSLog->WriteLogLine("P," + boost::lexical_cast<std::string>(CarControl->TimeStamp()) + "," + boost::lexical_cast<std::string>(Latitude) + "," + boost::lexical_cast<std::string>(Longitude) + "," + boost::lexical_cast<std::string>(Position.x) + "," + boost::lexical_cast<std::string>(Position.y), true);
		GPSLog->WriteLogLine("S," + boost::lexical_cast<std::string>(CarControl->TimeStamp()) + "," + boost::lexical_cast<std::string>(Speed), true);
	}	

	CarControl->Fuser->GPSUpdate(Position, Speed);

}

void GPSConnection::NewTrack() {

	CarControl->Fuser->GPSTrackAngleUpdate(TrackAngle);

	if(CarControl->ExtLogging) {
		GPSLog->WriteLogLine("T," + boost::lexical_cast<std::string>(CarControl->TimeStamp()) + "," + boost::lexical_cast<std::string>(TrackAngle), true);
	}
}

void GPSConnection::Monitor() {

	while(Run) {

		//Log->WriteLogLine("old " + boost::lexical_cast<std::string>(OldTime) + " new " + boost::lexical_cast<std::string>(Time));

		if(CarControl->AutoRun) {
			if(! (Time > (OldTime + 0.1))) { Log->WriteLogLine("GPS - GPS Fix is old."); CarControl->Trip(7); } 
		}

		OldTime = Time;

		boost::this_thread::sleep(boost::posix_time::milliseconds(500));
	}

}


