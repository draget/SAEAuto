
#ifndef _Logger_H
#define	_Logger_H

#include <string>
#include <iostream>
#include <fstream>


class Logger {
public:

	Logger(std::string LogFile);
    	Logger(const Logger& orig);
    	virtual ~Logger();

	void GetLogLines(std::string outputbuffer[], int NoLines);
	void WriteLogLine(std::string LogLine);
	void CloseLog();

private:

	std::string LogFile;
	std::ofstream LogFileStream;

};

#endif	/* _Logger_H */

