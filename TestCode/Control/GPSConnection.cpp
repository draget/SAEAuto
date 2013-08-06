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

			}
			if(NewData->set & SPEED_SET) { 
				Speed = NewData->fix.speed;

			}
			if(NewData->set & TRACK_SET) {// Log->WriteLogLine("track"); 
				TrackAngle = NewData->fix.track;

			}
			if(NewData->set & SPEED_SET & LATLON_SET) { NewState(); }
		}
    }

}

void GPSConnection::Stop() {

	Run = false;
	GPSLog->CloseLog();
	sleep(1);

}

void GPSConnection::NewState() {

	timeval current;
	gettimeofday(&current,NULL);

	GPSLog->WriteLogLine(boost::lexical_cast<std::string>(current.tv_sec + current.tv_usec/1000000) + "," + boost::lexical_cast<std::string>(Latitude) + "," + boost::lexical_cast<std::string>(Longitude) + "," + boost::lexical_cast<std::string>(Speed) + "," + boost::lexical_cast<std::string>(TrackAngle), true);


}



