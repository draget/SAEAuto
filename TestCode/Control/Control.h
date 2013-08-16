#ifndef _CONTROL_H
#define	_CONTROL_H

#define EARTH_RADIUS 6371000

#include <vector>
#include <string>

#include "PID.h"

class CarNetwork;
class Logger;
class SafetySerialOut;
class LowLevelSerialOut;
class GPSConnection;
class IBEO;
class IPC;
class Xsens;




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
	bool AutoRun;
	bool BrakeILOn;

	double DatumLat;
	double DatumLong;

	double AutoSpeedTarget;

	void AutoStart();
	void AutoPosUpdate(MAPPOINT_2D CurPosn);
	void AutoSpeedUpdate(double CurSpeed);
	void AutoTrackUpdate(double CurTrack);
	void AutoPause();
	void AutoContinue();
	void AutoStop();
	void CheckFenceposts(MAPPOINT_2D CurPosn);
	
	void ClearMap();

	double LatOffset;
	double LongOffset;

	unsigned int NextWaypoint;

	std::string LogDir;

	int CurrentSteeringSetPosn;
	int CurrentThrottleBrakeSetPosn;
 
	static double TimeStamp();

	MAPPOINT_2D LatLongToXY(double lat, double lng);

	static MAPPOINT_2D SubtractMapPoint(MAPPOINT_2D Point1, MAPPOINT_2D Point2);

private:

	bool RunState;

	void UpdateTerminal();
	void WriteInfoFile();

	void TimedBrake();

	double DesiredBearing;

	PID *SpeedController;
	PID *SteerController;

	MAP CurrentMap;

	GPSConnection* GPS;
	CarNetwork* CarNetworkConnection;
	Logger* Log;
	SafetySerialOut* SafetySerial;
	LowLevelSerialOut* LowLevelSerial;
	IBEO* Lux;
	IPC* WebIPC;
	Xsens* IMU;

	Logger* WebLogger;

};



#endif	/* _CONTROL_H */
