
#ifndef _Xsens_H
#define	_Xsens_H

#include <vector>
#include <string>
#include <boost/thread.hpp> 

#include "xsens/cmt3.h"

#include "Control.h"

#define IMU_PORT "/dev/ttyUSB0"


class Logger;

using namespace xsens;

class Xsens {
public:

	Xsens(Control* CarController, Logger* Logger);
    	Xsens(const Xsens& orig);
    	virtual ~Xsens();

	bool Open();
	void Start();
	VECTOR_2D GetAverageAccel(int n);

	double Yaw;
	
	std::vector<VECTOR_2D> Accelerations;

	bool IMUState;

private:

	Cmt3* serial;
  	Packet* reply;

	Logger* IMULog;

	bool Run;

	boost::thread m_Thread;

	void ProcessMessages();

	Control* CarControl;

	Logger* Log;

};

#endif	/* _Xsens_H */

