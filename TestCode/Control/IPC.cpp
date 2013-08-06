/* 
 * File:   IPC.cpp
 * Author: T. Drage
 *
 * IPC class...
 *
 * Created on 30/7/13
 */


#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include <boost/thread.hpp> 
#include <boost/lexical_cast.hpp>

#include "IPC.h"
#include "Logger.h"
#include "Control.h"
#include "GPSConnection.h"



/**
 * Purpose: Creates a new instance of the IPC object.
 * Inputs : None.
 * Outputs: None.
 */
IPC::IPC(Control* CarController, Logger* Logger) {
 
	OpenState = false;

	CarControl = CarController;
 	Log = Logger;	

	int res = mkfifo("./IPC_FIFO_RX", 0777);
	if(res != 0) { Log->WriteLogLine("IPC - Couldn't create fifo!"); }

}

void IPC::Open() {

	OpenState = true;

	RXpipe = fopen("./IPC_FIFO_RX", "r+");
	if(RXpipe == NULL) { Log->WriteLogLine("IPC - RX Pipe open failed..."); OpenState = false; }
	else { Log->WriteLogLine("IPC - Pipe open okay."); }

}

void IPC::Start() {

	if(OpenState) {	

		Run = true;

		m_Thread = boost::thread(&IPC::ProcessMessages, this);
		m_Thread.detach();

	}

}


/**
 * Purpose: Creates a new instance of the Network object.
 * Inputs : An Network object.
 * Outputs: None.
 */
IPC::IPC(const IPC& orig) {
}

/**
 * Purpose: Destroys the instance of the Network object.
 * Inputs : None.
 * Outputs: None.
 */
IPC::~IPC() {
}


void IPC::ProcessMessages() {

	char readbuf[30];

	while(Run) {

		bzero(readbuf,sizeof(readbuf));

		fgets(readbuf,30,RXpipe);
	
		std::string Message = readbuf;

		Log->WriteLogLine("IPC - Received " + Message);

		std::string WriteData;

		if(Message.compare(0,6,"COOL") == 0) { Log->WriteLogLine("yea"); }

	}

}




