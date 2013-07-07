


#include <iostream>
#include <curses.h>
#include <signal.h>
#include <string>

#include "Control.h"
#include "CarNetwork.h"
#include "Logger.h"




Control::Control() {

HeartbeatState = false;
Trip = false;
ManualOn = false;

CurrentSteeringPosn = 0;
CurrentThrottleBrake = 0;

Log = new Logger("log.txt");

CarNetworkConnection = new CarNetwork(this, Log);


}

Control::Control(const Control& orig) {
}

Control::~Control() {
}


void Control::Setup() {

CarNetworkConnection->Open();

}

void Control::Quit(int param) {

echo();
endwin();
exit(1);

}

void Control::Run() {

	CarNetworkConnection->StartProcessMessages();

	while(1) {

		mvprintw(0,0,"Trip State: %i \n", this->Trip);
		mvprintw(1,0,"HB State: %i \n", this->HeartbeatState);
		mvprintw(2,0,"Car Net State: %s \n", this->CarNetworkConnection->StatusString.c_str());

		mvprintw(4,0,"Current Steering Posn: %i \n", this->CurrentSteeringPosn);
		mvprintw(5,0,"Current Throttle/Brake Level: %i \n", this->CurrentThrottleBrake);

		int y,x;
		getmaxyx(stdscr,y,x);

		if(y > 10) {

			std::string RecentLogLines[y - 10];
			this->Log->GetLogLines(RecentLogLines, y - 10);

			for(int i = y; i > 10; i--) {

				mvprintw(i-1,0,"Log: %s", (RecentLogLines[y - i] + '\n').c_str());

			}

		}

		refresh();

		boost::this_thread::sleep(boost::posix_time::milliseconds(100));

	}

}

int main() {

initscr();
noecho();
nodelay(stdscr, 1);
keypad(stdscr, 1);

Control *SAECar = new Control();

signal(SIGINT, Control::Quit);

SAECar->Setup();

SAECar->Run();

return 0;

}


