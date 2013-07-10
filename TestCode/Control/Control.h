#ifndef _CONTROL_H
#define	_CONTROL_H

class CarNetwork;
class Logger;
class SafetySerialOut;

class Control {
public:

	Control();
    	Control(const Control& orig);
    	virtual ~Control();

	void Setup();
	void Run();
	void Quit();

	bool HeartbeatState;
	int Trip;
	bool ManualOn;

	int CurrentSteeringPosn;
	int CurrentThrottleBrake;

    
private:

	bool RunState;
    
	CarNetwork* CarNetworkConnection;
	Logger* Log;
	SafetySerialOut* SafetySerial;

};



#endif	/* _CONTROL_H */
