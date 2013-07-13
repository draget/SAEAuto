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

Control *SAECar;


Control::Control() {

	HeartbeatState = false;
	TripState = 0;
	ManualOn = false;

	CurrentSteeringPosn = 0;
	CurrentThrottleBrake = 0;

	Log = new Logger("log.txt");

	CarNetworkConnection = new CarNetwork(this, Log);

	SafetySerial = new SafetySerialOut(this,Log);

}

Control::Control(const Control& orig) {
}

Control::~Control() {
}


void Control::Setup() {

	CarNetworkConnection->Open();
	SafetySerial->Open();

}

void Control::Quit() {

	CarNetworkConnection->Disconnect();

	SafetySerial->Stop();

	RunState = false;

}

void Control::Run() {

	RunState = true;

	CarNetworkConnection->StartProcessMessages();
	SafetySerial->Start();

	while(RunState) {

		mvprintw(1,0,"Manual State: %i \n", this->ManualOn);
		mvprintw(2,0,"Trip State: %i \n", this->TripState);
		mvprintw(3,0,"HB State: %i \n", this->HeartbeatState);
		mvprintw(4,0,"Car Net State: %s \n", this->CarNetworkConnection->StatusString.c_str());
		mvprintw(5,0,"Safety Serial State: %i \n", this->SafetySerial->SerialState);

		mvprintw(7,0,"Brake IL Status: %i \n", this->BrakeILOn);

		mvprintw(9,0,"Current Steering Posn: %i \n", this->CurrentSteeringPosn);
		mvprintw(10,0,"Current Throttle/Brake Level: %i \n", this->CurrentThrottleBrake);

		int y,x;
		getmaxyx(stdscr,y,x);

		int TitleX = 0;

		if((x/2 - 28) > 0) { TitleX = x/2 - 28; }

		mvprintw(0,TitleX,"UWA AUTONOMOUS SAE CAR CONTROLLER - THOMAS DRAGE 2013\n");

		int LogStartLine = 12;

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

// C-style non -member functions follow.

void HandleExit(int param) {

	SAECar->Quit();

}


int main() {

	initscr();
	noecho();
	nodelay(stdscr, 1);
	keypad(stdscr, 1);

	SAECar = new Control();

	std::signal(SIGINT, HandleExit);

	SAECar->Setup();

	SAECar->Run();

	echo();
	endwin();


	return 0;

}




