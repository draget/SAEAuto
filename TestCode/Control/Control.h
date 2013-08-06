#ifndef _CONTROL_H
#define	_CONTROL_H

class CarNetwork;
class Logger;
class SafetySerialOut;
class LowLevelSerialOut;
class GPSConnection;
class IBEO;
class IPC;

class Control {
public:

	Control(std::string LogDir);
    	Control(const Control& orig);
    	virtual ~Control();

	void Setup();
	void Run();
	void Quit();
	void Trip(int TripState);
	void ToggleBrakeIL();
	void SendAlarm();

	bool HeartbeatState;
	int TripState;
	bool ManualOn;
	bool BrakeILOn;

	std::string LogDir;

	int CurrentSteeringSetPosn;
	int CurrentThrottleBrakeSetPosn;
 
	

private:

	bool RunState;

	void UpdateTerminal();
	void WriteInfoFile();

	GPSConnection* GPS;
	CarNetwork* CarNetworkConnection;
	Logger* Log;
	SafetySerialOut* SafetySerial;
	LowLevelSerialOut* LowLevelSerial;
	IBEO *Lux;
	IPC *WebIPC;

	Logger* WebLogger;

};



#endif	/* _CONTROL_H */
