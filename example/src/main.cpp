
#include "../include/main.h"

// enable TSCpp logging in this project
#define TSCPP_LOGGING_ENABLED true

int main() {
	TSCpp tscpp = TSCpp("COM3", 115200);

	tscpp.WaitForThread();

	//tscpp.~TSCpp();
	return 0;
}
