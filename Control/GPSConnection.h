
#ifndef _GPSConnection_H
#define	_GPSConnection_H

#include <vector>
#include <string>
#include <boost/thread.hpp> 
#include <boost/math/constants/constants.hpp>

#include "libgpsmm.h"


class Control;
class Logger;


class GPSConnection {
public:

	GPSConnection(Control* CarController, Logger* Logger);
    	GPSConnection(const GPSConnection& orig);
    	virtual ~GPSConnection();

	bool Open();
	void GPSDFound();
	void Start();
	void Stop();
	
	double PI;
	bool GPSState;
	bool UsingGPSD;

	double Latitude;
	double Longitude;
	double Speed;
	double TrackAngle;
	double Time;
	int NumSat;

private:

	gpsmm* GPSReceiver;

	Logger* GPSLog;

	boost::thread m_Thread;
	boost::thread s_Thread;

	bool Run;
	int fd;

	double OldTime;
	double velocity_n;
	double velocity_e;

	void ProcessMessages();
	void ProcessMessagesDep();
	void NewSpeedAndPosition();
	void NewTrack();

	void Monitor();

	Control* CarControl;

	Logger* Log;

};

#endif	/* _GPSConnection_H */

