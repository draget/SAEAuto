/*
* UWA Autonomous SAE Car Controller
* (C) Thomas Drage 2013
*
* Created June 2013
*
* Permission is given for use of this software in derivative works within the UWA Robotics and Automation laboratory,
* however acknowledgement must be given in all derived works and publications.
*
*/


#include <iostream>
#include <curses.h>
#include <csignal>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

#include "Control.h"
#include "CarNetwork.h"
#include "Logger.h"
#include "SafetySerialOut.h"
#include "LowLevelSerialOut.h"
#include "GPSConnection.h"
#include "IBEO.h"
#include "IPC.h"
#include "Xsens.h"
#include "Fusion.h"

#include "PID.h"

Control *SAECar;




/**
 * Purpose: Creates a new instance of the Control object.
 * Inputs : Name of directory logs are to be saved in.
 * Outputs: None.
 */
Control::Control(std::string LogDir, bool ExtLog) {

	TwoPi = 4*acos(0);

	this->LogDir = LogDir;
	this->ExtLogging = ExtLog;

	// Create directories for logging.
	mode_t process_mask = umask(0);
	mkdir(this->LogDir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	mkdir((LogDir + "/luxscan").c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	mkdir((LogDir + "/luxobj").c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	umask(process_mask);


	// Initialise variables...
	HeartbeatState = false;
	TripState = 0;
	ManualOn = false;
	AutoOn = false;
	BrakeILOn = true;
	RecordActive = false;

	FencepostRadius = MAPPOINT_RADIUS;

	DatumLat = -31.980569;
	DatumLong = 115.817807;

	LatOffset = 0.0;
	LongOffset = 0.0;

	CurrentSteeringSetPosn = 0;
	CurrentThrottleBrakeSetPosn = 0;


	// Create object instances...
	Log = new Logger(LogDir + "/mainlog.txt");

	Log->WriteLogLine("Control - Ext logging: " + boost::lexical_cast<std::string>(ExtLogging));

	CarNetworkConnection = new CarNetwork(this, Log);

	SafetySerial = new SafetySerialOut(this,Log);

	LowLevelSerial = new LowLevelSerialOut(this,Log);

	GPS = new GPSConnection(this,Log);

	Lux = new IBEO(this,Log);

	IMU = new Xsens(this,Log);

	WebIPC = new IPC(this,Log);

	Fuser = new Fusion(this,Log);

	WebLogger = new Logger("./ramdisk/weblog.txt");

}

Control::Control(const Control& orig) {
}

Control::~Control() {
}

/**
 * Purpose: Runs set up routines on all sensors/interfaces.
 * Inputs : None.
 * Outputs: None.
 */
void Control::Setup() {

	WebIPC->Open();
	CarNetworkConnection->Open();
	SafetySerial->Open();
	LowLevelSerial->Open();
	GPS->Open();
	if(access("noibeo", F_OK ) == -1) { Lux->Open(); }
	IMU->Open();

}

/**
 * Purpose: Handles cleanly closing the program.
 * Inputs : None.
 * Outputs: None.
 */
void Control::Quit() {

	Log->WriteLogLine("Caught SIGINT...bye!");

	Log->CloseLog();

	CarNetworkConnection->Disconnect();

	SafetySerial->Stop();

	LowLevelSerial->Stop();

	GPS->Stop();

	Lux->StopScanner();

	RunState = false;

}


/**
 * Purpose: Runs the main loop for Control class.
 * Inputs : None.
 * Outputs: None.
 */
void Control::Run() {

	RunState = true;

	// Start all sensors/interfaces.
	WebIPC->Start();
	CarNetworkConnection->StartProcessMessages();
	SafetySerial->Start();
	LowLevelSerial->Start();
	GPS->Start();
	Lux->Start();
	IMU->Start();

	// Loop through updating terminal and information output file.
	while(RunState) {

		UpdateTerminal();
		WriteInfoFile();
		if(CurrentMap.Waypoints.size() > 0 || CurrentMap.DetectedFenceposts.size() > 0) { DumpMap(); }

		boost::this_thread::sleep(boost::posix_time::milliseconds(250));

	}

}


/**
 * Purpose: Updates the terminal display.
 * Inputs : None.
 * Outputs: None.
 */
void Control::UpdateTerminal() {

	mvprintw(1,0,"Manual State: %i \n", this->ManualOn);
	mvprintw(2,0,"Trip State: %i \n", this->TripState);
	mvprintw(3,0,"HB State: %i \n", this->HeartbeatState);
	mvprintw(4,0,"Car Net State: %s \n", this->CarNetworkConnection->StatusString.c_str());
	mvprintw(5,0,"Safety Serial State: %i \n", this->SafetySerial->SerialState);
	mvprintw(6,0,"LowLevel Serial State: %i \n", this->LowLevelSerial->SerialState);
	mvprintw(7,0,"Brake IL Status: %i \n", this->BrakeILOn);

	mvprintw(9,0,"Current Steering Posn: %i \n", this->CurrentSteeringSetPosn);
	mvprintw(10,0,"Current Throttle/Brake Level: %i \n", this->CurrentThrottleBrakeSetPosn);

	mvprintw(8,50,"GPS State: %i \n", this->GPS->GPSState);
	mvprintw(9,50,"GPS Latitude: %lf \n", this->GPS->Latitude);
	mvprintw(10,50,"GPS Longitude: %lf \n", this->GPS->Longitude);
	mvprintw(11,50,"GPS Speed: %lf \n", this->GPS->Speed);
	mvprintw(12,50,"GPS Track Angle: %lf \n", this->GPS->TrackAngle);

	mvprintw(3,50,"IBEO State: %i \n", this->Lux->inUse);
	mvprintw(4,50,"IBEO N Objects: %i \n", this->Lux->object_data_header[this->Lux->curObjectDataSource].number_of_objects);
	mvprintw(5,50,"IBEO N Scan Pts: %i \n", this->Lux->scan_data_header[this->Lux->curScanDataSource].scan_points);
	mvprintw(6,50,"IBEO Scan No: %i \n", this->Lux->scan_data_header[this->Lux->curScanDataSource].scan_number);

	int y,x;
	getmaxyx(stdscr,y,x);

	int TitleX = 0;

	if((x/2 - 28) > 0) { TitleX = x/2 - 28; }

	mvprintw(0,TitleX,"UWA AUTONOMOUS SAE CAR CONTROLLER - THOMAS DRAGE 2013\n");

	int LogStartLine = 14;

	if(y > LogStartLine) {

		std::string RecentLogLines[y - LogStartLine];
		this->Log->GetLogLines(RecentLogLines, y - LogStartLine);
			
		for(int i = y; i > LogStartLine; i--) {

			mvprintw(i-1,0,"Log: %s", (RecentLogLines[y - i] + '\n').c_str());

		}

	}

	refresh();

}

/**
 * Purpose: Updates the weblog.txt info output file.
 * Inputs : None.
 * Outputs: None.
 */
void Control::WriteInfoFile() {

	WebLogger->WriteLock();
	WebLogger->ClearLog();

	WebLogger->WriteLogLine("Manual State|" + boost::lexical_cast<std::string>(this->ManualOn), true);
	WebLogger->WriteLogLine("Auto On|" + boost::lexical_cast<std::string>(this->AutoOn), true);
	WebLogger->WriteLogLine("Auto Run|" + boost::lexical_cast<std::string>(this->AutoRun), true);
	WebLogger->WriteLogLine("Trip State|" + boost::lexical_cast<std::string>(this->TripState), true);
	WebLogger->WriteLogLine("HB State|" + boost::lexical_cast<std::string>(this->HeartbeatState), true);
	WebLogger->WriteLogLine("Car Net State|" + boost::lexical_cast<std::string>(this->CarNetworkConnection->StatusString.c_str()), true);
	WebLogger->WriteLogLine("Safety Serial State|" + boost::lexical_cast<std::string>(this->SafetySerial->SerialState), true);
	WebLogger->WriteLogLine("LowLevel Serial State|" + boost::lexical_cast<std::string>(this->LowLevelSerial->SerialState), true);

	WebLogger->WriteLogLine("Brake IL Status|" + boost::lexical_cast<std::string>(this->BrakeILOn), true);

	WebLogger->WriteLogLine("Current Steering Posn|" + boost::lexical_cast<std::string>(this->CurrentSteeringSetPosn), true);
	WebLogger->WriteLogLine("Current Throttle/Brake Level|" + boost::lexical_cast<std::string>(this->CurrentThrottleBrakeSetPosn), true);

	WebLogger->WriteLogLine("GPS State|" + boost::lexical_cast<std::string>(this->GPS->GPSState), true);
	WebLogger->WriteLogLine("GPS Latitude|" + boost::lexical_cast<std::string>(this->GPS->Latitude), true);
	WebLogger->WriteLogLine("GPS Longitude|" + boost::lexical_cast<std::string>(this->GPS->Longitude), true);
	WebLogger->WriteLogLine("GPS Speed|" + boost::lexical_cast<std::string>(this->GPS->Speed), true);
	WebLogger->WriteLogLine("GPS Track Angle|" + boost::lexical_cast<std::string>(this->GPS->TrackAngle), true);
	WebLogger->WriteLogLine("GPS Time|" + boost::lexical_cast<std::string>(this->GPS->Time), true);

	WebLogger->WriteLogLine("Datum Lat|" + boost::lexical_cast<std::string>(this->DatumLat), true);
	WebLogger->WriteLogLine("Datum Long|" + boost::lexical_cast<std::string>(this->DatumLong), true);

	WebLogger->WriteLogLine("Offset Lat|" + boost::lexical_cast<std::string>(this->LatOffset), true);
	WebLogger->WriteLogLine("Offset Long|" + boost::lexical_cast<std::string>(this->LongOffset), true);

	WebLogger->WriteLogLine("IMU Heading|" + boost::lexical_cast<std::string>(this->IMU->Yaw), true);

	WebLogger->WriteLogLine("Desired Bearing|" + boost::lexical_cast<std::string>(this->DesiredBearing), true);
	WebLogger->WriteLogLine("NextWaypoint|" + boost::lexical_cast<std::string>(this->NextWaypoint), true);

	WebLogger->WriteLogLine("Fused X Pos|" + boost::lexical_cast<std::string>(Fuser->CurrentPosition.x), true);
	WebLogger->WriteLogLine("Fused Y Pos|" + boost::lexical_cast<std::string>(Fuser->CurrentPosition.y), true);
	WebLogger->WriteLogLine("Fused X Vel|" + boost::lexical_cast<std::string>(Fuser->CurrentVelocity.x), true);
	WebLogger->WriteLogLine("Fused Y Vel|" + boost::lexical_cast<std::string>(Fuser->CurrentVelocity.y), true);
	WebLogger->WriteLogLine("Fused Speed|" + boost::lexical_cast<std::string>(Fuser->CurrentSpeed), true);
	WebLogger->WriteLogLine("Fused Heading|" + boost::lexical_cast<std::string>(Fuser->CurrentHeading), true);

	WebLogger->WriteLogLine("IBEO State|" + boost::lexical_cast<std::string>(this->Lux->inUse), true);
	WebLogger->WriteLogLine("IBEO N Objects|" + boost::lexical_cast<std::string>(this->Lux->object_data_header[this->Lux->curObjectDataSource].number_of_objects), true);
	WebLogger->WriteLogLine("IBEO N Scan Pts|" + boost::lexical_cast<std::string>(this->Lux->scan_data_header[this->Lux->curScanDataSource].scan_points), true);
	WebLogger->WriteLogLine("IBEO Scan No|" + boost::lexical_cast<std::string>(this->Lux->scan_data_header[this->Lux->curScanDataSource].scan_number), true);

	WebLogger->WriteLogLine("IBEO LH Edge|" + boost::lexical_cast<std::string>(this->Lux->LHEdge), true);
	WebLogger->WriteLogLine("IBEO RH Edge|" + boost::lexical_cast<std::string>(this->Lux->RHEdge), true);
	WebLogger->WriteLogLine("IBEO Road Slope|" + boost::lexical_cast<std::string>(this->Lux->RoadSlope), true);
	WebLogger->WriteLogLine("IBEO Road Intcpt|" + boost::lexical_cast<std::string>(this->Lux->RoadIntercept), true);

	int y = 30;
	
	std::string RecentLogLines[y];
	this->Log->GetLogLines(RecentLogLines, y);
			
	for(int i = y; i > 0; i--) {

		WebLogger->WriteLogLine("Log|" + RecentLogLines[y - i], true);

	}

	WebLogger->ClearLock();

}


/**
 * Purpose: Actions trips.
 * Inputs : The new trip state (type of trip).
 * Outputs: None.
 */
void Control::Trip(int TripState) {

	std::string TripReason;

	this->TripState = TripState;

	if(TripState == 1) {
		TripReason = "Base ESTOP";
	}
	else if(TripState == 2) {
		TripReason = "Couldn't send HB";
	}
	else if(TripState == 3) {
		TripReason = "Safety supervisor initiated trip.";
	}
	else if(TripState == 4) {
		TripReason = "Safety supervisor msgs not ack'd.";
	}
	else if(TripState == 5) {
		TripReason = "Error sending low level commands.";
	}
	else if(TripState == 6) {
		TripReason = "Network error!";
	}
	else if(TripState == 7) {
		TripReason = "GPS Error";
	}
	else if(TripState == 8) {
		TripReason = "Autonomous issue";
	}
	else if(TripState == 9) {
		TripReason = "Web IPC estop";
	}
	else if(TripState == 10) {
		TripReason = "Low Level Error";
	}
 

	// Stop things from happening...
	AutoOn = false;
	AutoRun = false;

	if(BrakeILOn) { CurrentThrottleBrakeSetPosn = -256; }
	else { CurrentThrottleBrakeSetPosn = 0; }

	Log->WriteLogLine("Control - changed to trip state " + boost::lexical_cast<std::string>(TripState) + " reason: " + TripReason);

}

/**
 * Purpose: Sets the trip state back to zero and turns off the E-brake. Does not reset hardware supervisor.
 * Inputs : None.
 * Outputs: None.
 */
void Control::Untrip() {

	TripState = 0;
	CurrentThrottleBrakeSetPosn = 0;

	Log->WriteLogLine("Control - trip state returned to zero.");

}

/**
 * Purpose: Toggle the safety mode (BrakeIL) from human driving to no driver and vice-versa...
 * Inputs : None.
 * Outputs: None.
 */
void Control::ToggleBrakeIL() {

	bool Success;

	if(! this->BrakeILOn) { Success = SafetySerial->Send('B'); }
	else { Success = SafetySerial->Send('H'); }

	if(Success) {
		this->BrakeILOn = ! this->BrakeILOn;
		Log->WriteLogLine("Control - Brake IL toggled " + this->BrakeILOn);
	}
	else {
		Log->WriteLogLine("Control - Brake IL toggle failed.");
	}


}


/**
 * Purpose: Loads a map file into the CurrentMap struct ready for execution.
 * Inputs : Name of map.
 * Outputs: None.
 */
void Control::LoadMap(std::string MapFilename) {

	ClearMap();

	std::ifstream infile(("../../FrontEnd/Maps/maps/" + MapFilename).c_str());

	if (infile.is_open()) {

		int f = 0;
		int w = 0;

		VECTOR_2D MapPoint;

		while ( infile.good() ) {
			std::string line;
      			getline (infile,line);

			boost::algorithm::trim(line);

			if(line.empty()) { continue; }

			std::vector<std::string> LineParts;
			boost::algorithm::split(LineParts, line,  boost::algorithm::is_any_of(","));

			if(LineParts[0].compare(0,1,"D") == 0) { 
				DatumLat = boost::lexical_cast<double>(LineParts[1]); 
				DatumLong = boost::lexical_cast<double>(LineParts[2]);
				continue;
			}

			MapPoint.x = boost::lexical_cast<double>(LineParts[1]);
			MapPoint.y = boost::lexical_cast<double>(LineParts[2]);


			if(LineParts[0].compare(0,1,"F") == 0 ) {
				f++;
				CurrentMap.Fenceposts.push_back(MapPoint);
			}
			else if(LineParts[0].compare(0,1,"I") == 0) { continue; }
			else {
				w++;
				CurrentMap.Waypoints.push_back(MapPoint);
			}

    		}
    		infile.close();
		Log->WriteLogLine("Control - loaded " + boost::lexical_cast<std::string>(f) + " fence and " + boost::lexical_cast<std::string>(w) + " waypoints.");
  	}
	else { Log->WriteLogLine("Control - Couldn't open map"); }
	


}

/**
 * Purpose: Empties the loaded map.
 * Inputs : None.
 * Outputs: None.
 */
void Control::ClearMap() {

	NextWaypoint = 0;

	CurrentMap.Fenceposts.clear();
	CurrentMap.Waypoints.clear();
	CurrentMap.DetectedFenceposts.clear();

}

/**
 * Purpose: Calls DumpMap with default path.
 * Inputs : None.
 * Outputs: None.
 */
void Control::DumpMap() {

	DumpMap("./ramdisk/runningmap.txt");

}


/**
 * Purpose: Saves current map to a wpy file.
 * Inputs : Path to save map.
 * Outputs: None.
 */
void Control::DumpMap(std::string MapName) {

	// Create a new logger, set a lock and empty the file if it exists.
	Logger* DumpLog = new Logger(MapName);

	DumpLog->WriteLock();
	DumpLog->ClearLog();

	// Write Datum line.
	DumpLog->WriteLogLine("D," + boost::lexical_cast<std::string>(DatumLat) + "," + boost::lexical_cast<std::string>(DatumLong), true);

	// Dump the waypoints, fence posts and detected points.
	int i = 0;
	BOOST_FOREACH( VECTOR_2D MapPoint, CurrentMap.Waypoints ) {
   		DumpLog->WriteLogLine(boost::lexical_cast<std::string>(i) + "," + boost::lexical_cast<std::string>(MapPoint.x) + "," + boost::lexical_cast<std::string>(MapPoint.y), true);
		i++;
	}

	i = 0;
	BOOST_FOREACH( VECTOR_2D MapPoint, CurrentMap.Fenceposts ) {
   		DumpLog->WriteLogLine("F" + boost::lexical_cast<std::string>(i) + "," + boost::lexical_cast<std::string>(MapPoint.x) + "," + boost::lexical_cast<std::string>(MapPoint.y), true);
		i++;
	}

	i = 0;
	BOOST_FOREACH( VECTOR_2D MapPoint, CurrentMap.DetectedFenceposts ) {
   		DumpLog->WriteLogLine("I" + boost::lexical_cast<std::string>(i) + "," + boost::lexical_cast<std::string>(MapPoint.x) + "," + boost::lexical_cast<std::string>(MapPoint.y), true);
		i++;
	}

	DumpLog->CloseLog();
	DumpLog->ClearLock();	
}


/**
 * Purpose: Starts Autonomous driving.
 * Inputs : None.
 * Outputs: None.
 */
void Control::AutoStart() {

	if(ExtLogging) { AutoLogger = new Logger(LogDir + "/autolog.txt"); }

	if(CurrentMap.Waypoints.size() == 0) { Log->WriteLogLine("Control - No map loaded, can't start auto."); return; }

	if(! CarNetworkConnection->HasConnection && BrakeILOn) { Log->WriteLogLine("Control - No base connection, can't start auto."); return; }

	NextWaypoint = 0;
	ManualOn = false;
	AutoOn = true;
	AutoRun = true;
	AutoSpeedTarget = 0;

	//Initialise the three PID controllers.

	ThrottleController = new PID(30.0,5.0,0,0.1);
	ThrottleController->setInputLimits(0.0, 30);
	ThrottleController->setOutputLimits(-255,255);
	ThrottleController->setMode(AUTO_MODE);

	BrakeController = new PID(40.0,10.0,0,0.1);
	BrakeController->setInputLimits(0.0, 30);
	BrakeController->setOutputLimits(-255,255);
	BrakeController->setMode(AUTO_MODE);

	SteerController = new PID(6.3,0.5,0.0,0.1);
	SteerController->setInputLimits(-360, 720);
	SteerController->setOutputLimits(-127,127);
	SteerController->setMode(AUTO_MODE);

	SteerController->setSetPoint(0);
	ThrottleController->setSetPoint(0);
}


/**
 * Purpose: Check that the current position isn't dangerously close to a fencepost.
 * Inputs : None.
 * Outputs: None.
 */
void Control::CheckFenceposts(VECTOR_2D CurPosn) {

	BOOST_FOREACH( VECTOR_2D Fencepost, CurrentMap.Fenceposts ) {
		VECTOR_2D DistanceVector = SubtractVector(Fencepost,CurPosn);
		if(sqrt(pow(DistanceVector.x,2) + pow(DistanceVector.y,2)) < FencepostRadius) {
			Log->WriteLogLine("Control - Near fence post!");
			Control::AutoStop();
			if(TripState == 0) { Trip(8); }
		}
	}

}


/**
 * Purpose: Updates controller set points when a new position is received during autonomous driving.
 * Inputs : New position vector.
 * Outputs: None.
 */
void Control::AutoPosUpdate(VECTOR_2D CurPosn) {

	// Check if we have reached a waypoint.
	VECTOR_2D DistanceVector = SubtractVector(CurrentMap.Waypoints[NextWaypoint],CurPosn);
	if(VectorMagnitude(DistanceVector) < MAPPOINT_RADIUS) {
		Log->WriteLogLine("Control - Reached waypoint " + boost::lexical_cast<std::string>(NextWaypoint));
		NextWaypoint++;
		if(NextWaypoint >= CurrentMap.Waypoints.size()) { Log->WriteLogLine("Control - Reached end of map."); Control::AutoStop(); return; }
	}

	// Calculate the vector to the next waypoint.
	VECTOR_2D VectorToNextWp = SubtractVector(CurrentMap.Waypoints[NextWaypoint], CurPosn);

	//Log->WriteLogLine("Current xy " + boost::lexical_cast<std::string>(CurPosn.x) + " " +  boost::lexical_cast<std::string>(CurPosn.y));
	//Log->WriteLogLine("Current vec " + boost::lexical_cast<std::string>(VectorToNextWp.x) + " " +  boost::lexical_cast<std::string>(VectorToNextWp.y));
	//Log->WriteLogLine("next wp " + boost::lexical_cast<std::string>(CurrentMap.Waypoints[NextWaypoint].x) + " " +  boost::lexical_cast<std::string>(CurrentMap.Waypoints[NextWaypoint].y));

	// Calculate an angle bearing.
	if(VectorToNextWp.y > 0 && VectorToNextWp.x < 0) { DesiredBearing = 360 + 90 - 360*atan2(VectorToNextWp.y,VectorToNextWp.x)/TwoPi; }
	else { DesiredBearing = 90 - 360*atan2(VectorToNextWp.y,VectorToNextWp.x)/TwoPi; }
	//Log->WriteLogLine("pre flip: " + boost::lexical_cast<std::string>(DesiredBearing));

	
	// Sometimes we need to go out of bounds on the bearing to smooth north crossings and make sure we turn the shortest dirn.
	if(DesiredBearing > (180 + Fuser->CurrentHeading) && Fuser->CurrentHeading < 180) { DesiredBearing = DesiredBearing - 360; } // Turning from NE to NW quadrant.
	if(DesiredBearing < (Fuser->CurrentHeading - 180) && Fuser->CurrentHeading > 180) { DesiredBearing = DesiredBearing + 360; } // Turning from NW to NE quadrant.

	SteerController->setSetPoint(DesiredBearing);

	// Speed profile based on turn radius.
	if(CurrentSteeringSetPosn > 60) { DesiredSpeed = 0.6; }
	else { DesiredSpeed = 1.0; }

	ThrottleController->setSetPoint(DesiredSpeed); 
	BrakeController->setSetPoint(DesiredSpeed);

	if(ExtLogging) { 
		std::string time = boost::lexical_cast<std::string>(TimeStamp());
		AutoLogger->WriteLogLine("DB," + time + "," + boost::lexical_cast<std::string>(DesiredBearing), true);
		AutoLogger->WriteLogLine("DS," + time + "," + boost::lexical_cast<std::string>(DesiredSpeed), true);
	}

}

/**
 * Purpose: Performs control loop actions when a new speed is received during autonomous driving.
 * Inputs : New speed value.
 * Outputs: None.
 */
void Control::AutoSpeedUpdate(double CurSpeed) {

	ThrottleController->setProcessValue(CurSpeed);
	BrakeController->setProcessValue(CurSpeed);

	
	double SpeedIncrement = ThrottleController->compute();
	double BrakeValue = BrakeController->compute();

	std::string time;
	
	if(ExtLogging) { 
		time = boost::lexical_cast<std::string>(TimeStamp());
		AutoLogger->WriteLogLine("SI,"  + time + "," + boost::lexical_cast<std::string>(SpeedIncrement), true);
		AutoLogger->WriteLogLine("BV,"  + time + "," + boost::lexical_cast<std::string>(BrakeValue), true);
	}

	if(BrakeValue < 0) { CurrentThrottleBrakeSetPosn = BrakeValue; }

	else {

		if(TripState == 0) { 

			if((CurrentThrottleBrakeSetPosn + SpeedIncrement) > 255) {
				CurrentThrottleBrakeSetPosn = 255;
			}
			else if ((CurrentThrottleBrakeSetPosn + SpeedIncrement) < 0) {
				CurrentThrottleBrakeSetPosn = 0;
			}
			else {
				CurrentThrottleBrakeSetPosn = CurrentThrottleBrakeSetPosn + SpeedIncrement; 
			}

		}
		else { CurrentThrottleBrakeSetPosn = 0; }
	}

	if(ExtLogging) { AutoLogger->WriteLogLine("TB," + time + "," + boost::lexical_cast<std::string>(CurrentThrottleBrakeSetPosn), true); }
		
}


/**
 * Purpose: Performs control loop actions when a new heading is received during autonomous driving.
 * Inputs : New heading value.
 * Outputs: None.
 */
void Control::AutoTrackUpdate(double CurTrack) {

	// If we cross north before the desired bearing is recalculated it will need fixing!
	if(DesiredBearing < 0 && CurTrack > 180) { DesiredBearing = DesiredBearing + 360; SteerController->setSetPoint(DesiredBearing); }
	if(DesiredBearing > 360 && CurTrack < 180) { DesiredBearing = DesiredBearing - 360; SteerController->setSetPoint(DesiredBearing); }

	SteerController->setProcessValue(CurTrack);
	CurrentSteeringSetPosn = SteerController->compute();

	if(ExtLogging) { AutoLogger->WriteLogLine("SS," + boost::lexical_cast<std::string>(TimeStamp()) + "," + boost::lexical_cast<std::string>(CurrentSteeringSetPosn), true); }

}

/**
 * Purpose: Temporarily suspend autonomous driving.
 * Inputs : None.
 * Outputs: None.
 */
void Control::AutoPause() {
	
	AutoRun = false;
	boost::thread brake_Thread = boost::thread(&Control::TimedBrake, this);
	brake_Thread.detach();

	Log->WriteLogLine("Control - autonomous pause.");

}

/**
 * Purpose: Resume autonomous driving.
 * Inputs : None.
 * Outputs: None.
 */
void Control::AutoContinue() {
	
	AutoRun = true;

	Log->WriteLogLine("Control - autonomous continue.");

}

/**
 * Purpose: Stop autonomous driving.
 * Inputs : None.
 * Outputs: None.
 */
void Control::AutoStop() {

	AutoRun = false;
	AutoOn = false;
	CurrentThrottleBrakeSetPosn = 0;
	CurrentSteeringSetPosn = 0;

	boost::thread brake_Thread = boost::thread(&Control::TimedBrake, this);
	brake_Thread.detach();

	if(ExtLogging) { AutoLogger->CloseLog(); }

	Log->WriteLogLine("Control - autonomous stop.");

}

/**
 * Purpose: Start automatic map recording mode.
 * Inputs : None.
 * Outputs: None.
 */
void Control::StartMapRecord() {

	ClearMap();

//	DatumLat = GPS->Latitude;
//	DatumLong = GPS->Longitude;

//	boost::this_thread::sleep(boost::posix_time::milliseconds(2000));

	RecordActive = true;

	Log->WriteLogLine("Control - Recording map points...");

}

/**
 * Purpose: Act on new position data during map recording.
 * Inputs : None.
 * Outputs: None.
 */
void Control::MapRecordPosUpdate(VECTOR_2D CurPosn) {

	if(VectorMagnitude(SubtractVector(CurPosn,LastRecordedPoint)) > MAPPOINT_RADIUS) {
		CurrentMap.Waypoints.push_back(CurPosn);
		LastRecordedPoint = CurPosn;
	}

}

/**
 * Purpose: Stop automatic map recording and save with default name.
 * Inputs : None.
 * Outputs: None.
 */
void Control::StopMapRecord() {

	StopMapRecord(boost::lexical_cast<std::string>(TimeStamp()));

}

/**
 * Purpose: Stop automatic map recording.
 * Inputs : Map file name.
 * Outputs: None.
 */
void Control::StopMapRecord(std::string MapName) {

	RecordActive = false;

	DumpMap("../../FrontEnd/Maps/maps/" + MapName + ".wyp");
	
	Log->WriteLogLine("Control - Finished recording " + boost::lexical_cast<std::string>(CurrentMap.Waypoints.size()) + " map points.");

}

/**
 * Purpose: Brake for two seconds. Needs to be run its own thread as to not block execution.
 * Inputs : None.
 * Outputs: None.
 */
void Control::TimedBrake() {

	Log->WriteLogLine("Control - Braking for 2s.");

	CurrentThrottleBrakeSetPosn = -255;
	boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
	CurrentThrottleBrakeSetPosn = 0;

}

/**
 * Purpose: Tell the safety serial box to beep.
 * Inputs : None.
 * Outputs: None.
 */
void Control::SendAlarm() {

	if(SafetySerial->SerialState) { SafetySerial->Send('A'); }

}

// Utility functions below.

double Control::TimeStamp() {

	timeval current;
	gettimeofday(&current,NULL);

	return current.tv_sec + (double)current.tv_usec/1000000;

}

VECTOR_2D Control::LatLongToXY(double lat, double lng) {

	VECTOR_2D MapPoint;

	MapPoint.y = -1*EARTH_RADIUS*TwoPi*(DatumLat - lat)/360;
	MapPoint.x = -1*EARTH_RADIUS*cos(abs(lat))*TwoPi*(DatumLong - lng)/360;

	return MapPoint;

}

VECTOR_2D Control::SubtractVector(VECTOR_2D Point1, VECTOR_2D Point2) {

	VECTOR_2D Result;

	Result.x = Point1.x - Point2.x;
	Result.y = Point1.y - Point2.y;

	return Result;

}

double Control::VectorMagnitude(VECTOR_2D MapPoint) {

	return sqrt(pow(MapPoint.x,2) + pow(MapPoint.y,2));

}

// C-style non -member functions follow.

void HandleExit(int param) {

	SAECar->Quit();

}

/*
* Entry point to the program.
* Argument to executable is the name of the folder to save logs in for this run. Default folder name is '0'.
*/
int main(int argc, char *argv[]) {

	std::string LogDir;

	bool ExtLogging = true;

	if(argc > 1) {
		std::string Arg = boost::lexical_cast<std::string>(argv[1]);
		if(Arg.compare(0,4,"null") == 0) { ExtLogging = false; }
		else { LogDir = "./RunFiles/" + Arg; }
	}
	else { system("rm -rf ./RunFiles/0"); LogDir = "./RunFiles/0"; }


	// Set up the ncurses terminal display.
	initscr();
	noecho();
	nodelay(stdscr, 1);
	keypad(stdscr, 1);

	SAECar = new Control(LogDir,ExtLogging);

	std::signal(SIGINT, HandleExit);

	SAECar->Setup();

	SAECar->Run(); // Execution happens in this function.

	cout << "Close.\n";

	echo();
	endwin();


	return 0;

}




