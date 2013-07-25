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

	GPSReceiver = new gpsmm("localhost", DEFAULT_GPSD_PORT);

    	if (GPSReceiver->stream(WATCH_ENABLE|WATCH_JSON) == NULL) {
    	    	Log->WriteLogLine("GPS - No GPSD running.");
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
		} 
		else {
			if(NewData->set & LATLON_SET) {
				Latitude = NewData->fix.latitude;
				Longitude = NewData->fix.longitude;
			}
			if(NewData->set & SPEED_SET) { 
				Speed = NewData->fix.speed;
			}
		}
    }

}

void GPSConnection::Stop() {

	Run = false;
	sleep(1);

}



