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

		if(CarControl->TripState > 0 && CarControl->TripState != 3) { char E = 'E'; Serial->write(&E,1); }
		else {
			if(CarControl->HeartbeatState) { char Plus = '+'; Serial->write(&Plus,1); }
			else { char Minus = '-'; Serial->write(&Minus,1); }
		}

		int i;
		for(i = 0; i<15; i++) {
			if(ExpectedAck.empty()) {	

				if(CarControl->TripState > 0  && CarControl->TripState != 3) { ExpectedAck = "ACK E"; }
				else {
					if(CarControl->HeartbeatState) { ExpectedAck = "ACK +"; }
					else { ExpectedAck = "ACK -"; }
				}
				break;

			}
			else { WrongAckCount++; }
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
		}



		if(Serial->errorStatus() || Serial->isOpen() == false) {
                		Log->WriteLogLine("SafetySerial - Error: serial port unexpectedly closed");
				Run = false;
				SerialState = false;
				CarControl->Trip(2);
       		}

		boost::this_thread::sleep(boost::posix_time::milliseconds(40));

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

	if(! SerialState) { Log->WriteLogLine("SafetySerial - Port not active."); return false; }

	for(int i = 0; i<100; i++) {
		if(ExpectedAck.empty()) { 
			Serial->write(&Command,1); 
			ExpectedAck = "ACK " + boost::lexical_cast<std::string>(Command); 
			return true; 
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	}

	WrongAckCount++; 
	Log->WriteLogLine("SafetySerial - Ack for last message not received, couldn't send in 100ms.");
	
	return false;

}

void SafetySerialOut::ProcessMessage() {

	std::string MsgString(RxBuffer.begin(), RxBuffer.end());

//Log->WriteLogLine(MsgString + " " + ExpectedAck);

	if(MsgString.compare(0,3,"ACK") == 0) {

		if(MsgString.compare(ExpectedAck) == 0) { WrongAckCount = 0; }
		else { Log->WriteLogLine("SafetySerial - Received wrong ACK! " + MsgString + " " + ExpectedAck); WrongAckCount++; }
		ExpectedAck.clear(); 
	}

	else if(MsgString.compare(0,4,"TRIP") == 0) {
		CarControl->Trip(3);
		Log->WriteLogLine("SafetySerial - TRIP!");
	}

	else {

		Log->WriteLogLine("SafetySerial - Received non-ack: " + MsgString);

	}

	RxBuffer.clear();

}

void SafetySerialOut::MonitorAckCount() {

	while(Run) { 
		if(WrongAckCount > 250 && CarControl->TripState == 0) { Log->WriteLogLine("SafetySerial - Exceeded 250 wrong/no acks!"); CarControl->Trip(4); }
	
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}
}

