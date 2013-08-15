/* 
 * File:   CarNetwork.h
 * Author: T. Drage
 *
 * Converted from C....mixes C and C++, needs cleaning up.
 *
 * Created on 23/6/13
 */


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <boost/thread.hpp> 
#include <boost/lexical_cast.hpp>
#include <string>

#include <sys/time.h>

#include "CarNetwork.h"
#include "Logger.h"
#include "Control.h"



/**
 * Purpose: Creates a new instance of the Network object.
 * Inputs : None.
 * Outputs: None.
 */
CarNetwork::CarNetwork(Control* CarController, Logger* Logger) {

	SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
 
	CarControl = CarController;

	Log = Logger;

	Run = false;
	HasConnection = false;

    	if(-1 == SocketFD) {
      		perror("can not create socket");
      		exit(EXIT_FAILURE);
   	}
	
	StatusString = "Socket created";

}

/**
 * Purpose: Creates a new instance of the Network object.
 * Inputs : An Network object.
 * Outputs: None.
 */
CarNetwork::CarNetwork(const CarNetwork& orig) {
}

/**
 * Purpose: Destroys the instance of the Network object.
 * Inputs : None.
 * Outputs: None.
 */
CarNetwork::~CarNetwork() {
    if (IsConnected()) Disconnect();
}

bool CarNetwork::Open() {
    
	struct sockaddr_in stSockAddr;
 
    	memset(&stSockAddr, 0, sizeof(stSockAddr));
 
    	stSockAddr.sin_family = AF_INET;
    	stSockAddr.sin_port = htons(LISTEN_PORT);
    	stSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
 
    	if(-1 == bind(SocketFD,(struct sockaddr *)&stSockAddr, sizeof(stSockAddr))) {
      		perror("error bind failed");
      		close(SocketFD);
      		exit(EXIT_FAILURE);
    	}
 
    	if(-1 == listen(SocketFD, 10)) {
      		perror("error listen failed");
      		close(SocketFD);
      		exit(EXIT_FAILURE);
    	}

	StatusString = "Bound to socket";

return true;


}

void CarNetwork::StartProcessMessages() {

Run = true;

m_Thread = boost::thread(&CarNetwork::ProcessMessages, this);
m_Thread.detach();

}

void CarNetwork::ProcessMessages() {

	char msgbuf[1024];
	char temp[2];
	char recvbuf;
	int ConnectFD;

	fd_set read_set;
	fd_set write_set;

	

while(Run) {  // Wait for connections

		// printf("Waiting to accept a connection \n");
		Log->WriteLogLine("Car Network - Waiting to accept a connection");		

		StatusString = "Waiting to accept a connection";

    		ConnectFD = accept(SocketFD, NULL, NULL);
 
      		if(0 > ConnectFD) {
        		perror("error accept failed");
        		close(SocketFD);
      		}

		// printf("Conn. accepted - waiting for first message \n");

		StatusString = "Conn. accepted - waiting for first message";
		Log->WriteLogLine("Car Network - waiting for messages");
 
		HasConnection = true;

    		while(1) { // Wait for messages
		
			bool breakandclose = false;

			int msg_length = 0;

			bzero(msgbuf,sizeof(msgbuf));

			struct timeval timeoutlong;
			timeoutlong.tv_sec = 1;
			timeoutlong.tv_usec = 0;
	
			FD_ZERO(&read_set);
			FD_SET(ConnectFD,&read_set);  

			//printf("Checking if we can read \n");

			int readable = select(ConnectFD + 1, &read_set, NULL, NULL, &timeoutlong);

			if(readable == 0) { 
				//printf("No message?!\n");
				StatusString = "No message rxd";
				Log->WriteLogLine("Car Network - No message rxd"); 
				CarControl->Trip(6);
				break; 
			}
			
			//printf("Looks like there is something to read! %i \n", readable);

			StatusString = "Reading bytes";

			while(1) { // Recieve bytes
				recvbuf = '\0';
				struct timeval timeout;
				timeout.tv_sec = 1;
				timeout.tv_usec = 0;
				FD_ZERO(&read_set);
				FD_SET(ConnectFD,&read_set);  
				readable = select(ConnectFD + 1, &read_set, NULL, NULL, &timeout);
				if(readable == 0) { 
					//printf("Drop halfway through message?!\n");
					StatusString = "Drop halfway through message?"; 
					Log->WriteLogLine("Car Network - Drop halfway through message?");		
					breakandclose = true; 
					break; 
				}
				recv(ConnectFD,&recvbuf,sizeof(recvbuf),0);
				if(recvbuf == 0) { 
					//printf("Drop halfway through message (null read)?!\n");
					StatusString = "Drop halfway through message? (null read)"; 
					Log->WriteLogLine("Car Network - Drop halfway through message? (null read)");
					breakandclose = true; 
					break; 
				}
				if(recvbuf == '\n' || recvbuf == '\r') break;
				temp[0] = recvbuf;
				temp[1] = '\0';
				strcat(msgbuf,temp);
				msg_length++;
			} 

			if(breakandclose) { breakandclose = false; CarControl->Trip(6); break; }

			//printf("Data: *%s* \n",msgbuf);

			std::string Message = msgbuf;

			StatusString = "Data read: " + Message;

			if(Message.compare(0,3,"HBT") == 0) {

			gettimeofday(&current,NULL);
			int ms_gap = (current.tv_usec - last.tv_usec)/1000 ;
			if(ms_gap > 100) { Log->WriteLogLine("CarNetwork - Slow response on HB! " + boost::lexical_cast<std::string>(ms_gap) + " " +  Message.substr(4,1)); }
			gettimeofday(&last,NULL);

				if(Message.compare(4,1,"+") == 0) { 
					//printf("Set state true \n"); 
					CarControl->HeartbeatState = true; 
				}
				else if(Message.compare(4,1,"-") == 0) { 
					//printf("Set state false \n"); 
					CarControl->HeartbeatState = false; 
				}

			}
			else if(Message.compare(0,5,"ESTOP") == 0) {
				CarControl->Trip(1);
				if(CarControl->TripState == 0) { Log->WriteLogLine("Car Network - Rxd ESTOP!!"); }
			}
			else if(Message.compare(0,3,"MAN") == 0) {
				CarControl->ManualOn = true;
				Log->WriteLogLine("Car Network - Received force manual.");
			}
			else if(Message.compare(0,3,"BIL") == 0) {
				CarControl->ToggleBrakeIL();
			}
			else if(Message.compare(0,3,"ACL") == 0 && CarControl->ManualOn && CarControl->TripState == 0) {
				CarControl->CurrentThrottleBrakeSetPosn = boost::lexical_cast<int>(Message.substr(4,std::string::npos));
			}
			else if(Message.compare(0,3,"STR") == 0 && CarControl->ManualOn) {
				CarControl->CurrentSteeringSetPosn = boost::lexical_cast<int>(Message.substr(4,std::string::npos));
			}
			else if(Message.compare(0,5,"ALM") == 0) {
				CarControl->SendAlarm();
			}
			
				

			if(msgbuf[0] > 0) {
				char ackmsg[6+msg_length];
				snprintf(ackmsg, sizeof(ackmsg), "ACK|%s\n", msgbuf);
				char ackmsg_no0[5+msg_length];
				for(int i=0;i<5+msg_length;i++) { ackmsg_no0[i] = ackmsg[i]; }

				struct timeval timeout;
				timeout.tv_sec = 1;
				timeout.tv_usec = 0;
				FD_ZERO(&write_set);
				FD_SET(ConnectFD,&write_set);  
				if(select(ConnectFD + 1, NULL, &write_set, NULL, &timeout) == 0) { 
					//printf("Did it go away before write?\n");  
					StatusString = "Problem writing";
					Log->WriteLogLine("Car Network - Problem writing");
					CarControl->Trip(6); break; 
				}

				write(ConnectFD,ackmsg_no0,sizeof(ackmsg_no0));
			}
 
      		/*if (-1 == shutdown(ConnectFD, SHUT_RDWR)) {
        		perror("can not shutdown socket");
        		close(ConnectFD);
        		close(SocketFD);
       		 	exit(EXIT_FAILURE);
      		}*/
      		
    		}
	// printf("Going to close connection...\n");
	close(ConnectFD);
	HasConnection = false;

	}

}


bool CarNetwork::IsConnected() {

	int act;
	char buff;

	act = recv(SocketFD, &buff, 1, MSG_PEEK);

	if(act) { return true; }
	else { return false; }

}

void CarNetwork::Disconnect() {

	Run = false;

	sleep(1);

    	(void) shutdown(SocketFD, SHUT_RDWR);
 
    	close(SocketFD);

}


