/* 
 * File:   LowLevelSerialOut.cpp
 * Author: T. Drage
 *
 * Created on 13/7/13
 */


#include <iostream>
#include <boost/thread.hpp> 
#include <boost/lexical_cast.hpp>
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
 * Purpose: Destroys the instance of the LowLevelSerialOut object.
 * Inputs : None.
 * Outputs: None.
 */
LowLevelSerialOut::~LowLevelSerialOut() {
}

bool LowLevelSerialOut::Open() {
    
	try { 
		Serial = new CallbackAsyncSerial(LL_DEVICE,LL_BAUDRATE); 
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
		Log->WriteLogLine("LowLevelSerial - Caught exception when opening serial port: " + ExpString); 
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

		if(CarControl->CurrentSteeringSetPosn <= 127 && CarControl->CurrentSteeringSetPosn >= -128) {
			std::string SteeringCommand = "S" + boost::lexical_cast<std::string>(CarControl->CurrentSteeringSetPosn);
			Serial->writeString(SteeringCommand + '\n');
		}

		else { 
			Log->WriteLogLine("LowLevelSerial - Steering set point out of range");
			CarControl->Trip(5);
		}

		if(CarControl->CurrentThrottleBrakeSetPosn <= 255 && CarControl->CurrentThrottleBrakeSetPosn >= -256) {
			
			std::string BrakeCommand = "B0";
			std::string ThrottleCommand = "A0";			

			if(CarControl->CurrentThrottleBrakeSetPosn < 0) {
				BrakeCommand = "B" + boost::lexical_cast<std::string>(-1*CarControl->CurrentThrottleBrakeSetPosn);
			}
			else {
				ThrottleCommand = "A" + boost::lexical_cast<std::string>(CarControl->CurrentThrottleBrakeSetPosn);
			}

			Serial->writeString(BrakeCommand + '\n');
			Serial->writeString(ThrottleCommand + '\n');

		}

		else { 
			Log->WriteLogLine("LowLevelSerial - Brake/throttle set point out of range");
			CarControl->Trip(5);
		}

		if(CarControl->TripState > 0) { char E = 'E'; Serial->write(&E,1); }

	
		if(Serial->errorStatus() || Serial->isOpen() == false) {
                		Log->WriteLogLine("LowLevelSerial - Error: serial port unexpectedly closed");
				Run = false;
				SerialState = false;
				CarControl->Trip(2);
       		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(50));

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
	else {

		Log->WriteLogLine("LowLevelSerial - Received non-ack: " + MsgString);

	}

	RxBuffer.clear();

}
