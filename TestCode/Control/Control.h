#ifndef _CONTROL_H
#define	_CONTROL_H

#define EARTH_RADIUS 6371000

#include <vector>
#include <string>

class CarNetwork;
class Logger;
class SafetySerialOut;
class LowLevelSerialOut;
class GPSConnection;
class IBEO;
class IPC;


struct MAPPOINT_2D {
    double x;
    double y;
};


struct MAP {

	std::vector<MAPPOINT_2D> Fenceposts;
	std::vector<MAPPOINT_2D> Waypoints;

};

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

	void LoadMap(std::string MapFilename);
	void DumpMap();

	bool HeartbeatState;
	int TripState;
	bool ManualOn;
	bool AutoOn;
	bool BrakeILOn;

	double DatumLat;
	double DatumLong;

	double AutoSpeedTarget;

	int NextWaypoint;

	std::string LogDir;

	int CurrentSteeringSetPosn;
	int CurrentThrottleBrakeSetPosn;
 
	static double TimeStamp();

private:

	bool RunState;

	void UpdateTerminal();
	void WriteInfoFile();

	MAP CurrentMap;

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
