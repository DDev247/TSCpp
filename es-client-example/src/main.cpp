
#include "../include/main.h"
#include <nlohmann/json.hpp>
#include <windows.h>
using json = nlohmann::json;

int main() {
	TSCpp tscpp("COM3", 115200);

	while (true) {
        // Connect to ES-Client pipe...
        HANDLE hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\est-input-pipe"),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            4096,
            4096,
            0,
            NULL);

        if (hPipe == INVALID_HANDLE_VALUE)
        {
            std::cout << "Failed to create named pipe. Error code: " << GetLastError() << std::endl;
            CloseHandle(hPipe);
            
            // If pipe failed continue loop but notify TS (happens often because of desync or other issues)
            tscpp.currentStatus.hasSync = false;
            continue;
            //return 1;
        }

        if (!ConnectNamedPipe(hPipe, NULL))
        {
            std::cout << "Failed to connect named pipe. Error code: " << GetLastError() << std::endl;
            CloseHandle(hPipe);

            // If pipe failed continue loop but notify TS (happens often because of desync or other issues)
            tscpp.currentStatus.hasSync = false;
            continue;
            //return 1;
        }

        char buffer[4096];
        DWORD bytesRead;
        BOOL success = ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL);
        if (success && bytesRead > 0)
        {
            tscpp.currentStatus.hasSync = true;

            std::string line(buffer, bytesRead);

            json j = json::parse(line);

            long prevRPM = tscpp.currentStatus.longRPM;
            tscpp.currentStatus.rpmDOT = j["RPM"] - prevRPM;
            tscpp.currentStatus.longRPM = j["RPM"];
            tscpp.currentStatus.RPM = tscpp.currentStatus.longRPM;
            tscpp.currentStatus.RPMdiv100 = tscpp.currentStatus.longRPM / 100;
            tscpp.currentStatus.TPSlast = tscpp.currentStatus.TPS;
            tscpp.currentStatus.TPS = std::round(200.0 * j["tps"]);
            tscpp.currentStatus.tpsADC = std::round(255.0 * j["tps"]);
            tscpp.currentStatus.MAP = (j["manifoldPressure"] * 100) / 102000;
            tscpp.currentStatus.battery10 = 144;
            tscpp.currentStatus.advance = j["sparkAdvance"];
            tscpp.currentStatus.advance1 = j["sparkAdvance"];
            tscpp.currentStatus.advance2 = j["sparkAdvance"];
            tscpp.currentStatus.gear = j["gear"];
            tscpp.currentStatus.vss = j["vehicleSpeed"] * 1.609344;
            tscpp.currentStatus.coolant = 90;
            tscpp.currentStatus.IAT = 27;
            tscpp.currentStatus.fuelPumpOn = true;

            // other
            tscpp.currentStatus.CTPSActive = 0;
            tscpp.currentStatus.EMAP = 0;
            tscpp.currentStatus.EMAPADC = 0;
            tscpp.currentStatus.baro = 100;
            tscpp.currentStatus.afrTarget = 147;
            tscpp.currentStatus.nitrous_status = 0;
            tscpp.currentStatus.nSquirts = 4;
            tscpp.currentStatus.nChannels = j["cylinderCount"];
        }
        else
        {
            // If pipe failed continue loop but notify TS (happens often because of desync or other issues)
            std::cout << "Failed to read from the pipe. Error code: " << GetLastError() << std::endl;
            tscpp.currentStatus.hasSync = false;
        }

        CloseHandle(hPipe);
    }

    tscpp.WaitForThreads();

    tscpp.~TSCpp();
	return 0;
}
