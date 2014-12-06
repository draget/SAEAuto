//
//  GPSConnection.h
//  
//
//  Created by Ruvan Muthu-Krishna on 15/09/2014.
//
//

#ifndef _GPSConnection_H
#define	_GPSConnection_H

#include <vector>
#include <boost/thread.hpp> 
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sstream>
#include <math.h>

class Control;
class Logger;

#define PI 3.14159265

typedef struct {
	int tow;
	double n;
	double e;
    double d;
    double h_accuracy;
    double v_accuracy;
    int n_sats;
    int flags;
} NED;

typedef struct {
    int tow;
    double lat;
    double lon;
    double height;
    double h_accuracy;
    double v_accuracy;
    int n_sats;
    int flags;
} LlhPos;

typedef struct {
	int wn;
	int tow;
	double ns;
	int flags;
} GPS_TIME;

class GPSConnection {
public:

	GPSConnection(Control* CarController, Logger* Logger);
    	GPSConnection(const GPSConnection& orig);
    	virtual ~GPSConnection();

	bool Open();
	void Start();
	void Stop();
	
	bool GPSState;

	double Latitude;
	double Longitude;
	double Speed;
	double TrackAngle;
	double Time;
	NED baselinePosNED;
	NED baselineVelNED;
	LlhPos llhPos;
	
private:

	Logger* GPSLog;

	boost::thread m_Thread;
	boost::thread s_Thread;

	bool Run;
	GPS_TIME gpsTime;
	int oldTime;
	int baselinePosNEDOldTime;

	void ProcessMessages();
	void NewTrack();

	void Monitor();

	Control* CarControl;

	Logger* Log;

};

#endif	/* _GPSConnection_H */
