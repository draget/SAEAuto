#ifndef _CONTROL_H
#define	_CONTROL_H

#include "CarNetwork.h"

class Control {
public:

	Control();
    	Control(const Control& orig);
    	virtual ~Control();

	void Setup();
	void Run();
	static void Quit(int param);

	bool HeartbeatState;
	bool ManualOn;

    
private:
    
	CarNetwork* CarNetworkConnection;

};



#endif	/* _CONTROL_H */
