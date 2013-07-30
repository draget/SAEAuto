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

#include <boost/lexical_cast.hpp>

#include "Control.h"
#include "CarNetwork.h"
#include "Logger.h"
#include "SafetySerialOut.h"
#include "LowLevelSerialOut.h"
#include "GPSConnection.h"
#include "IBEO.h"

Control *SAECar;


Control::Control(std::string LogDir) {

	this->LogDir = LogDir;

	mode_t process_mask = umask(0);
	mkdir(this->LogDir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	umask(process_mask);
	
	HeartbeatState = false;
	TripState = 0;
	ManualOn = false;

	CurrentSteeringSetPosn = 0;
	CurrentThrottleBrakeSetPosn = 0;

	Log = new Logger(LogDir + "/mainlog.txt");

	CarNetworkConnection = new CarNetwork(this, Log);

	SafetySerial = new SafetySerialOut(this,Log);

	LowLevelSerial = new LowLevelSerialOut(this,Log);

	GPS = new GPSConnection(this,Log);

	Lux = new IBEO(this,Log);

}

Control::Control(const Control& orig) {
}

Control::~Control() {
}


void Control::Setup() {

	CarNetworkConnection->Open();
//	SafetySerial->Open();
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

	CarNetworkConnection->StartProcessMessages();
	SafetySerial->Start();
	LowLevelSerial->Start();
	GPS->Start();
	Lux->Start();

	while(RunState) {

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

		boost::this_thread::sleep(boost::posix_time::milliseconds(100));

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

void Control::SendAlarm() {

	if(SafetySerial->SerialState) { SafetySerial->Send('A'); }

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
	else { LogDir = "./RunFiles/0"; }

	initscr();
	noecho();
	nodelay(stdscr, 1);
	keypad(stdscr, 1);

	SAECar = new Control(LogDir);

	std::signal(SIGINT, HandleExit);

	SAECar->Setup();

	SAECar->Run();

	echo();
	endwin();


	return 0;

}




