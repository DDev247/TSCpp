
#include "main.h"
#include "../../submodules/Serialib/lib/serialib.h"
#include <thread>
#include <chrono>

TSCpp* TSCpp::instance = nullptr;

TSCpp::TSCpp(std::string port, int baud) : EEPROM() {
	// Check if tscpp directory exists and create if it doesn't.
	int i = 0;
	if (!CreateDirectory("tscpp", NULL) && (i = GetLastError()) != ERROR_ALREADY_EXISTS) {
		std::cerr << "Failed to create tscpp directory error " << i << std::endl;
		exit(1);
	}
	
	instance = this;

	// Init debug log
	Log = DLog();

	//pages.InitPages();

	Log.Add("Starting TSC++...");
	Serial = serialib();
	Log.Add("Opening serial port...");
	if (Serial.openDevice(port.c_str(), baud) != 1) {
		Log.Add("ERROR: FAILED TO OPEN SERIAL PORT");
		//Log.Dump(3);
		exit(1);
	}
	Log.Add("Serial port opened and ready.");

	Log.Add("Loading config...");
	loadConfig();
	Log.Add("Config loaded.");

	Log.Add("Starting main recv loop...");
	recvThread = std::thread(&TSCpp::threadFn, this);

	Log.Add("Starting timer loop...");
	timerThread = std::thread(&TSCpp::timerFn, this);
	//Log.DumpAll();
}

TSCpp::~TSCpp() {
	Log.DumpAll();
}

void TSCpp::WaitForThreads() {
	recvThread.join();
	timerThread.join();
	Log.DumpAll();
}

void TSCpp::StopThreads() {
	Log.Add("Stopping TSC++");
	quit = true;
	Log.Add("Waiting for threads to finish...");
	WaitForThreads();
}

void TSCpp::timerFn() {
	using namespace std::chrono_literals;

	while (true) {
		if (quit)
			break;

		std::this_thread::sleep_for(1000ms);

		currentStatus.loopsPerSecond = loops;
		loops = 0;
		EEPROM.writeFile();
		//Log.DumpAll();
		//EEPROM.writeCacheToFlash();
	}

	Log.Add("Stopping timer loop...");
}

unsigned int makeWord(unsigned char h, unsigned char l) { return (h << 8) | l; }
