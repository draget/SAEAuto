

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <curses.h>
#include <signal.h>

#include "Control.h"
#include "CarNetwork.h"



using namespace std;



Control::Control() {

HeartbeatState = false;
ManualOn = false;

CarNetworkConnection = new CarNetwork(&HeartbeatState);

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
printw("1!\n");
CarNetworkConnection->StartProcessMessages();

while(1) {

mvprintw(0,0,"State: %i \n", this->HeartbeatState);

scr_dump("./scrdump.txt");

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


