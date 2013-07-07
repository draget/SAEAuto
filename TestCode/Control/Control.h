#ifndef _CONTROL_H
#define	_CONTROL_H

class CarNetwork;
class Logger;

class Control {
public:

	Control();
    	Control(const Control& orig);
    	virtual ~Control();

	void Setup();
	void Run();
	static void Quit(int param);

	bool HeartbeatState;
	bool Trip;
	bool ManualOn;

	int CurrentSteeringPosn;
	int CurrentThrottleBrake;

    
private:
    
	CarNetwork* CarNetworkConnection;
	Logger* Log;

};



#endif	/* _CONTROL_H */
