/*

Dump Xsens data to a file...

Modifed from example_3s, T. Drage 2013.

*/

#include "cmt3.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>


int quit = 0;

void exitFunc(void)
{
    (void) signal(SIGINT, SIG_DFL);
}

void ctrlchandler(int sig)
{
    // got ctrl-c
    quit = 1;
}

using namespace xsens;

#define EXIT_ERROR(loc) {printf("Error %d occurred during " loc ": %s\n", serial.getLastResult(), xsensResultText(serial.getLastResult())); exit(-1); }

int main(int argc, char* argv[])
{
    Cmt3 serial;
    Packet reply(1,0); /* 1 item, not xbus */
    (void) signal(SIGINT, ctrlchandler);
    atexit(exitFunc);
        
    char portname[32];
    if(argc > 1)
	strcpy(portname, argv[1]);

    if (strlen(portname) == 0)
        sprintf(portname, "/dev/ttyUSB0");

    if (serial.openPort(portname, B115200) != XRV_OK)
        EXIT_ERROR("open");
    printf("MT now in config mode\n");
    
	 //set the measurement timeout to 100ms (default is 16ms)
	int timeOut = 100;
	if (serial.setTimeoutMeasurement(timeOut) != XRV_OK)
		EXIT_ERROR("set timeout");
	printf("Measurement timeout set to %d ms\n", timeOut);
	
    CmtDeviceMode mode(CMT_OUTPUTMODE_ORIENT | CMT_OUTPUTMODE_CALIB, CMT_OUTPUTSETTINGS_TIMESTAMP_SAMPLECNT | CMT_OUTPUTSETTINGS_ORIENTMODE_EULER | CMT_OUTPUTSETTINGS_CALIBMODE_ACCGYR, 256);
    if (serial.setDeviceMode(mode, false, CMT_DID_BROADCAST))
        EXIT_ERROR("set device mode");
    printf("Device modes set\n");

    

    if (serial.gotoMeasurement())
        EXIT_ERROR("goto measurement");
    printf("Now in measurement mode\n");

    FILE *fp;
    if(argc > 2) { fp = fopen(argv[2],"w"); }
    else { fp = fopen("./data.imu","w"); }

    if (fp == NULL) {
        perror("Failed to open file!");
        return EXIT_FAILURE;
    }

    long msgCount = 0;
    while (!quit)
    {
        if (serial.waitForDataMessage(&reply) != XRV_OK)
        {
            printf("\n");
            EXIT_ERROR("read data message");
        }

        msgCount++;
        fprintf(fp,"%3ld,%hu,%llu,%u,%f,%f,%f,%f,%f,%f\n",
            msgCount,
            reply.getSampleCounter(),              // sample counter
            reply.m_rtc,                           // Time Of Arrival / Real-Time Clock
            getTimeOfDay(),                         // timestamp
            (double) reply.getOriEuler().m_roll,     // roll
            (double) reply.getOriEuler().m_pitch,    // pitch
            (double) reply.getOriEuler().m_yaw ,      // yaw
	    (double) reply.getCalData().m_acc.m_data[0]    ,
	    (double) reply.getCalData().m_acc.m_data[1]   ,
	    (double) reply.getCalData().m_acc.m_data[2]
            );

    }

    fclose(fp);
    printf("\n\nDone reading\n");
    return 0;
}
