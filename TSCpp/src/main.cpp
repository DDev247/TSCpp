
#include "main.h"
#include "../../submodules/Serialib/lib/serialib.h"

TSCpp::TSCpp(std::string port, int baud) {
	// Init debug log
	Log = DLog();

	pages.InitPages();

	Log.Add("Starting TSC++...");
	Serial = serialib();
	Log.Add("Opening serial port...");
	if (Serial.openDevice(port.c_str(), baud) != 1) {
		Log.Add("ERROR: FAILED TO OPEN SERIAL PORT");
		Log.Dump(3);
		exit(1);
	}
	Log.Add("Serial port opened and ready.");

	// dump current logs because they are important
	Log.Dump(3);

	// initiate modules
	Log.Add("Initiating modules...");
	
	pages = Pages();

	Log.Add("Modules Initiated.");
	Log.Add("Starting main recv loop...");
	Log.Dump(3);
	recvThread = std::thread(&TSCpp::threadFn, this);
}

TSCpp::~TSCpp() {
	Log.DumpAll();
}

void TSCpp::WaitForThread() {
	recvThread.join();
}

unsigned int makeWord(unsigned char h, unsigned char l) { return (h << 8) | l; }
