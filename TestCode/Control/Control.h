#ifndef _CONTROL_H
#define	_CONTROL_H

#define EARTH_RADIUS 6371000
#define MAPPOINT_RADIUS 2.5

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
class Fusion;




struct VECTOR_2D {
    double x;
    double y;
};


struct MAP {

	std::vector<VECTOR_2D> Fenceposts;
	std::vector<VECTOR_2D> Waypoints;

};

class Control {
public:

	double TwoPi;

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
	void DumpMap(std::string MapName);
	void ClearMap();

	bool HeartbeatState;
	int TripState;
	bool ManualOn;
	bool AutoOn;
	bool AutoRun;
	bool RecordActive;
	bool BrakeILOn; // If this is off we don't require a heartbeat or network connection for auto!

	double DatumLat;
	double DatumLong;

	double AutoSpeedTarget;

	void AutoStart();
	void AutoPosUpdate(VECTOR_2D CurPosn);
	void AutoSpeedUpdate(double CurSpeed);
	void AutoTrackUpdate(double CurTrack);
	void AutoPause();
	void AutoContinue();
	void AutoStop();
	void CheckFenceposts(VECTOR_2D CurPosn);
	void StopMapRecord(std::string MapName);
	void StopMapRecord();	
	void StartMapRecord();
	void MapRecordPosUpdate(VECTOR_2D CurPosn);

	
	

	double LatOffset;
	double LongOffset;

	unsigned int NextWaypoint;

	std::string LogDir;

	int CurrentSteeringSetPosn;
	int CurrentThrottleBrakeSetPosn;
 
	static double TimeStamp();

	VECTOR_2D LatLongToXY(double lat, double lng);
	
	static double VectorMagnitude(VECTOR_2D MapPoint);
	static VECTOR_2D SubtractVector(VECTOR_2D Point1, VECTOR_2D Point2);

	Xsens* IMU;
	GPSConnection* GPS;
	Fusion* Fuser;

private:

	bool RunState;

	void UpdateTerminal();
	void WriteInfoFile();

	void TimedBrake();

	double DesiredBearing;
	
	VECTOR_2D LastRecordedPoint;
	int MapRecordCounter;

	PID *ThrottleController;
	PID *BrakeController;
	PID *SteerController;

	MAP CurrentMap;

	
	CarNetwork* CarNetworkConnection;
	Logger* Log;
	SafetySerialOut* SafetySerial;
	LowLevelSerialOut* LowLevelSerial;
	IBEO* Lux;
	IPC* WebIPC;

	
	Logger* MapRecordLogger;

	Logger* WebLogger;

};



#endif	/* _CONTROL_H */
