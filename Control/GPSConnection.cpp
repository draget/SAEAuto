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

#define PIKSI_PATH "/dev/serial/by-id/usb-FTDI_Single_RS232-HS-if00-port0"
#define QSTAR_PATH "/dev/serial/by-id/usb-FTDI_Single_RS232-HS-if00-port0"

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
	UsingGPSD = false;
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

	if (fd < 0) {
		Log->WriteLogLine("No Piksi Found - Attempting GPSD...");
		GPSDFound();
	} else {
		Log->WriteLogLine("Piksi - GPS successfully opened and initialized");
		UsingGPSD = false;
		GPSState = true;
	}

	return true;
}

void GPSConnection::GPSDFound() {

	GPSReceiver = new gpsmm("localhost", DEFAULT_GPSD_PORT);

   	if (GPSReceiver->stream(WATCH_ENABLE|WATCH_JSON) == NULL) {
   	    Log->WriteLogLine("QStar - No GPSD running.");
		CarControl->Trip(7);
		GPSState = false;

	} else {
		Log->WriteLogLine("QStar - GPSD Running.");
		UsingGPSD = true;
		GPSState = true;
	}
}

void GPSConnection::Start() {

	if(GPSState) {

		Run = true;

		if (! UsingGPSD) {
			m_Thread = boost::thread(&GPSConnection::ProcessMessages, this);
		} else {
			m_Thread = boost::thread(&GPSConnection::ProcessMessagesDep, this);
		}
		m_Thread.detach();


		s_Thread = boost::thread(&GPSConnection::Monitor, this);
		s_Thread.detach();

	}

}

void GPSConnection::ProcessMessages() {
	int nullcount = 0;
	NumSat = 0;

	while(Run) {

	    ProcessPiksi();
		NumSat = pos_llh.n_sats;
		if (NumSat == 0) {
			nullcount += 1;
			if (nullcount == 1000) {
	    			Log->WriteLogLine("GPS - Read error! No Fix.");
			}
		} else {
			nullcount = 0;
		}
		Time = gps_time.tow;
		velocity_n = double (vel_ned.n);
		velocity_e = double (vel_ned.e);
		if (vel_ned.n > 0) {
			if (vel_ned.e > 0) {
				TrackAngle = (atan(velocity_e/velocity_n)* 180 / PI);
			} else {
				TrackAngle = 360.0 + (atan(velocity_e/velocity_n)* 180 / PI);
			}
		} else if (vel_ned.n < 0) {
			if (vel_ned.e > 0) {
				TrackAngle = 180.0 + (atan(velocity_e/velocity_n)* 180 / PI);
			} else {
				TrackAngle = 180.0 + (atan(velocity_e/velocity_n)* 180 / PI);
			}
		} else {
			TrackAngle = 0;
		}

		NewTrack();
		Speed = sqrt(vel_ned.e*vel_ned.e + vel_ned.n*vel_ned.n)*0.001; // convert to m/s

		Latitude = pos_llh.lat - CarControl->LatOffset;
		Longitude = pos_llh.lon - CarControl->LongOffset;
//		Log->WriteLogLine("PositionUpdated:  " + boost::lexical_cast<std::string>(Latitude));

		NewSpeedAndPosition();
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	}

}

void GPSConnection::ProcessMessagesDep() {

	while(Run) {

		struct gps_data_t* NewData;

		if (! GPSReceiver->waiting(10000)) { continue; }

		if ((NewData = GPSReceiver->read()) == NULL) {
	    	Log->WriteLogLine("GPS - Read error! - Will attempt again soon...");
			boost::this_thread::sleep(boost::posix_time::milliseconds(10000));
			continue;
		}
		else {
			if (NewData->set & TIME_SET) {
				Time = NewData->fix.time;
			}
			if(NewData->set & TRACK_SET) {
				TrackAngle = NewData->fix.track;
				NewTrack();
			}
			if((NewData->set & LATLON_SET) && (NewData->set & SPEED_SET)) {
				Latitude = NewData->fix.latitude - CarControl->LatOffset;
				Longitude = NewData->fix.longitude - CarControl->LongOffset;
				Speed = NewData->fix.speed;
				NewSpeedAndPosition();
			}

			NumSat = NewData->satellites_used;
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

	CarControl->Fuser->GPSUpdate(Position, Speed, NumSat);

}

void GPSConnection::NewTrack() {

	CarControl->Fuser->GPSTrackAngleUpdate(TrackAngle, NumSat);

	if(CarControl->ExtLogging) {
		GPSLog->WriteLogLine("T," + boost::lexical_cast<std::string>(CarControl->TimeStamp()) + "," + boost::lexical_cast<std::string>(TrackAngle), true);
	}
}

void GPSConnection::Monitor() {

	while(Run) {

		//Log->WriteLogLine("old " + boost::lexical_cast<std::string>(OldTime) + " new " + boost::lexical_cast<std::string>(Time));

		if(CarControl->AutoRun) {
			if(! (Time > (OldTime + 1000))) { Log->WriteLogLine("GPS - GPS Fix is old."); } 
		}

		OldTime = Time;

		boost::this_thread::sleep(boost::posix_time::milliseconds(500));
	}
}


