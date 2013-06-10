/* 
 * File:   NetworkTest.cpp
 * Author: Thomas Drage (20510505)
 * 
 * Created on 9 June 2013
 */



#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string.h>




int main(int argc, char* argv[]) {

	char msgbuf[1024];
	char temp[2];
	char recvbuf;
	int ConnectFD;

	fd_set read_set;
	fd_set write_set;


	struct sockaddr_in stSockAddr;
    	int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
 
    	if(-1 == SocketFD) {
      		perror("can not create socket");
      		exit(EXIT_FAILURE);
   	}
 
    	memset(&stSockAddr, 0, sizeof(stSockAddr));
 
    	stSockAddr.sin_family = AF_INET;
    	stSockAddr.sin_port = htons(1100);
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

	while(1) {  // Wait for connections

    		ConnectFD = accept(SocketFD, NULL, NULL);
 
      		if(0 > ConnectFD) {
        		perror("error accept failed");
        		close(SocketFD);
        		exit(EXIT_FAILURE);
      		}

		printf("Waiting to for first message \n");

 
    		while(1) { // Wait for messages
		
			bool breakandclose = false;

			int msg_length = 0;

			bzero(msgbuf,sizeof(msgbuf));

			struct timeval timeoutlong;
			timeoutlong.tv_sec = 10;
			timeoutlong.tv_usec = 0;
	
			FD_ZERO(&read_set);
			FD_SET(ConnectFD,&read_set);  

			printf("Checking if we can read \n");

			int readable = select(ConnectFD + 1, &read_set, NULL, NULL, &timeoutlong);

			if(readable == 0) { printf("No message?!\n"); break; }
			printf("Looks like there is something to read! %i \n", readable);

			while(1) { // Recieve bytes
				recvbuf = '\0';
				struct timeval timeout;
				timeout.tv_sec = 1;
				timeout.tv_usec = 0;
				FD_ZERO(&read_set);
				FD_SET(ConnectFD,&read_set);  
				readable = select(ConnectFD + 1, &read_set, NULL, NULL, &timeout);
				if(readable == 0) { printf("Drop halfway through message?!\n"); breakandclose = true; break; }
				recv(ConnectFD,&recvbuf,sizeof(recvbuf),0);
				if(recvbuf == 0) { printf("Drop halfway through message (null read)?!\n"); breakandclose = true; break; }
				if(recvbuf == '\n' || recvbuf == '\r') break;
				temp[0] = recvbuf;
				temp[1] = '\0';
				strcat(msgbuf,temp);
				msg_length++;
			} 

			if(breakandclose) { breakandclose = false; break; }

			printf("Data: *%s* \n",msgbuf);

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
				if(select(ConnectFD + 1, NULL, &write_set, NULL, &timeout) == 0) { printf("Did it go away before write?\n");  break; }

				write(ConnectFD,ackmsg_no0,sizeof(ackmsg_no0));
			}
 
      		/*if (-1 == shutdown(ConnectFD, SHUT_RDWR)) {
        		perror("can not shutdown socket");
        		close(ConnectFD);
        		close(SocketFD);
       		 	exit(EXIT_FAILURE);
      		}*/
      		
    		}
	printf("Going to close connection...\n");
	close(ConnectFD);

	}

close(SocketFD);


return 0;

}

