
#ifndef _SafetySerialOut_H
#define	_SafetySerialOut_H

#define BAUDRATE 9600
#define DEVICE "/dev/ttyUSB0"

#include <boost/thread.hpp> 

#include "AsyncSerial.h"

class Control;
class Logger;

class SafetySerialOut {
public:

	SafetySerialOut(Control* CarController, Logger* Logger);
    	SafetySerialOut(const SafetySerialOut& orig);
    	virtual ~SafetySerialOut();

	bool Open();
	void Start();
	
	std::string StatusString;

private:

	CallbackAsyncSerial* Serial;

	boost::thread m_Thread;

	void SendMessages();

	void CheckAcks(const char *data, unsigned int len);

	Control* CarControl;

	Logger* Log;

};

#endif	/* _SafetySerialOut_H */

