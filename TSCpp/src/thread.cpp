
#include "main.h"
#include <string>

void TSCpp::threadFn() {
	while (true) {
		if (Serial.available() > 0) {
            /*if (currentStatus.RPM >= 6000) {
                currentStatus.longRPM = 0;
                currentStatus.RPM = currentStatus.longRPM;
                currentStatus.RPMdiv100 = currentStatus.RPM / 100;
            }
            else {
                currentStatus.longRPM += 100;
                currentStatus.RPM = currentStatus.longRPM;
                currentStatus.RPMdiv100 = currentStatus.RPM / 100;
            }*/

            if (Serial.available() > 1) {
                Log.Add("WARNING: We are " + std::to_string(Serial.available()) + " bytes behind TS!");
            }
			char c = Read();

            if (c == 'F') {
                //F command is always allowed as it provides the initial serial protocol version. 
                LegacyComms(c);
            }
            else if ((((c >= 'A') && (c <= 'z')) || (c == '?')) && (allowLegacy)) {
                //Handle legacy cases here
                LegacyComms(c);
            }
            else {
                Comms(c);
            }
		}
	}
}
