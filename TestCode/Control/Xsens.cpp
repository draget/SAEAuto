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

#include <boost/lexical_cast.hpp>


#define EXIT_ERROR(loc) {printf("Error %d occurred during " loc ": %s\n", serial.getLastResult(), xsensResultText(serial.getLastResult())); exit(-1); }


Xsens::Xsens(Control* CarController, Logger* Logger) {

	serial = new Cmt3;

	reply = new Packet(1,0); /* 1 item, not xbus */

	Yaw = 0;

	VECTOR_2D ZeroVec;
	ZeroVec.x = 0.0;
	ZeroVec.y = 0.0;

	Accelerations.resize(20,ZeroVec);

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

	std::string LogPath = CarControl->LogDir + "/imu.log";

	IMULog = new Logger(LogPath.c_str());

	char portname[32];
	sprintf(portname, IMU_PORT);

	if (serial->openPort(portname, B115200) != XRV_OK) { Log->WriteLogLine("XSens - Couldn't open IMU port."); }
	else {

		//set the measurement timeout to 100ms (default is 16ms)
		int timeOut = 100;
		if (serial->setTimeoutMeasurement(timeOut) != XRV_OK) { Log->WriteLogLine("XSens - Couldn't set timeout."); return false; }
	
    		CmtDeviceMode mode(CMT_OUTPUTMODE_ORIENT | CMT_OUTPUTMODE_CALIB, CMT_OUTPUTSETTINGS_TIMESTAMP_SAMPLECNT | CMT_OUTPUTSETTINGS_COORDINATES_NED |CMT_OUTPUTSETTINGS_ORIENTMODE_MATRIX | CMT_OUTPUTSETTINGS_CALIBMODE_ACCGYR, 100);
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

	CmtMatrix matrix_data;
	long double xacc;
	long double yacc;
	long double zacc;
	long double xacc_comp;
	long double yacc_comp;
	long double zacc_comp;

	VECTOR_2D CurrentAccel;


    while (Run)
    {
	Log->WriteLogLine("B " + boost::lexical_cast<std::string>(CarControl->TimeStamp()));
        if (serial->waitForDataMessage(reply) != XRV_OK)
        {
		Log->WriteLogLine("XSens - Error reading messages!");
        }
       Log->WriteLogLine("A " + boost::lexical_cast<std::string>(CarControl->TimeStamp()));

	matrix_data = reply->getOriMatrix();

	//long double cos_roll = cosl((long double)reply->getOriEuler().m_roll*2*M_PI/360);
	//long double cos_pitch = cosl((long double)reply->getOriEuler().m_pitch*2*M_PI/360);
	//long double sin_roll = sinl((long double)reply->getOriEuler().m_roll*2*M_PI/360);
	//long double sin_pitch = sinl((long double)reply->getOriEuler().m_pitch*2*M_PI/360);

	xacc = (long double) reply->getCalData().m_acc.m_data[0];
	yacc = (long double) reply->getCalData().m_acc.m_data[1];
	zacc = (long double) reply->getCalData().m_acc.m_data[2];

	xacc_comp = (long double) matrix_data.m_data[0][0]*xacc + matrix_data.m_data[1][0]*yacc + matrix_data.m_data[2][0]*zacc;
	yacc_comp = (long double) matrix_data.m_data[0][1]*xacc + matrix_data.m_data[1][1]*yacc + matrix_data.m_data[2][1]*zacc;
	//zacc_comp = (long double) matrix_data.m_data[0][2]*xacc + matrix_data.m_data[1][2]*yacc + matrix_data.m_data[2][2]*zacc;

	// IMU axes are the wrong way about
	CurrentAccel.x = -yacc_comp;
	CurrentAccel.y = xacc_comp;


	Accelerations.erase(Accelerations.begin());

	Accelerations.push_back(CurrentAccel);

	//long double xacc_comp = (long double) cos_pitch*xacc + sin_pitch*zacc;
	//long double yacc_comp = (long double) sin_pitch*sin_roll*xacc + cos_roll*yacc - sin_roll*cos_pitch*zacc;
	//long double zacc_comp = (long double) -cos_roll*sin_pitch*xacc + sin_roll*yacc + cos_roll*cos_pitch*zacc;

	
	if(matrix_data.m_data[0][1] < 0 && matrix_data.m_data[1][1] < 0) { Yaw = 360*asinl(matrix_data.m_data[1][0])/2/M_PI; }
	else if(matrix_data.m_data[1][1] > 0) { Yaw = 180 - 360*asinl(matrix_data.m_data[1][0])/2/M_PI; }
	else { Yaw = 360 + 360*asinl(matrix_data.m_data[1][0])/2/M_PI; }

	if(CarControl->ExtLogging) {
		IMULog->WriteLogLine(boost::lexical_cast<std::string>(CarControl->TimeStamp()) + "," + boost::lexical_cast<std::string>(Yaw) + "," + boost::lexical_cast<std::string>(xacc_comp) + "," + boost::lexical_cast<std::string>(yacc_comp), true);
	}

//	if(reply->getOriEuler().m_yaw < 0) { Yaw = 360 + reply->getOriEuler().m_yaw ; }
//	else { Yaw = reply->getOriEuler().m_yaw; }

	// Massively decrease the amount of time that the CMT API spends waiting for a message.
	boost::this_thread::sleep(boost::posix_time::milliseconds(7));

    }

}

VECTOR_2D Xsens::GetAverageAccel(int n) {

	VECTOR_2D Result;
	Result.x = 0.0;
	Result.y = 0.0;

	for(unsigned int i = Accelerations.size()-n; i < Accelerations.size(); i++) {
		Result.x += Accelerations[i].x;
		Result.y += Accelerations[i].y;
	}

	Result.x = Result.x / (double)n;
	Result.y = Result.y / (double)n;

	return Result;

}
