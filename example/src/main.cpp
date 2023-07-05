
#include "../include/main.h"

int main() {
	TSCpp tscpp = TSCpp("COM3", 115200);

	tscpp.WaitForThreads();

	//tscpp.~TSCpp();
	return 0;
}
