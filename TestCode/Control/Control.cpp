/*
* Autonomous SAE Car Controller
* (C) Thomas Drage 2013
*
* Created June 2013
*/


#include <iostream>
#include <curses.h>
#include <csignal>
#include <string>
#include <fstream>
#include <vector>

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

#include "PID.h"

Control *SAECar;

double TwoPi = 4*acos(0);

Control::Control(std::string LogDir) {

	this->LogDir = LogDir;

	mode_t process_mask = umask(0);
	mkdir(this->LogDir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	umask(process_mask);
	
	HeartbeatState = false;
	TripState = 0;
	ManualOn = false;
	AutoOn = false;

	DatumLat = -31.980569;
	DatumLong = 115.817807;

	CurrentSteeringSetPosn = 0;
	CurrentThrottleBrakeSetPosn = 0;

	Log = new Logger(LogDir + "/mainlog.txt");

	CarNetworkConnection = new CarNetwork(this, Log);

	SafetySerial = new SafetySerialOut(this,Log);

	LowLevelSerial = new LowLevelSerialOut(this,Log);

	GPS = new GPSConnection(this,Log);

	Lux = new IBEO(this,Log);

	WebIPC = new IPC(this,Log);

	WebLogger = new Logger("./weblog.txt");

}

Control::Control(const Control& orig) {
}

Control::~Control() {
}


void Control::Setup() {

	WebIPC->Open();
	CarNetworkConnection->Open();
	//SafetySerial->Open();
	LowLevelSerial->Open();
	GPS->Open();
	if(access("noibeo", F_OK ) == -1) { Lux->Open(); }

}

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

void Control::Run() {

	RunState = true;

	WebIPC->Start();
	CarNetworkConnection->StartProcessMessages();
	//SafetySerial->Start();
	LowLevelSerial->Start();
	GPS->Start();
	Lux->Start();

	while(RunState) {

		UpdateTerminal();
		WriteInfoFile();

		boost::this_thread::sleep(boost::posix_time::milliseconds(100));

	}

}

void Control::UpdateTerminal() {

	mvprintw(1,0,"Manual State: %i \n", this->ManualOn);
	mvprintw(2,0,"Trip State: %i \n", this->TripState);
	mvprintw(3,0,"HB State: %i \n", this->HeartbeatState);
	mvprintw(4,0,"Car Net State: %s \n", this->CarNetworkConnection->StatusString.c_str());
	mvprintw(5,0,"Safety Serial State: %i \n", this->SafetySerial->SerialState);

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


void Control::WriteInfoFile() {

	WebLogger->ClearLog();

	WebLogger->WriteLogLine("Manual State|" + boost::lexical_cast<std::string>(this->ManualOn), true);
	WebLogger->WriteLogLine("Trip State|" + boost::lexical_cast<std::string>(this->TripState), true);
	WebLogger->WriteLogLine("HB State|" + boost::lexical_cast<std::string>(this->HeartbeatState), true);
	WebLogger->WriteLogLine("Car Net State|" + boost::lexical_cast<std::string>(this->CarNetworkConnection->StatusString.c_str()), true);
	WebLogger->WriteLogLine("Safety Serial State|" + boost::lexical_cast<std::string>(this->SafetySerial->SerialState), true);

	WebLogger->WriteLogLine("Brake IL Status|" + boost::lexical_cast<std::string>(this->BrakeILOn), true);

	WebLogger->WriteLogLine("Current Steering Posn|" + boost::lexical_cast<std::string>(this->CurrentSteeringSetPosn), true);
	WebLogger->WriteLogLine("Current Throttle/Brake Level|" + boost::lexical_cast<std::string>(this->CurrentThrottleBrakeSetPosn), true);

	WebLogger->WriteLogLine("GPS State|" + boost::lexical_cast<std::string>(this->GPS->GPSState), true);
	WebLogger->WriteLogLine("GPS Latitude|" + boost::lexical_cast<std::string>(this->GPS->Latitude), true);
	WebLogger->WriteLogLine("GPS Longitude|" + boost::lexical_cast<std::string>(this->GPS->Longitude), true);
	WebLogger->WriteLogLine("GPS Speed|" + boost::lexical_cast<std::string>(this->GPS->Speed), true);
	WebLogger->WriteLogLine("GPS Track Angle|" + boost::lexical_cast<std::string>(this->GPS->TrackAngle), true);

	WebLogger->WriteLogLine("Datum Lat|" + boost::lexical_cast<std::string>(this->DatumLat), true);
	WebLogger->WriteLogLine("Datum Long|" + boost::lexical_cast<std::string>(this->DatumLong), true);



	WebLogger->WriteLogLine("IBEO State|" + boost::lexical_cast<std::string>(this->Lux->inUse), true);
	WebLogger->WriteLogLine("IBEO N Objects|" + boost::lexical_cast<std::string>(this->Lux->object_data_header[this->Lux->curObjectDataSource].number_of_objects), true);
	WebLogger->WriteLogLine("IBEO N Scan Pts|" + boost::lexical_cast<std::string>(this->Lux->scan_data_header[this->Lux->curScanDataSource].scan_points), true);
	WebLogger->WriteLogLine("IBEO Scan No|" + boost::lexical_cast<std::string>(this->Lux->scan_data_header[this->Lux->curScanDataSource].scan_number), true);

	int y = 20;
	
	std::string RecentLogLines[y];
	this->Log->GetLogLines(RecentLogLines, y);
			
	for(int i = y; i > 0; i--) {

		WebLogger->WriteLogLine("Log|" + RecentLogLines[y - i], true);

	}

}



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

	if(BrakeILOn) { CurrentThrottleBrakeSetPosn = -256; }
	else { CurrentThrottleBrakeSetPosn = 0; }

	Log->WriteLogLine("Control - changed to trip state " + boost::lexical_cast<std::string>(TripState) + " reason: " + TripReason);

}

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

void Control::LoadMap(std::string MapFilename) {

	ClearMap();

	std::ifstream infile(("../../FrontEnd/Maps/maps/" + MapFilename).c_str());

	if (infile.is_open()) {
		while ( infile.good() ) {
			std::string line;
      			getline (infile,line);

			boost::algorithm::trim(line);

			if(line.empty()) { continue; }
      			
			std::vector<std::string> LineParts;
			boost::algorithm::split(LineParts, line,  boost::algorithm::is_any_of(","));

			MAPPOINT_2D MapPoint;

			MapPoint = LatLongToXY(boost::lexical_cast<double>(LineParts[1]), boost::lexical_cast<double>(LineParts[2]));

			if(LineParts[0].compare(0,1,"F") == 0 ) {
				Log->WriteLogLine("Adding fence.");
				CurrentMap.Fenceposts.push_back(MapPoint);
			}
			else {
				Log->WriteLogLine("Adding wp.");
				CurrentMap.Waypoints.push_back(MapPoint);
			}

    		}
    		infile.close();
  	}
	else { Log->WriteLogLine("Couldn't open map"); }
	


}

void Control::ClearMap() {

	NextWaypoint = 0;

	CurrentMap.Fenceposts.clear();
	CurrentMap.Waypoints.clear();

}

void Control::DumpMap() {

	BOOST_FOREACH( MAPPOINT_2D MapPoint, CurrentMap.Waypoints ) {
   		Log->WriteLogLine("WP " + boost::lexical_cast<std::string>(MapPoint.x) + " " + boost::lexical_cast<std::string>(MapPoint.y));
	}

	BOOST_FOREACH( MAPPOINT_2D MapPoint, CurrentMap.Fenceposts ) {
   		Log->WriteLogLine("F " + boost::lexical_cast<std::string>(MapPoint.x) + " " + boost::lexical_cast<std::string>(MapPoint.y));
	}
	

}

void Control::AutoStart() {

	if(CurrentMap.Waypoints.size() == 0) { Log->WriteLogLine("Control - No map loaded, can't start auto."); }

	NextWaypoint = 0;
	ManualOn = false;
	AutoOn = true;
	AutoRun = true;
	AutoSpeedTarget = 0;

	SpeedController = new PID(10.0,10.0,0,0.2);
	SpeedController->setInputLimits(0.0, 30);
	SpeedController->setOutputLimits(-255,255);
	SpeedController->setMode(AUTO_MODE);

	SteerController = new PID(2.0,1.0,0,0.2);
	SteerController->setInputLimits(-360, 360);
	SteerController->setOutputLimits(-127,127);
	SteerController->setMode(AUTO_MODE);

}

void Control::AutoPosUpdate(MAPPOINT_2D CurPosn) {

	MAPPOINT_2D VectorToNextWp = SubtractMapPoint(CurrentMap.Waypoints[NextWaypoint], CurPosn);

	if(VectorToNextWp.x > 0) { DesiredBearing = 90 - 360*atan(VectorToNextWp.y/VectorToNextWp.x)/TwoPi; }
	else { DesiredBearing = 270 - 360*atan(VectorToNextWp.y/VectorToNextWp.x)/TwoPi; }

}

void Control::AutoSpeedUpdate(double CurSpeed) {

	SpeedController->setProcessValue(CurSpeed);
	CurrentThrottleBrakeSetPosn = SpeedController->compute();

}


void Control::AutoTrackUpdate(double CurTrack) {

	SteerController->setProcessValue(CurTrack);
	CurrentSteeringSetPosn = SteerController->compute();

}

void Control::AutoPause() {
	
	AutoRun = false;
	boost::thread brake_Thread = boost::thread(&Control::TimedBrake, this);
	brake_Thread.detach();

}

void Control::AutoContinue() {
	
	AutoRun = true;

}

void Control::TimedBrake() {

	CurrentThrottleBrakeSetPosn = -255;
	boost::this_thread::sleep(boost::posix_time::milliseconds(1500));
	CurrentThrottleBrakeSetPosn = 0;

}

void Control::SendAlarm() {

	if(SafetySerial->SerialState) { SafetySerial->Send('A'); }

}

double Control::TimeStamp() {

	timeval current;
	gettimeofday(&current,NULL);

	return current.tv_sec + (double)current.tv_usec/1000000;

}

MAPPOINT_2D Control::LatLongToXY(double lat, double lng) {

	MAPPOINT_2D MapPoint;

	MapPoint.x = EARTH_RADIUS*TwoPi*(DatumLat - lat)/360;
	MapPoint.y = EARTH_RADIUS*cos(abs(lat))*TwoPi*(DatumLong - lng)/360;

	return MapPoint;

}

MAPPOINT_2D Control::SubtractMapPoint(MAPPOINT_2D Point1, MAPPOINT_2D Point2) {

	MAPPOINT_2D Result;

	Result.x = Point1.x - Point2.x;
	Result.y = Point1.y - Point2.y;

	return Result;

}

// C-style non -member functions follow.

void HandleExit(int param) {

	SAECar->Quit();

}


int main(int argc, char *argv[]) {

	std::string LogDir;

	if(argc > 1) {
		LogDir = "./RunFiles/" + boost::lexical_cast<std::string>(argv[1]);
	}
	else { system("rm -rf ./RunFiles/0"); LogDir = "./RunFiles/0"; }

	initscr();
	noecho();
	nodelay(stdscr, 1);
	keypad(stdscr, 1);

	SAECar = new Control(LogDir);

	std::signal(SIGINT, HandleExit);

	SAECar->Setup();

	SAECar->Run();

	cout << "Close.\n";

	echo();
	endwin();


	return 0;

}




