
#include "../include/logging.h"
#include <iostream>
#include <sstream>

DLog::DLog() {
	logs = std::list<std::string>();
	logFile = std::ofstream("tscpp/log.txt");
}

std::string DLog::Prepare(std::string log) {
	time_t currentTime = time(nullptr);
	std::string currentTimeString = ctime(&currentTime);
	currentTimeString.erase(currentTimeString.find_last_not_of("\n") + 1);
	std::stringstream ss;
	ss << "[" << currentTimeString << "] -> " << log;
	
	std::string logString = ss.str();
//#ifdef _DEBUG
	std::cout << ss.str() << std::endl;
//#endif // _DEBUG

	return ss.str();
}

void DLog::Add(std::string log) {
	log = Prepare(log);
	logs.push_back(log);
}

void DLog::AddFront(std::string log) {
	log = Prepare(log);
	logs.push_front(log);
}

void DLog::Dump(int amount) {
	std::stringstream ss;
	ss << "Dumping " << amount << " logs to file.";
	AddFront(ss.str());

	// pop 1 more (the internal "dumping n logs to file" message)
	for (int i = 0; i < amount+1; i++) {
		logFile << logs.front() << std::endl;
		logs.pop_front();
	}
	logFile.flush();
}

void DLog::DumpAll() {
	AddFront("Dumping all logs to file.");

	for (int i = 0; i < logs.size(); i++) {
		logFile << logs.front() << std::endl;
		logs.pop_front();
	}
	logFile.flush();
}
