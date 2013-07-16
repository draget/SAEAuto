#ifndef _CONTROL_H
#define	_CONTROL_H

class CarNetwork;
class Logger;
class SafetySerialOut;
class LowLevelSerialOut;

class Control {
public:

	Control();
    	Control(const Control& orig);
    	virtual ~Control();

	void Setup();
	void Run();
	void Quit();
	void Trip(int TripState);
	void ToggleBrakeIL();

	bool HeartbeatState;
	int TripState;
	bool ManualOn;
	bool BrakeILOn;

	int CurrentSteeringSetPosn;
	int CurrentThrottleBrakeSetPosn;

    
private:

	bool RunState;
    
	CarNetwork* CarNetworkConnection;
	Logger* Log;
	SafetySerialOut* SafetySerial;
	LowLevelSerialOut* LowLevelSerial;

};



#endif	/* _CONTROL_H */
