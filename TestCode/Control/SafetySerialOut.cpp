/* 
 * File:   SafetySerialOut.cpp
 * Author: T. Drage
 *
 * Created on 7/7/13
 */


#include <iostream>
#include <boost/thread.hpp> 
#include <boost/lexical_cast.hpp>
#include <exception>
#include <vector>

#include <string>

#include "AsyncSerial.h"

#include "SafetySerialOut.h"
#include "Logger.h"
#include "Control.h"

using namespace std;

/**
 * Purpose: Creates a new instance of the SafetySerialOut object.
 * Inputs : None.
 * Outputs: None.
 */
SafetySerialOut::SafetySerialOut(Control* CarController, Logger* Logger) {

	CarControl = CarController;
 	Log = Logger;	

	SerialState = false;
	Run = false;
	WrongAckCount = 0;
	ExpectedAck = "";

	RxBuffer.reserve(10);

}

/**
 * Purpose: Creates a new instance of the SafetySerialOut object.
 * Inputs : An Network object.
 * Outputs: None.
 */
SafetySerialOut::SafetySerialOut(const SafetySerialOut& orig) {
}

/**
 * Purpose: Destroys the instance of the SafetySerialOut object.
 * Inputs : None.
 * Outputs: None.
 */
SafetySerialOut::~SafetySerialOut() {
}

bool SafetySerialOut::Open() {
    
	try { 
		Serial = new CallbackAsyncSerial(DEVICE,BAUDRATE); 
		Serial->setCallback(boost::bind(&SafetySerialOut::Receive,this, _1, _2));

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

void SafetySerialOut::Start() {

	if(SerialState) {	

		Run = true;

		m_Thread = boost::thread(&SafetySerialOut::SendHB, this);
		m_Thread.detach();

		m_Thread_AckCount = boost::thread(&SafetySerialOut::MonitorAckCount, this);
		m_Thread.detach();

	}

}


void SafetySerialOut::Stop() {

	Run = false;

	sleep(1);

	if(SerialState) { Serial->close(); }
	
}


void SafetySerialOut::SendHB() {

	while(Run) {

		if(ExpectedAck.empty()) {	

			if(CarControl->HeartbeatState) { char Plus = '+'; Serial->write(&Plus,1); ExpectedAck = "ACK +"; }
			else { char Minus = '-'; Serial->write(&Minus,1); ExpectedAck = "ACK -"; }

			if(CarControl->TripState > 0) { char E = 'E'; Serial->write(&E,1); ExpectedAck = "ACK E"; }

		}

		//else { WrongAckCount++; Log->WriteLogLine("SafetySerial - Ack for last message not received!"); }

	
		if(Serial->errorStatus() || Serial->isOpen() == false) {
                		Log->WriteLogLine("SafetySerial - Error: serial port unexpectedly closed");
				Run = false;
				SerialState = false;
				CarControl->Trip(2);
       		}

		boost::this_thread::sleep(boost::posix_time::milliseconds(10));

	}

}

void SafetySerialOut::Receive(const char *data, unsigned int len) {

	std::vector<char> RxdVector(data,data+len);

	while(RxdVector.size() > 0) {

		if(RxdVector.front() != '\n') { RxBuffer.push_back(RxdVector.front()); }
		else { ProcessMessage(); }

		RxdVector.erase(RxdVector.begin());

	}


}

bool SafetySerialOut::Send(char Command) {

	if(SerialState && ExpectedAck.empty()) { Serial->write(&Command,1); ExpectedAck = "ACK " + boost::lexical_cast<std::string>(Command); return true; }
	else if(ExpectedAck.empty() == false) { WrongAckCount++; Log->WriteLogLine("SafetySerial - Ack for last message not received!"); }
	
	return false;

}

void SafetySerialOut::ProcessMessage() {

	std::string MsgString(RxBuffer.begin(), RxBuffer.end());

	Log->WriteLogLine(MsgString + " " + ExpectedAck);

	if(MsgString.compare(0,3,"ACK") == 0) {

		if(MsgString.compare(ExpectedAck) == 0) { ExpectedAck.clear(); }
		else { Log->WriteLogLine("SafetySerial - Received wrong ACK!"); WrongAckCount++; }

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

void SafetySerialOut::MonitorAckCount() {

	if(WrongAckCount > 10 && CarControl->TripState == 0) { CarControl->Trip(4); }

	boost::this_thread::sleep(boost::posix_time::milliseconds(100));

}

