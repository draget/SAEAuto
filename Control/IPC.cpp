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

	if(access("noibeo", F_OK ) != -1) { unlink("./IPC_FIFO_RX"); }

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

		if(MessageParts[0].compare(0,5,"ESTOP") == 0) {
			CarControl->Trip(9);
		}
		else if(MessageParts[0].compare(0,8,"SETDATUM") == 0) { 
			Log->WriteLogLine("IPC - Setting new datum " + MessageParts[1] + MessageParts[2]);
			if(! CarControl->AutoOn)	{
				CarControl->ClearMap();		
				CarControl->DatumLat = boost::lexical_cast<double>(MessageParts[1]); 
				CarControl->DatumLong = boost::lexical_cast<double>(MessageParts[2]); 
			}
			else { Log->WriteLogLine("IPC - Can't change datum during auto!!!!"); }
		}
		else if(MessageParts[0].compare(0,7,"LOADMAP") == 0) {
			Log->WriteLogLine("IPC - calling load map");
			if(! CarControl->AutoOn) { 
				CarControl->LoadMap(MessageParts[1]);
				CarControl->DumpBaseFrame();
			}
			else { Log->WriteLogLine("IPC - Can't change map during auto!!!!"); }
		}
		else if(MessageParts[0].compare(0,7,"DUMPMAP") == 0) {
			Log->WriteLogLine("IPC - calling dump map");
			CarControl->DumpMap();
		}
		else if(MessageParts[0].compare(0,9,"AUTOSTART") == 0) {
			Log->WriteLogLine("IPC - calling auto start");
			CarControl->AutoStart();
		}
		else if(MessageParts[0].compare(0,8,"AUTOSTOP") == 0) {
			Log->WriteLogLine("IPC - calling auto stop");
			CarControl->AutoStop();
		}
		else if(MessageParts[0].compare(0,9,"AUTOPAUSE") == 0) {
			Log->WriteLogLine("IPC - calling auto pause");
			CarControl->AutoPause();
		}
		else if(MessageParts[0].compare(0,8,"AUTOCONT") == 0) {
			Log->WriteLogLine("IPC - calling auto cont");
			CarControl->AutoContinue();
		}
		else if(MessageParts[0].compare(0,6,"TOGBIL") == 0) {
			Log->WriteLogLine("IPC - calling toggle brakeil");
			CarControl->ToggleBrakeIL();
		}
		else if(MessageParts[0].compare(0,8,"STARTREC") == 0) {
			Log->WriteLogLine("IPC - start record");
			CarControl->StartMapRecord();
		}
		else if(MessageParts[0].compare(0,7,"STOPREC") == 0) {
			Log->WriteLogLine("IPC - calling stop record");
			CarControl->StopMapRecord(MessageParts[1]);
		}
		else if(MessageParts[0].compare(0,9,"SETGPSOFF") == 0) {
			Log->WriteLogLine("IPC - setting GPS offset" + MessageParts[1] + " " + MessageParts[2]);
			if(! CarControl->AutoOn) {
				CarControl->LatOffset = boost::lexical_cast<double>(MessageParts[1]);
				CarControl->LongOffset = boost::lexical_cast<double>(MessageParts[2]);
			}
			else { Log->WriteLogLine("IPC - Can't change offset during auto!!!!"); }
		}	
		else if(MessageParts[0].compare(0,6,"UNTRIP") == 0) {
			Log->WriteLogLine("IPC - calling untrip");
			CarControl->Untrip();
		}


	}

}




