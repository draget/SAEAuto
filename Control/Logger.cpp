/* 
 * File:   Logger.h
 * Author: T. Drage
 *
 * Logger class...
 *
 * Created on 5/7/13
 */


#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <algorithm>
#include <stdio.h>

#include "Logger.h"




/**
 * Purpose: Creates a new instance of the Logger object.
 * Inputs : None.
 * Outputs: None.
 */
Logger::Logger(std::string LogFileArg) {
 
	LogFile = LogFileArg;

	LogFileStream.open(LogFile.c_str());
	
	this->WriteLogLine("Log started.");
	
	linecount = 0;

}

/**
 * Purpose: Creates a new instance of the Logger object.
 * Inputs : An Logger object.
 * Outputs: None.
 */
Logger::Logger(const Logger& orig) {
}

/**
 * Purpose: Destroys the instance of the Logger object.
 * Inputs : None.
 * Outputs: None.
 */
Logger::~Logger() {
}


void Logger::WriteLogLine(std::string LogLine) { WriteLogLine(LogLine, false, false); }
void Logger::WriteLogLine(std::string Logline, bool NoTime) { WriteLogLine(LogLine, NoTime, false); } 
void Logger::WriteLogLine(std::string LogLine, bool NoTime, bool Main) {

	if(! NoTime) {	

		std::time_t t = std::time(NULL);
		char timestr[50];

		std::strftime(timestr, 50, "%d/%m/%y %T", std::localtime(&t));

		std::string TimeString = timestr;

		LogFileStream << TimeString + ": " + LogLine + '\n';
	}
	else {
		LogFileStream << LogLine + '\n';
	}
	if (linecount == 10 || Main){
		LogFileStream.flush();
		linecount = 0;
	}
	else {
		linecount++;
	}
}

void Logger::ClearLog() {

	LogFileStream.close();
	LogFileStream.open(LogFile.c_str(), std::ios::trunc);

}

void Logger::WriteLock() {

	std::ofstream LockFileStream;
	LockFileStream.open((LogFile + "_LOCK").c_str());
	LockFileStream.close();
}

void Logger::ClearLock() {
	
	remove((LogFile + "_LOCK").c_str());

}

void Logger::GetLogLines(std::string outputbuffer[], int NoLines) {

	std::string strLine;
	char buf;

	std::ifstream infile(LogFile.c_str());
 
	infile.seekg(0, std::ios::beg);
	std::ifstream::pos_type posBeg = infile.tellg();

	infile.seekg(-1, std::ios::end);
	std::ifstream::pos_type posEnd = infile.tellg();

	int i = NoLines; 

	while(i > 0) {

		if((infile.tellg() >= posBeg)) {    	
	
			buf = static_cast <char>(infile.peek());
 
		//	cout << infile.tellg() << '\n';

    			if ((buf != '\n' || infile.tellg() == posEnd) && infile.tellg() > posBeg) {
      	  			strLine += buf;
			}
    			else {
        			
				if(infile.tellg() == posBeg) { strLine += buf; }				

				std::reverse(strLine.begin(), strLine.end());

				outputbuffer[NoLines - i] = strLine;
	
				i--;

        			strLine.clear();
    			}
 
  	  		infile.seekg(-1, std::ios::cur);
		}
		else { outputbuffer[NoLines - i] = ""; i--; }

		

	}
 

	infile.close();
 
}	

void Logger::CloseLog() {

	LogFileStream.close();

}



