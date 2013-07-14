/* 
 * File:   LowLevelSerialOut.cpp
 * Author: T. Drage
 *
 * Created on 13/7/13
 */


#include <iostream>
#include <boost/thread.hpp> 
#include <exception>
#include <vector>

#include <string>

#include "AsyncSerial.h"

#include "LowLevelSerialOut.h"
#include "Logger.h"
#include "Control.h"

using namespace std;

/**
 * Purpose: Creates a new instance of the LowLevelSerialOut object.
 * Inputs : None.
 * Outputs: None.
 */
LowLevelSerialOut::LowLevelSerialOut(Control* CarController, Logger* Logger) {

	CarControl = CarController;
 	Log = Logger;	

	SerialState = false;
	Run = false;

	RxBuffer.reserve(10);

}

/**
 * Purpose: Creates a new instance of the LowLevelSerialOut object.
 * Inputs : An Network object.
 * Outputs: None.
 */
LowLevelSerialOut::LowLevelSerialOut(const LowLevelSerialOut& orig) {
}

/**
 * Purpose: Destroys the instance of the SafetySerialOut object.
 * Inputs : None.
 * Outputs: None.
 */
LowLevelSerialOut::~LowLevelSerialOut() {
}

bool LowLevelSerialOut::Open() {
    
	try { 
		Serial = new CallbackAsyncSerial(DEVICE,BAUDRATE); 
		Serial->setCallback(boost::bind(&LowLevelSerialOut::Receive,this, _1, _2));

		if(Serial->errorStatus() || Serial->isOpen() == false) {
                	cerr<<"Error: serial port unexpectedly closed"<<endl;
			return false;
        	}
		else {	
			SerialState = true;
			return true;
		}

	}
	catch(std::exception& e) { 
		std::string ExpString(e.what()); 
		Log->WriteLogLine("SafetySerial - Caught exception when opening serial port: " + ExpString); 
		return false;
	}
        
}

void LowLevelSerialOut::Start() {

	if(SerialState) {	

		Run = true;

		m_Thread = boost::thread(&LowLevelSerialOut::SendCurrent, this);
		m_Thread.detach();

	}

}


void LowLevelSerialOut::Stop() {

	Run = false;

	sleep(1);

	if(SerialState) { Serial->close(); }
	
}


void LowLevelSerialOut::SendCurrent() {

	while(Run) {

		if(CarControl->HeartbeatState) { char Plus = '+'; Serial->write(&Plus,1); }
		else { char Minus = '-'; Serial->write(&Minus,1); }

		if(CarControl->TripState > 0) { char E = 'E'; Serial->write(&E,1); }

	
		if(Serial->errorStatus() || Serial->isOpen() == false) {
                		Log->WriteLogLine("SafetySerial - Error: serial port unexpectedly closed");
				Run = false;
				SerialState = false;
				CarControl->Trip(2);
       		}

		boost::this_thread::sleep(boost::posix_time::milliseconds(10));

	}

}

void LowLevelSerialOut::Receive(const char *data, unsigned int len) {

	std::vector<char> RxdVector(data,data+len);

	while(RxdVector.size() > 0) {

		if(RxdVector.front() != '\n') { RxBuffer.push_back(RxdVector.front()); }
		else { ProcessMessage(); }

		RxdVector.erase(RxdVector.begin());

	}


}

bool LowLevelSerialOut::Send(std::string Command) {

	if(SerialState) { Serial->writeString(Command); return true; }
	else return false;

}

void LowLevelSerialOut::ProcessMessage() {

	std::string MsgString(RxBuffer.begin(), RxBuffer.end());

	if(MsgString.compare(0,3,"ACK") == 0) {


	}

	else if(MsgString.compare(0,5,"ESTOP") == 0) {
		CarControl->Trip(3);
		Log->WriteLogLine("SafetySerial - TRIP!");
	}

	else {

		Log->WriteLogLine("SafetySerial - Received non-ack: " + MsgString);

	}

	RxBuffer.clear();

}

