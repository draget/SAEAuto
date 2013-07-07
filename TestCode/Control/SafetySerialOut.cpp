/* 
 * File:   SafetySerialOut.cpp
 * Author: T. Drage
 *
 * Created on 7/7/13
 */


#include <iostream>
#include <boost/thread.hpp> 

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
    
	Serial = new CallbackAsyncSerial(DEVICE,BAUDRATE);
        Serial->setCallback(boost::bind(&SafetySerialOut::CheckAcks,this, _1, _2));	

	if(Serial->errorStatus() || Serial->isOpen()==false) {
                cerr<<"Error: serial port unexpectedly closed"<<endl;
		return false;
        }
	else {	
		return true;
	}

}

void SafetySerialOut::Start() {




}

void SafetySerialOut::CheckAcks(const char *data, unsigned int len) {


}



