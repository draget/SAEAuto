
#ifndef _SafetySerialOut_H
#define	_SafetySerialOut_H

#define BAUDRATE 9600
#define DEVICE "/dev/ttyUSB0"

#include <vector>
#include <string>
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
	void Stop();
	
	std::string StatusString;

private:

	std::vector<char> RxBuffer;

	bool SerialState;

	CallbackAsyncSerial* Serial;

	boost::thread m_Thread;

	bool Run;

	void SendMessages();

	void Receive(const char *data, unsigned int len);

	void ProcessMessage();

	void SendHB();

	Control* CarControl;

	Logger* Log;

};

#endif	/* _SafetySerialOut_H */

