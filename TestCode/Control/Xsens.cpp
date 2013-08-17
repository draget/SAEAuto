/* 
 * File:   Xsens.cpp
 * Author: T. Drage
 * 
 * A wrapper for the Xsense CMT3 API.
 *
 * Created on 12/8/13
 */



#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>
#include <sys/time.h>

#include "Xsens.h"
#include "Logger.h"
#include "Control.h"



#define EXIT_ERROR(loc) {printf("Error %d occurred during " loc ": %s\n", serial.getLastResult(), xsensResultText(serial.getLastResult())); exit(-1); }


Xsens::Xsens(Control* CarController, Logger* Logger) {

	serial = new Cmt3;

	reply = new Packet(1,0); /* 1 item, not xbus */

	CarControl = CarController;
 	Log = Logger;	

	IMUState = false;

}

/**
 * Purpose: Creates a new instance of the GPSConnection object.
 * Inputs : An Network object.
 * Outputs: None.
 */
Xsens::Xsens(const Xsens& orig) {
}

/**
 * Purpose: Destroys the instance of the GPSConnection object.
 * Inputs : None.
 * Outputs: None.
 */
Xsens::~Xsens() {
}

bool Xsens::Open() {

	char portname[32];
	sprintf(portname, IMU_PORT);

	if (serial->openPort(portname, B115200) != XRV_OK) { Log->WriteLogLine("XSens - Couldn't open IMU port."); }
	else {

		//set the measurement timeout to 100ms (default is 16ms)
		int timeOut = 100;
		if (serial->setTimeoutMeasurement(timeOut) != XRV_OK) { Log->WriteLogLine("XSens - Couldn't set timeout."); return false; }
	
    		CmtDeviceMode mode(CMT_OUTPUTMODE_ORIENT | CMT_OUTPUTMODE_CALIB, CMT_OUTPUTSETTINGS_TIMESTAMP_SAMPLECNT | CMT_OUTPUTSETTINGS_COORDINATES_NED |CMT_OUTPUTSETTINGS_ORIENTMODE_EULER | CMT_OUTPUTSETTINGS_CALIBMODE_ACCGYR, 100);
    		if (serial->setDeviceMode(mode, false, CMT_DID_BROADCAST)) { Log->WriteLogLine("XSens - Failed to set device mode."); return false; }

		if (serial->gotoMeasurement()) { Log->WriteLogLine("XSens - Failed to set device mode."); return false; }

		IMUState = true;

		Log->WriteLogLine("XSens - IMU Connected & configured.");

		return true;

	}

	return false;

}


void Xsens::Start() {
    
	if(IMUState) {	

		Run = true;

		m_Thread = boost::thread(&Xsens::ProcessMessages, this);
		m_Thread.detach();

	}
        
}

void Xsens::ProcessMessages() {


    while (Run)
    {
        if (serial->waitForDataMessage(reply) != XRV_OK)
        {
		Log->WriteLogLine("XSens - Error reading messages!");
        }

	long double cos_roll = cosl((long double)reply->getOriEuler().m_roll*2*M_PI/360);
	long double cos_pitch = cosl((long double)reply->getOriEuler().m_pitch*2*M_PI/360);
	long double sin_roll = sinl((long double)reply->getOriEuler().m_roll*2*M_PI/360);
	long double sin_pitch = sinl((long double)reply->getOriEuler().m_pitch*2*M_PI/360);

	long double xacc = (long double) reply->getCalData().m_acc.m_data[0];
	long double yacc = (long double) reply->getCalData().m_acc.m_data[1];
	long double zacc = (long double) reply->getCalData().m_acc.m_data[2];

	long double xacc_comp = (long double) cos_pitch*xacc + sin_pitch*zacc;
	long double yacc_comp = (long double) sin_pitch*sin_roll*xacc + cos_roll*yacc - sin_roll*cos_pitch*zacc;
	long double zacc_comp = (long double) -cos_roll*sin_pitch*xacc + sin_roll*yacc + cos_roll*cos_pitch*zacc;

	if(reply->getOriEuler().m_yaw < 0) { Yaw = 360 + reply->getOriEuler().m_yaw ; }
	else { Yaw = reply->getOriEuler().m_yaw; }

    }

}

