
#ifndef _IPC_H
#define	_IPC_H

#include <string>
#include <iostream>
#include <fstream>

class Control;
class Logger;

class IPC {
public:

	IPC(Control* CarController, Logger* Logger);
    	IPC(const IPC& orig);
    	virtual ~IPC();

	void Open();
	void Start();
	void ProcessMessages();

private:

	bool OpenState;

	bool Run;

	boost::thread m_Thread;
	FILE *RXpipe;

	Control* CarControl;

	Logger* Log;

};

#endif	/* _IPC_H */

