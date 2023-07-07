
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
            currentStatus.hasSync = false;
            continue;
            //return 1;
        }

        if (!ConnectNamedPipe(hPipe, NULL))
        {
            std::cout << "Failed to connect named pipe. Error code: " << GetLastError() << std::endl;
            CloseHandle(hPipe);

            // If pipe failed continue loop but notify TS (happens often because of desync or other issues)
            currentStatus.hasSync = false;
            continue;
            //return 1;
        }

        char buffer[4096];
        DWORD bytesRead;
        BOOL success = ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL);
        if (success && bytesRead > 0)
        {
            currentStatus.hasSync = true;

            std::string line(buffer, bytesRead);

            json j = json::parse(line);

            long prevRPM = currentStatus.longRPM;
            currentStatus.rpmDOT = j["RPM"] - prevRPM;
            currentStatus.longRPM = j["RPM"];
            currentStatus.RPM = currentStatus.longRPM;
            currentStatus.RPMdiv100 = currentStatus.longRPM / 100;
            currentStatus.TPSlast = currentStatus.TPS;
            currentStatus.TPS = std::round(200.0 * j["tps"]);
            currentStatus.tpsADC = std::round(255.0 * j["tps"]);
            currentStatus.MAP = (j["manifoldPressure"] * 100) / 102000;
            currentStatus.battery10 = 144;
            currentStatus.advance = j["sparkAdvance"];
            currentStatus.advance1 = j["sparkAdvance"];
            currentStatus.advance2 = j["sparkAdvance"];
            currentStatus.gear = j["gear"];
            currentStatus.vss = j["vehicleSpeed"] * 1.609344;
            currentStatus.coolant = 90;
            currentStatus.IAT = 27;
            currentStatus.fuelPumpOn = true;

            // other
            currentStatus.CTPSActive = 0;
            currentStatus.EMAP = 0;
            currentStatus.EMAPADC = 0;
            currentStatus.baro = 100;
            currentStatus.afrTarget = 147;
            currentStatus.nitrous_status = 0;
            currentStatus.nSquirts = 4;
            currentStatus.nChannels = j["cylinderCount"];
        }
        else
        {
            // If pipe failed continue loop but notify TS (happens often because of desync or other issues)
            std::cout << "Failed to read from the pipe. Error code: " << GetLastError() << std::endl;
            currentStatus.hasSync = false;
        }

        CloseHandle(hPipe);
    }

    tscpp.WaitForThreads();

    tscpp.~TSCpp();
	return 0;
}
