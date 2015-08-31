
#ifndef _Logger_H
#define	_Logger_H

#include <string>
#include <iostream>
#include <fstream>
#define DEFAULTBUFFER 10

class Logger {
public:
	Logger(std::string, int);
	Logger(std::string LogFile);
    	Logger(const Logger& orig);
    	virtual ~Logger();

	void logInit(std::string, int);
	void GetLogLines(std::string outputbuffer[], int NoLines);
	void WriteLogLine(std::string LogLine, bool NoTime);
	void WriteLogLine(std::string LogLine);
	void CloseLog();
	void ClearLog();
	void WriteLock();
	void ClearLock();

private:

	std::string LogFile;
	std::ofstream LogFileStream;
	int linecount;
	int linesToBuffer;
};

#endif	/* _Logger_H */

