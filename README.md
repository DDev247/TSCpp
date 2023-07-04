# TSCpp
C++ library for recieving TunerStudio MS information via Serial.

## Disclaimer
This library is in developement, and the current API is very unfinished and is subject to change.

## What is this?
This is a library which "emulates" a [Speeduino](https://speeduino.com/) ECU which can connect to TunerStudio MS for data display and tuning.
Note that the "emulation" of the ECU is very crude and is mostly there for communicating with TunerStudio and not accurately emulating the operations of the Speeduino.

## How do I use it?
This project uses CMake for the build system and can easily be integrated into a new/existing CMake project.
Just add the project using `add_subdirectory` and make your executable link the TSCpp library to it.
Example CMakeLists file:
```cmake
cmake_minimum_required (VERSION 3.8)

project ("TSC++ Example")

# Add TSCpp library
add_subdirectory ("TSCpp")

# Add project executable
add_executable (TSCppExample
	"main.cpp"
)

# Link TSCpp to the executable
target_link_libraries (TSCppExample TSCpp)
```
Example C++ code:
```c++
#include "TSCpp/include/tscpp.h"

int main() {
	// Init TSCpp
	TSCpp tscpp("COM3", 115200);

	// Wait for TSCpp thread to finish (never)
	tscpp.WaitForThread();

	return 0;
}
```

## Building the project

1. Clone the project source
```bash
git clone --recurse-submodules https://github.com/DDev247/TSCpp.git
```

2. Create build directory
```bash
mkdir out
```

3. Create CMake cache and build
```bash
cmake ..
cmake --build .
```
