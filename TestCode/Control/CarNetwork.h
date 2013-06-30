
#ifndef _CarNetwork_H
#define	_CarNetwork_H

#define LISTEN_PORT 1100

#include <string>

#include <boost/thread.hpp> 

class CarNetwork {
public:

	CarNetwork(bool* HeartbeatStatePointer);
    	CarNetwork(const CarNetwork& orig);
    	virtual ~CarNetwork();

	bool IsConnected();
	void Disconnect();
	bool Open();
	void StartProcessMessages();
	void JoinProcessMessages();

	bool NetworkActive;
	std::string StatusString;


private:

	int SocketFD;

	bool Run;

	boost::thread m_Thread;

	void ProcessMessages();

	bool* HeartbeatState;

};

#endif	/* _CarNetwork_H */

