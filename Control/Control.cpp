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

#include <sys/time.h>

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
#include "spline.hpp"

#include "matlab/matlab_emxAPI.h"
#include "matlab/arclengthcurve.h"
#include "matlab/builddetailedbf.h"
#include "matlab/parevalspline.h"
#include "matlab/buildbfcurvature.h"
#include "matlab/oblocalize.h"
#include "matlab/localize.h"
#include "matlab/evalheading.h"
#include "matlab/buildmanouvers.h"
#include "matlab/checkpathcollision.h"
#include "matlab/equatesafetycost.h"
#include "matlab/equateoffsetcost.h"
#include "matlab/mincost.h"
#include "matlab/equateconscost.h"
#include "matlab/genprevpathq.h"

static timestamp_t get_timestamp()
{
  struct timeval now;
  gettimeofday (&now, NULL);
  return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}

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
	
	TimeLog = new Logger(LogDir + "/timelog.txt");

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
	
	if(ExtLogging) { AutoLogger = new Logger(LogDir + "/autolog.txt"); }
	
	JunkLogger = new Logger("./ramdisk/junklog.txt");
	

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
	//SafetySerial->Open();
	//LowLevelSerial->Open();
	//GPS->Open();
	if(access("noibeo", F_OK ) == -1) { Lux->Open(); }
	//IMU->Open();
	
	//Set up simulator CAR RX
	bool OpenStateSim = false;	
	boost::thread s_Thread;

	mode_t process_mask = umask(0);
	int resSim = mkfifo("./CAR_IPC_FIFO_RX", S_IRWXU | S_IRWXG | S_IRWXO);
	umask(process_mask);

	if(resSim != 0) { Log->WriteLogLine("SIM - Couldn't create fifo!"); }
	
	OpenStateSim = true;

	RXpipeSim = fopen("./CAR_IPC_FIFO_RX", "r+");
	if(RXpipeSim == NULL) { Log->WriteLogLine("SIM - RX Pipe open failed..."); OpenStateSim = false; }
	else { Log->WriteLogLine("SIM - Pipe open okay."); }
	
	if(OpenStateSim) {	

		s_Thread = boost::thread(&Control::ProcessSimMessages, this);
		s_Thread.detach();

	}
	
	TXpipeSim = fopen("/home/martinfrench/Desktop/ode-0.13/ode/demo/SIM_IPC_FIFO_RX", "w");
	if(TXpipeSim == NULL) { Log->WriteLogLine("SIM - TX Pipe open failed..."); OpenStateSim = false; }
	
	PathPlan.manxi = emxCreate_real_T(1,1);
	PathPlan.manyi = emxCreate_real_T(1,1);
	
	

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
	//GPS->Start();
	//Lux->Start();
	//IMU->Start();
	//SafetySerial->Start();

	boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	SendAlarm();
	boost::this_thread::sleep(boost::posix_time::milliseconds(400));

	//LowLevelSerial->Start();


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
	
	WebLogger->WriteLogLine("Advanced Path Planning Active|" + boost::lexical_cast<std::string>(this->PathPlan.active), true);

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

	ResetTrip = true;

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

	if(true) {
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

	std::ifstream infile(("../FrontEnd/Maps/maps/" + MapFilename).c_str());

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
    		
    	timestamp_t t0 = get_timestamp();

		PathPlan.points = emxCreateWrapper_real_T(&CurrentMap.Waypoints[0].x, 2, w); //Create waypoint matrix by wrapping Waypoints vector
		
		
		PathPlan.scoefx = emxCreate_real_T(1,1); //Create arc length parameterised x coeficients matrix
		PathPlan.scoefy = emxCreate_real_T(1,1); //Create arc length parameterised y coeficients matrix
		PathPlan.si = emxCreate_real_T(1,1); //Create arc length parameterised breaks matrix
		
		
		//Calculate an arc length paramterised path based on the original waypoints with the arc length path matching the original path at PATHESTIMATEGRANULARITY intervals. 
		arclengthcurve(PathPlan.points, PATHESTIMATEGRANULARITY, EPSILON, 
						PathPlan.scoefx, PathPlan.scoefy, PathPlan.si);
						
		Log->WriteLogLine("#Curves: " + boost::lexical_cast<std::string>(PathPlan.si->size[0]));
		Log->WriteLogLine("si[0]: " + boost::lexical_cast<std::string>(PathPlan.si->data[0]));
		Log->WriteLogLine("si[1]: " + boost::lexical_cast<std::string>(PathPlan.si->data[1]));
		Log->WriteLogLine("si[2]: " + boost::lexical_cast<std::string>(PathPlan.si->data[2]));
		Log->WriteLogLine("si[3]: " + boost::lexical_cast<std::string>(PathPlan.si->data[3]));
		Log->WriteLogLine("si[4]: " + boost::lexical_cast<std::string>(PathPlan.si->data[4]));
						
		timestamp_t t1 = get_timestamp();
					
		emxArray_real_T *sx = emxCreate_real_T(1,1); //Create baseframe x point samples matrix, only used in plotting
		emxArray_real_T *sy = emxCreate_real_T(1,1); //Create baseframe y point samples matrix, only used in plotting
		PathPlan.ss = emxCreate_real_T(1,1); //Create baseframe arc length samples matrix
		
		//Calculate detailed sample points at a distance GRANULATRITY between each point.
		builddetailedbf(PathPlan.scoefx, PathPlan.scoefy, PathPlan.si, GRANULARITY,
						sx, sy, PathPlan.ss);
		
		timestamp_t t2 = get_timestamp();
		
		//Put calculated detailes baseframe samples into the current map baseframe vector.
		for(int i = 0; i < sx->size[0]; i++) {
			MapPoint.x = sx->data[i];
			MapPoint.y = sy->data[i];
			PathPlan.BaseFrame.push_back(MapPoint);
		}
		Log->WriteLogLine("#Samples: " + boost::lexical_cast<std::string>(PathPlan.ss->size[0]));
		Log->WriteLogLine("Total Baseframe Path Length: " + boost::lexical_cast<std::string>(PathPlan.si->data[PathPlan.si->size[0]-1]));
		
		timestamp_t t3 = get_timestamp();
		
		PathPlan.dxds = emxCreate_real_T(1,1); //Create first derivative of x with respect to arc length matrix
		PathPlan.dyds = emxCreate_real_T(1,1); //Create first derivative of y with respect to arc length matrix
		emxArray_real_T *dx2ds = emxCreate_real_T(1,1); //Create second derivative of x with respect to arc length matrix
		emxArray_real_T *dy2ds = emxCreate_real_T(1,1); //Create second derivative of y with respect to arc length matrix
		
		emxArray_real_T *dontcare = emxCreate_real_T(1,1); //Create a dontcare matrix because we dont care about the curvn
		
		//Calculate derivatives
		parevalspline(PathPlan.scoefx,PathPlan.si,PathPlan.ss,1,PathPlan.dxds,dontcare);
		parevalspline(PathPlan.scoefy,PathPlan.si,PathPlan.ss,1,PathPlan.dyds,dontcare);
		parevalspline(PathPlan.scoefx,PathPlan.si,PathPlan.ss,2,dx2ds,dontcare);
		parevalspline(PathPlan.scoefy,PathPlan.si,PathPlan.ss,2,dy2ds,dontcare);
		
		timestamp_t t4 = get_timestamp();
		
		
		PathPlan.BaseFrameCurvature = emxCreate_real_T(1,1); //Create base frame curvature matrix, store in pathplanning struct
		
		//Calculate baseframe curvature
		buildbfcurvature(PathPlan.dxds, PathPlan.dyds, dx2ds, dy2ds, PathPlan.BaseFrameCurvature);
		
		
		
		timestamp_t t5 = get_timestamp();
		
		//Create prev path offset matrix and set to 0
		PathPlan.prevpathq = emxCreate_real_T(0,0);
		
		double secs1 = (t1 - t0) / 1000000.0L;
		double secs2 = (t2 - t1) / 1000000.0L;
		double secs3 = (t3 - t2) / 1000000.0L;
		double secs4 = (t4 - t3) / 1000000.0L;
		double secs5 = (t5 - t4) / 1000000.0L;
		
		
		
		Log->WriteLogLine("#Curvature Samples: " + boost::lexical_cast<std::string>(PathPlan.BaseFrameCurvature->size[0]));
		TimeLog->WriteLogLine("Time to execute arclengthcurve: " + boost::lexical_cast<std::string>(secs1) + " s");
		TimeLog->WriteLogLine("Time to execute builddetailedbf: " + boost::lexical_cast<std::string>(secs2) + " s");
		TimeLog->WriteLogLine("Time to push baseframe onto currentmap: " + boost::lexical_cast<std::string>(secs3) + " s");
		TimeLog->WriteLogLine("Time to execute parevalspline derivitives: " + boost::lexical_cast<std::string>(secs4) + " s");
		TimeLog->WriteLogLine("Time to execute buildbfcurvature: " + boost::lexical_cast<std::string>(secs5) + " s");
		
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
	PathPlan.BaseFrame.clear();
	PathPlan.PlannedWaypoints.clear();

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
	
	int npaths = PathPlan.manxi->size[1];
	int npoints = PathPlan.manxi->size[0];
	double x;
	double y;
	
	for (i = 0; i<npaths; i++) {
		for (int j = 0; j<npoints; j++){
			x = PathPlan.manxi->data[i*npoints + j];
			y = PathPlan.manyi->data[i*npoints + j];
			if (i == PathPlan.selectedpath - 1) {
				DumpLog->WriteLogLine("S," + boost::lexical_cast<std::string>(j) + "," + boost::lexical_cast<std::string>(x) + "," + boost::lexical_cast<std::string>(y), true);
			} else {
				DumpLog->WriteLogLine("P," + boost::lexical_cast<std::string>(j) + "," + boost::lexical_cast<std::string>(x) + "," + boost::lexical_cast<std::string>(y), true);
			}
			
		}
	}
	
	i = 0;
	/* Dont log best path seperately for now
	BOOST_FOREACH( VECTOR_2D MapPoint, PathPlan.PlannedWaypoints ) {
   		DumpLog->WriteLogLine("B," + boost::lexical_cast<std::string>(MapPoint.x) + "," + boost::lexical_cast<std::string>(MapPoint.y), true);
		i++;
	}*/
	

	DumpLog->CloseLog();
	DumpLog->ClearLock();
}

/**
 * Purpose: Saves current baseframe to a text file.
 * Inputs : None.
 * Outputs: None.
 */
void Control::DumpBaseFrame() {

	// Create a new logger, set a lock and empty the file if it exists.
	Logger* DumpLog = new Logger("./ramdisk/baseframe.txt");
	

	DumpLog->WriteLock();
	DumpLog->ClearLog();


	// Dump the baseframe waypoints in lat/long. Re-use Map point datatype...
	int i = 0;
	BOOST_FOREACH( VECTOR_2D MapPoint, PathPlan.BaseFrame ) {
		VECTOR_2D LatLongPoint = XYToLatLong(MapPoint.x, MapPoint.y);
		//VECTOR_2D LatLongPoint = MapPoint;
   		DumpLog->WriteLogLine(boost::lexical_cast<std::string>(i) + "," + boost::lexical_cast<std::string>(LatLongPoint.x) + "," + boost::lexical_cast<std::string>(LatLongPoint.y), true);
		i++;
	}

	DumpLog->CloseLog();
	DumpLog->ClearLock();	
}


void Control::UpdatePathPlan() {
	if (!PathPlan.active) {Log->WriteLogLine("PathPlan - Planning Not Active. Waiting...");}
	while (!PathPlan.active && AutoRun) {boost::this_thread::sleep(boost::posix_time::milliseconds(100));}
	while ((AutoRun && PathPlan.active)) {
		
		PlanLock.lock(); //block until intermediate heading is calculated and sent to PIDs, then lock the mutex
		Log->WriteLogLine("PathPlan - Updating Plan");
		
		NextWaypoint = 0; //reset next waypoint
		double posx = Fuser->CurrentPosition.x;
		double posy = Fuser->CurrentPosition.y;
		
		//Manouver Parameters
		double maxlatoffset = 8;
		double mangran = 1;
		double manlength = 30;
		double mincurverad = 2;
		
		//Get current Obstacles
		double ob[] = {30,70,2.5,18,64,2.5};
		emxArray_real_T *obstacles = emxCreateWrapper_real_T(ob, 3, 1);
		emxArray_real_T *arcob = emxCreate_real_T(1,1);
		
		Log->WriteLogLine("Object Localize");
		//Log->WriteLogLine("Obstacles Size: " + boost::lexical_cast<std::string>(obstacles->size[0]) + "x" + boost::lexical_cast<std::string>(obstacles->size[1]));
		//Log->WriteLogLine("Obstacles Data: " + boost::lexical_cast<std::string>(obstacles->data[0]) + "," + boost::lexical_cast<std::string>(obstacles->data[1]));
		
		
		timestamp_t t0 = get_timestamp();
		oblocalize(PathPlan.scoefx,PathPlan.scoefy,PathPlan.si,obstacles,241,EPSILON,arcob);
		timestamp_t t1 = get_timestamp();
		Log->WriteLogLine("Arcob s = " + boost::lexical_cast<std::string>(arcob->data[0]));
		Log->WriteLogLine("Arcob q = " + boost::lexical_cast<std::string>(arcob->data[1]));
		
		double cpdistance;
		double newcurven;
		double count;
		
		Log->WriteLogLine("Position Localize x = " + boost::lexical_cast<std::string>(posx) + ", y = " + boost::lexical_cast<std::string>(posy));
		Log->WriteLogLine("With curvn = " + boost::lexical_cast<std::string>(PathPlan.curvn));
		timestamp_t t2 = get_timestamp();
		localize(PathPlan.scoefx,PathPlan.scoefy,PathPlan.si,posx,posy,PathPlan.curvn,EPSILON,&PathPlan.scp,&cpdistance,&newcurven,&count);
		timestamp_t t3 = get_timestamp();
		Log->WriteLogLine("scp = " + boost::lexical_cast<std::string>(PathPlan.scp));
		Log->WriteLogLine("cpdistance = " + boost::lexical_cast<std::string>(cpdistance));
		Log->WriteLogLine("New curvn = " + boost::lexical_cast<std::string>(newcurven));
		Log->WriteLogLine("count = " + boost::lexical_cast<std::string>(count));
		PathPlan.curvn = newcurven;
		double sindex;
		double paththeta;
		
		Log->WriteLogLine("Evaluate Heading");
		timestamp_t t4 = get_timestamp();
		evalheading(PathPlan.scp,PathPlan.ss,PathPlan.dxds,PathPlan.dyds,&sindex,&paththeta);
		timestamp_t t5 = get_timestamp();
		Log->WriteLogLine("sindex = " + boost::lexical_cast<std::string>(sindex));
		Log->WriteLogLine("paththeta = " + boost::lexical_cast<std::string>(paththeta/TwoPi * 360));
		
		double algorithmCarHeading; //This is needed because 0 degrees is positive x-axis in the matlab algorithm
		
		if (Fuser->CurrentHeading > 90) {
			algorithmCarHeading = 450 - Fuser->CurrentHeading;
		} else {
			algorithmCarHeading = 90 - Fuser->CurrentHeading;
		}
	
		Log->WriteLogLine("FusionCarHeading = " + boost::lexical_cast<std::string>(Fuser->CurrentHeading));
		Log->WriteLogLine("algorithmCarHeading = " + boost::lexical_cast<std::string>(algorithmCarHeading));
		
		double thetadiff = algorithmCarHeading - (paththeta/TwoPi * 360); //+ve is anticlockwise from path heading
		
		Log->WriteLogLine("Thetadiff = " + boost::lexical_cast<std::string>(thetadiff));
		
		
		emxArray_real_T *pathki = emxCreate_real_T(1,1);
		emxArray_real_T *pathqi = emxCreate_real_T(1,1);
		emxArray_real_T *dthetai = emxCreate_real_T(1,1);
		emxArray_real_T *mans = emxCreate_real_T(1,1);
		
		
		Log->WriteLogLine("Build Manouvers");
		timestamp_t t6 = get_timestamp();
		buildmanouvers(PathPlan.scp, cpdistance, posx, posy,
						maxlatoffset, mangran, manlength, mincurverad, thetadiff,
						PathPlan.ss, sindex, PathPlan.BaseFrameCurvature, paththeta,
						PathPlan.manxi, PathPlan.manyi, pathki, pathqi, dthetai, mans); //Outputs
		timestamp_t t7 = get_timestamp();
						
		Log->WriteLogLine("pathki size = " + boost::lexical_cast<std::string>(pathki->size[0]) + "x" + boost::lexical_cast<std::string>(pathki->size[1]));				
		
		emxArray_boolean_T *pathcollision = emxCreate_boolean_T(1,1);
		
		Log->WriteLogLine("Check Path Collision");
		timestamp_t t8 = get_timestamp();
		checkpathcollision(arcob, pathqi, mans, pathcollision);
		timestamp_t t9 = get_timestamp();
		Log->WriteLogLine("pathcollision size = " + boost::lexical_cast<std::string>(pathcollision->size[0]) + "x" + boost::lexical_cast<std::string>(pathcollision->size[1]));
		
		emxArray_real_T *safetycost = emxCreate_real_T(1,1);
		
		Log->WriteLogLine("Equate safety cost");
		timestamp_t t10 = get_timestamp();
		equatesafetycost(mangran, pathcollision, safetycost);
		timestamp_t t11 = get_timestamp();
		
		emxArray_real_T *offsetcost = emxCreate_real_T(1,1);
		
		Log->WriteLogLine("Equate offset cost");
		timestamp_t t12 = get_timestamp();
		equateoffsetcost(pathqi, offsetcost);
		timestamp_t t13 = get_timestamp();
		
		emxArray_real_T *conscost = emxCreate_real_T(1,1);
		
		
		Log->WriteLogLine("Equate consistency cost");
		timestamp_t t16 = get_timestamp();
		equateconscost(PathPlan.prevpathq, pathqi, mans, conscost);
		timestamp_t t17 = get_timestamp();

		double bestpath;
		emxArray_real_T *costs = emxCreate_real_T(1,1);
		
		Log->WriteLogLine("Calculate Best Path");
		timestamp_t t14 = get_timestamp();
		mincost(5, safetycost, 0.5, offsetcost, 0.08, conscost, costs, &bestpath);
		timestamp_t t15 = get_timestamp();
		
		PathPlan.selectedpath = bestpath;
		Log->WriteLogLine("BestPath = " + boost::lexical_cast<std::string>(bestpath) + " at a cost = " + boost::lexical_cast<std::string>(costs->data[PathPlan.selectedpath-1]));
		
		Log->WriteLogLine("Save Selected Path For Next Plan");
		genprevpathq(PathPlan.selectedpath, pathqi, mans, PathPlan.prevpathq);

		
		int npoints = PathPlan.manxi->size[0];
		PathPlan.PlannedWaypoints.clear();
		for (int i = 0; i < npoints; i++) {
			VECTOR_2D MapPoint;
			MapPoint.x = PathPlan.manxi->data[npoints*(PathPlan.selectedpath-1) + i];
			MapPoint.y = PathPlan.manyi->data[npoints*(PathPlan.selectedpath-1) + i];
			
			PathPlan.PlannedWaypoints.push_back(MapPoint);
		}
		
		
		PlanLock.unlock(); //Unlock the mutex
		
		double secs1 = (t1 - t0) / 1000000.0L;
		double secs2 = (t3 - t2) / 1000000.0L;
		double secs3 = (t5 - t4) / 1000000.0L;
		double secs4 = (t7 - t6) / 1000000.0L;
		double secs5 = (t9 - t8) / 1000000.0L;
		double secs6 = (t11 - t10) / 1000000.0L;
		double secs7 = (t13 - t12) / 1000000.0L;
		double secs8 = (t15 - t14) / 1000000.0L;
		double secs9 = (t17 - t16) / 1000000.0L;
		
		TimeLog->WriteLogLine("----Path Planning Time Stats----");
		TimeLog->WriteLogLine("Time to execute oblozalize: " + boost::lexical_cast<std::string>(secs1) + " s");
		TimeLog->WriteLogLine("Time to execute localize: " + boost::lexical_cast<std::string>(secs2) + " s");
		TimeLog->WriteLogLine("Time to execute evalheading: " + boost::lexical_cast<std::string>(secs3) + " s");
		TimeLog->WriteLogLine("Time to execute buildmanouvers: " + boost::lexical_cast<std::string>(secs4) + " s");
		TimeLog->WriteLogLine("Time to execute checkpathcollision: " + boost::lexical_cast<std::string>(secs5) + " s");
		TimeLog->WriteLogLine("Time to execute equatesafetycost: " + boost::lexical_cast<std::string>(secs6) + " s");
		TimeLog->WriteLogLine("Time to execute equateoffsetcost: " + boost::lexical_cast<std::string>(secs7) + " s");
		TimeLog->WriteLogLine("Time to execute equateconscost: " + boost::lexical_cast<std::string>(secs9) + " s");
		TimeLog->WriteLogLine("Time to execute mincost: " + boost::lexical_cast<std::string>(secs8) + " s");
		
		boost::this_thread::sleep(boost::posix_time::milliseconds(2000)); //Plan path every 2 seconds
	}
}

void Control::ProcessSimMessages() {
	char readbuf[150];

	while(true) {

		bzero(readbuf,sizeof(readbuf));

		fgets(readbuf,150,RXpipeSim); // Block here waiting for a message.
	
		std::string Message = readbuf;
		
		boost::algorithm::trim(Message);

		//Log->WriteLogLine("SIM - Received " + Message);

		std::vector<std::string> MessageParts;

		boost::algorithm::split(MessageParts, Message,  boost::algorithm::is_any_of(","));

		if(MessageParts[0].compare(0,1,"P") == 0) {
			Fuser->CurrentPosition.x = strtod(MessageParts[1].c_str(),NULL);
			Fuser->CurrentPosition.y = strtod(MessageParts[2].c_str(),NULL);
			
			if(ExtLogging) { AutoLogger->WriteLogLine("CX," + boost::lexical_cast<std::string>(TimeStamp()) + "," + boost::lexical_cast<std::string>(Fuser->CurrentPosition.x), true); }
			if(ExtLogging) { AutoLogger->WriteLogLine("CY," + boost::lexical_cast<std::string>(TimeStamp()) + "," + boost::lexical_cast<std::string>(Fuser->CurrentPosition.y), true); }
			
			if(AutoRun) { 
				AutoPosUpdate(Fuser->CurrentPosition); 
			}
		}
		else if(MessageParts[0].compare(0,1,"S") == 0) { 
			Fuser->CurrentSpeed = strtod(MessageParts[1].c_str(),NULL);
			if(AutoRun) {  
				AutoSpeedUpdate(Fuser->CurrentSpeed);
			}
		}
		else if(MessageParts[0].compare(0,1,"B") == 0) { 
			Fuser->CurrentHeading = strtod(MessageParts[1].c_str(),NULL);
			if(ExtLogging) { AutoLogger->WriteLogLine("CB," + boost::lexical_cast<std::string>(TimeStamp()) + "," + boost::lexical_cast<std::string>(Fuser->CurrentHeading), true); }
			if(AutoRun) { AutoTrackUpdate(Fuser->CurrentHeading); }
		}
	}
}

/**
 * Purpose: Starts Autonomous driving.
 * Inputs : None.
 * Outputs: None.
 */
void Control::AutoStart() {

	if(CurrentMap.Waypoints.size() == 0) { Log->WriteLogLine("Control - No map loaded, can't start auto."); return; }

	if(! CarNetworkConnection->HasConnection && BrakeILOn) { Log->WriteLogLine("Control - No base connection, can't start auto."); return; }

	NextWaypoint = 0;
	PathPlan.curvn = 1; //Set path planning curvn to 1
	PathPlan.active = false; //Start with advanced path planning off
	
	ManualOn = false;
	AutoOn = true;
	AutoRun = true;
	AutoSpeedTarget = 0;

	//Initialise the three PID controllers.

	ThrottleController = new PID(10.0,3.0,0,0.1, JunkLogger);
	ThrottleController->setInputLimits(0.0, 30);
	ThrottleController->setOutputLimits(-255,255);
	ThrottleController->setMode(AUTO_MODE);

	BrakeController = new PID(20.0,4.0,0,0.1, JunkLogger);
	BrakeController->setInputLimits(0.0, 30);
	BrakeController->setOutputLimits(-255,255);
	BrakeController->setMode(AUTO_MODE);

	SteerController = new PID(35,0.0,0.0,0.01, JunkLogger);
	SteerController->setInputLimits(-360, 720);
	SteerController->setOutputLimits(-127,127);
	SteerController->setMode(AUTO_MODE);
	SteerController->setBias(0);

	SteerController->setSetPoint(0);
	ThrottleController->setSetPoint(0);
	
	
	boost::thread UpdatePathPlanThread = boost::thread(&Control::UpdatePathPlan, this);
	UpdatePathPlanThread.detach();
	
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
	
	PlanLock.lock(); //block until path planning is complete, lock mutex
	
	
	VECTOR_2D VectorToNextWp;
	
	//Check to see if path planning is active
	if (PathPlan.active) {
		// Check if we have reached a waypoint.
		VECTOR_2D DistanceVector = SubtractVector(PathPlan.PlannedWaypoints[NextWaypoint],CurPosn);
		while(VectorMagnitude(DistanceVector) < MAPPOINT_RADIUS) {
			Log->WriteLogLine("Control - Reached PP waypoint " + boost::lexical_cast<std::string>(NextWaypoint));
			NextWaypoint++;
			if(NextWaypoint >= PathPlan.PlannedWaypoints.size()) { Log->WriteLogLine("Control - Reached end of path plan."); Control::AutoStop(); return; }
			DistanceVector = SubtractVector(PathPlan.PlannedWaypoints[NextWaypoint],CurPosn);
		}

		// Calculate the vector to the next waypoint.
		VectorToNextWp = SubtractVector(PathPlan.PlannedWaypoints[NextWaypoint], CurPosn); // Simple
	} else {
		
		VECTOR_2D DistanceToBaseFrame = SubtractVector(PathPlan.BaseFrame[0],CurPosn);
		if(VectorMagnitude(DistanceToBaseFrame) < MAPPOINT_RADIUS) {
			Log->WriteLogLine("Control - Reached Start of BaseFrame. Activating Advanced Path Planning ");
			PathPlan.active = true;
		}
		
		// Check if we have reached a waypoint.
		VECTOR_2D DistanceVector = SubtractVector(CurrentMap.Waypoints[NextWaypoint],CurPosn);
		if(VectorMagnitude(DistanceVector) < MAPPOINT_RADIUS) {
			Log->WriteLogLine("Control - Reached waypoint " + boost::lexical_cast<std::string>(NextWaypoint));
			NextWaypoint++;
			if(NextWaypoint >= CurrentMap.Waypoints.size()) { Log->WriteLogLine("Control - Reached end of map."); Control::AutoStop(); return; }
		}

		// Calculate the vector to the next waypoint.
		//VECTOR_2D VectorToNextWp = SubtractVector(CurrentMap.Waypoints[NextWaypoint], CurPosn); // Simple
		VectorToNextWp = GetInterpolatedVector(CurPosn);				// Use interpolation
	}
	
	

	// Calculate an angle bearing.
	if(VectorToNextWp.y > 0 && VectorToNextWp.x < 0) { DesiredBearing = 360 + 90 - 360*atan2(VectorToNextWp.y,VectorToNextWp.x)/TwoPi; }
	else { DesiredBearing = 90 - 360*atan2(VectorToNextWp.y,VectorToNextWp.x)/TwoPi; }
	//Log->WriteLogLine("pre flip: " + boost::lexical_cast<std::string>(DesiredBearing));

	
	// Sometimes we need to go out of bounds on the bearing to smooth north crossings and make sure we turn the shortest dirn.
	if(DesiredBearing > (180 + Fuser->CurrentHeading) && Fuser->CurrentHeading < 180) { DesiredBearing = DesiredBearing - 360; } // Turning from NE to NW quadrant.
	if(DesiredBearing < (Fuser->CurrentHeading - 180) && Fuser->CurrentHeading > 180) { DesiredBearing = DesiredBearing + 360; } // Turning from NW to NE quadrant.

	SteerController->setSetPoint(DesiredBearing);

	// Speed profile based on turn radius.
	if(fabs(CurrentSteeringSetPosn) < 50) { DesiredSpeed = 2.5; }
	//else { DesiredSpeed = 2.5 - 0.02*(fabs(CurrentSteeringSetPosn)-50); }
	else { DesiredSpeed = 1.0; }

	ThrottleController->setSetPoint(DesiredSpeed); 
	BrakeController->setSetPoint(DesiredSpeed);
	
	PlanLock.unlock(); //Unlock mutex

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
	//Log->WriteLogLine("BV:" + boost::lexical_cast<std::string>(BrakeValue));
	std::string time;
	
	if(ExtLogging) { 
		time = boost::lexical_cast<std::string>(TimeStamp());
		AutoLogger->WriteLogLine("SI,"  + time + "," + boost::lexical_cast<std::string>(SpeedIncrement), true);
		AutoLogger->WriteLogLine("BV,"  + time + "," + boost::lexical_cast<std::string>(BrakeValue), true);
	}


	if(TripState == 0) { 

		if((CurrentThrottleBrakeSetPosn + SpeedIncrement) > 255) {
			CurrentThrottleBrakeSetPosn = 255;
		}
		else if ((CurrentThrottleBrakeSetPosn + SpeedIncrement) < 0 && BrakeValue < 0) {
			if(BrakeValue > -255) { CurrentThrottleBrakeSetPosn = BrakeValue; }
			else { BrakeValue = -255; }
		}
		else if((CurrentThrottleBrakeSetPosn + SpeedIncrement) > 0) {
			CurrentThrottleBrakeSetPosn = CurrentThrottleBrakeSetPosn + SpeedIncrement; 
		}
		else { CurrentThrottleBrakeSetPosn = 0; }

	}
	else { 
		if(BrakeValue < 0) { CurrentThrottleBrakeSetPosn = BrakeValue; } 
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
	if(CurTrack < 0 && DesiredBearing > 180) { DesiredBearing =  DesiredBearing - 360; SteerController->setSetPoint(DesiredBearing); }
	if(CurTrack > 360 && DesiredBearing < 180) { DesiredBearing =  DesiredBearing + 360; SteerController->setSetPoint(DesiredBearing); }


	SteerController->setProcessValue(CurTrack);
	CurrentSteeringSetPosn = SteerController->compute();
	
	//Log->WriteLogLine("CurTrack," + boost::lexical_cast<std::string>(CurTrack), true);
	//Log->WriteLogLine("DesiredBearing," + boost::lexical_cast<std::string>(DesiredBearing), true);
	
	fprintf(TXpipeSim,"S,%i\n",CurrentSteeringSetPosn);
	fflush(TXpipeSim);

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
 * Purpose: Intelligently compute trajectory.
 * Inputs : Current position vector.
 * Outputs: Desired heading vector.
 */
VECTOR_2D Control::GetInterpolatedVector(VECTOR_2D CurPosn) {

	Spline splinex;
  	Spline spliney;

	double time = 0;

	// Add the current position.
	splinex.addPoint(time, CurPosn.x);
	spliney.addPoint(time, CurPosn.y);

	time = VectorMagnitude(SubtractVector(CurPosn,CurrentMap.Waypoints[NextWaypoint]));

	// Add a few waypoints ahead
	for(int i = 0; (i < 3) && ((NextWaypoint + i) < CurrentMap.Waypoints.size()); i++) {

		splinex.addPoint(time, CurrentMap.Waypoints[NextWaypoint + i].x);
		spliney.addPoint(time, CurrentMap.Waypoints[NextWaypoint + i].y);

		if((NextWaypoint + i + 1) < CurrentMap.Waypoints.size()) {
			time = time + VectorMagnitude(SubtractVector(CurrentMap.Waypoints[NextWaypoint + i + 1],CurrentMap.Waypoints[NextWaypoint + i]));
		}
	
	}

	// Return the derivative for the next timestep at the current location.
	VECTOR_2D DesiredVector;

	DesiredVector.x = splinex(0.1) - splinex(0);
	DesiredVector.y = spliney(0.1) - spliney(0);

	return DesiredVector;
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

	if(VectorMagnitude(SubtractVector(CurPosn,LastRecordedPoint)) > MAPPOINT_RADIUS*2) {
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

	DumpMap("../FrontEnd/Maps/maps/" + MapName + ".wyp");
	
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

VECTOR_2D Control::XYToLatLong(double x, double y) {

	VECTOR_2D MapPoint;

	//Lat
	MapPoint.x = 1*y*360/(EARTH_RADIUS*TwoPi) + DatumLat;
	//Lon
	MapPoint.y = 1*x*360/(EARTH_RADIUS*TwoPi*cos(abs(MapPoint.x))) + DatumLong;

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




