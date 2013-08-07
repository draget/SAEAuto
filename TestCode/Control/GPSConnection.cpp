/* 
 * File:   GPSConnection.cpp
 * Author: T. Drage
 * 
 * A wrapper for the GPSD C++ API.
 *
 * Created on 23/7/13
 */


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

	GPSReceiver = new gpsmm("localhost", DEFAULT_GPSD_PORT);

    	if (GPSReceiver->stream(WATCH_ENABLE|WATCH_JSON) == NULL) {
    	    	Log->WriteLogLine("GPS - No GPSD running.");
		CarControl->Trip(7);
        	return false;
   	}

	GPSState = true;
	return true;

}

void GPSConnection::Start() {
    
	if(GPSState) {	

		Run = true;

		m_Thread = boost::thread(&GPSConnection::ProcessMessages, this);
		m_Thread.detach();

	}
        
}



void GPSConnection::ProcessMessages() {

	while(Run) {

		struct gps_data_t* NewData;

		if (! GPSReceiver->waiting(10000)) { continue; }

		if ((NewData = GPSReceiver->read()) == NULL) {
	    		Log->WriteLogLine("GPS - Read error!");
			CarControl->Trip(7);
		} 
		else {
			if (NewData->set & TIME_SET) {
				// Do something to make sure GPS data isn't terribly old!
			}
			if(NewData->set & LATLON_SET) {
				Latitude = NewData->fix.latitude;
				Longitude = NewData->fix.longitude;
				NewPosition();

			}
			if(NewData->set & SPEED_SET) { 
				Speed = NewData->fix.speed;
				NewSpeed();
			}
			if(NewData->set & TRACK_SET) {
				TrackAngle = NewData->fix.track;
				NewTrack();
			}
			
		}
    }

}

void GPSConnection::Stop() {

	Run = false;
	GPSLog->CloseLog();
	sleep(1);

}

void GPSConnection::NewPosition() {


	GPSLog->WriteLogLine("P," + boost::lexical_cast<std::string>(CarControl->TimeStamp()) + "," + boost::lexical_cast<std::string>(Latitude) + "," + boost::lexical_cast<std::string>(Longitude), true);
	if(CarControl->AutoRun) { CarControl->AutoPosUpdate(CarControl->LatLongToXY(Latitude, Longitude)); }

}

void GPSConnection::NewSpeed() {


	GPSLog->WriteLogLine("S," + boost::lexical_cast<std::string>(CarControl->TimeStamp()) + "," + boost::lexical_cast<std::string>(Speed), true);
	if(CarControl->AutoRun) { CarControl->AutoSpeedUpdate(Speed); }

}

void GPSConnection::NewTrack() {


	GPSLog->WriteLogLine("T," + boost::lexical_cast<std::string>(CarControl->TimeStamp()) + "," + boost::lexical_cast<std::string>(TrackAngle), true);
	if(CarControl->AutoRun) { CarControl->AutoTrackUpdate(TrackAngle); }

}



