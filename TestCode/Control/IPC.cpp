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
#include <vector>

#include <boost/thread.hpp> 
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

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

	mode_t process_mask = umask(0);
	int res = mkfifo("./IPC_FIFO_RX", S_IRWXU | S_IRWXG | S_IRWXO);
	umask(process_mask);

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

	char readbuf[150];

	while(Run) {

		bzero(readbuf,sizeof(readbuf));

		fgets(readbuf,150,RXpipe); // Block here waiting for a message.
	
		std::string Message = readbuf;
		
		boost::algorithm::trim(Message);

		Log->WriteLogLine("IPC - Received " + Message);

		std::vector<std::string> MessageParts;

		boost::algorithm::split(MessageParts, Message,  boost::algorithm::is_any_of(","));


		if(MessageParts[0].compare(0,8,"SETDATUM") == 0) { 
			Log->WriteLogLine("IPC - Setting new datum " + MessageParts[1] + " " + MessageParts[2]);
			CarControl->DatumLat = boost::lexical_cast<double>(MessageParts[1]); 
			CarControl->DatumLong = boost::lexical_cast<double>(MessageParts[2]); 
		}
		else if(MessageParts[0].compare(0,7,"LOADMAP") == 0) {
			Log->WriteLogLine("IPC - calling load map");
			CarControl->LoadMap(MessageParts[1]);
		}
		else if(MessageParts[0].compare(0,7,"DUMPMAP") == 0) {
			Log->WriteLogLine("IPC - calling dump map");
			CarControl->DumpMap();
		}

	}

}




