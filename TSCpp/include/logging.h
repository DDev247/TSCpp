
#pragma once

#include <list>
#include <fstream>

class DLog {
public:
	DLog();

	void Add(std::string log);
	void Dump(int amount = 1);
	void DumpAll();

	inline std::list<std::string> GetLogs() { return logs; }

private:
	std::list<std::string> logs;
	std::ofstream logFile;

	void AddFront(std::string log);
	std::string Prepare(std::string log);
	void front(std::string log);
	void back(std::string log);
};
