
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
	bool Send(char Command);
	
	bool SerialState;

private:

	std::vector<char> RxBuffer;

	CallbackAsyncSerial* Serial;

	boost::thread m_Thread;
	boost::thread m_Thread_AckCount;

	std::string ExpectedAck;

	int WrongAckCount;

	bool Run;

	void Receive(const char *data, unsigned int len);

	void ProcessMessage();

	void SendHB();

	void MonitorAckCount();

	Control* CarControl;

	Logger* Log;

};

#endif	/* _SafetySerialOut_H */

