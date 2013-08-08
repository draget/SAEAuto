
#ifndef _CarNetwork_H
#define	_CarNetwork_H

#define LISTEN_PORT 1100

#include <string>

#include <boost/thread.hpp> 

class Control;
class Logger;

class CarNetwork {
public:

	CarNetwork(Control* CarController, Logger* Logger);
    	CarNetwork(const CarNetwork& orig);
    	virtual ~CarNetwork();

	bool IsConnected();
	void Disconnect();
	bool Open();
	void StartProcessMessages();

	bool HasConnection;

	std::string StatusString;


private:

	int SocketFD;

	bool Run;

	boost::thread m_Thread;

	void ProcessMessages();

	Control* CarControl;

	Logger* Log;

	timeval last,current;

};

#endif	/* _CarNetwork_H */

