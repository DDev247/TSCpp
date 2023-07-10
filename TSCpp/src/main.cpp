
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
		Log.Dump(3);
		exit(1);
	}
	Log.Add("Serial port opened and ready.");

	Log.Add("Loading config...");

	loadConfig();

    // stuff
	reload2D();

	Log.Add("Config loaded.");

	Log.Add("Starting main recv loop...");
	recvThread = std::thread(&TSCpp::threadFn, this);

	Log.Add("Starting timer loop...");
	timerThread = std::thread(&TSCpp::timerFn, this);
	Log.DumpAll();
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
        reload2D();
		EEPROM.writeFile();
		Log.DumpAll();
	}

	Log.Add("Stopping timer loop...");
}

void TSCpp::reload2D() {
    taeTable.valueSize = SIZE_BYTE; //Set this table to use byte values
    taeTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    taeTable.xSize = 4;
    taeTable.values = configPage4.taeValues;
    taeTable.axisX = configPage4.taeBins;
    maeTable.valueSize = SIZE_BYTE; //Set this table to use byte values
    maeTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    maeTable.xSize = 4;
    maeTable.values = configPage4.maeRates;
    maeTable.axisX = configPage4.maeBins;
    WUETable.valueSize = SIZE_BYTE; //Set this table to use byte values
    WUETable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    WUETable.xSize = 10;
    WUETable.values = configPage2.wueValues;
    WUETable.axisX = configPage4.wueBins;
    ASETable.valueSize = SIZE_BYTE;
    ASETable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    ASETable.xSize = 4;
    ASETable.values = configPage2.asePct;
    ASETable.axisX = configPage2.aseBins;
    ASECountTable.valueSize = SIZE_BYTE;
    ASECountTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    ASECountTable.xSize = 4;
    ASECountTable.values = configPage2.aseCount;
    ASECountTable.axisX = configPage2.aseBins;
    PrimingPulseTable.valueSize = SIZE_BYTE;
    PrimingPulseTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    PrimingPulseTable.xSize = 4;
    PrimingPulseTable.values = configPage2.primePulse;
    PrimingPulseTable.axisX = configPage2.primeBins;
    crankingEnrichTable.valueSize = SIZE_BYTE;
    crankingEnrichTable.axisSize = SIZE_BYTE;
    crankingEnrichTable.xSize = 4;
    crankingEnrichTable.values = configPage10.crankingEnrichValues;
    crankingEnrichTable.axisX = configPage10.crankingEnrichBins;

    dwellVCorrectionTable.valueSize = SIZE_BYTE;
    dwellVCorrectionTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    dwellVCorrectionTable.xSize = 6;
    dwellVCorrectionTable.values = configPage4.dwellCorrectionValues;
    dwellVCorrectionTable.axisX = configPage6.voltageCorrectionBins;
    injectorVCorrectionTable.valueSize = SIZE_BYTE;
    injectorVCorrectionTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    injectorVCorrectionTable.xSize = 6;
    injectorVCorrectionTable.values = configPage6.injVoltageCorrectionValues;
    injectorVCorrectionTable.axisX = configPage6.voltageCorrectionBins;
    injectorAngleTable.valueSize = SIZE_INT;
    injectorAngleTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    injectorAngleTable.xSize = 4;
    injectorAngleTable.values = configPage2.injAng;
    injectorAngleTable.axisX = configPage2.injAngRPM;
    IATDensityCorrectionTable.valueSize = SIZE_BYTE;
    IATDensityCorrectionTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    IATDensityCorrectionTable.xSize = 9;
    IATDensityCorrectionTable.values = configPage6.airDenRates;
    IATDensityCorrectionTable.axisX = configPage6.airDenBins;
    baroFuelTable.valueSize = SIZE_BYTE;
    baroFuelTable.axisSize = SIZE_BYTE;
    baroFuelTable.xSize = 8;
    baroFuelTable.values = configPage4.baroFuelValues;
    baroFuelTable.axisX = configPage4.baroFuelBins;
    IATRetardTable.valueSize = SIZE_BYTE;
    IATRetardTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    IATRetardTable.xSize = 6;
    IATRetardTable.values = configPage4.iatRetValues;
    IATRetardTable.axisX = configPage4.iatRetBins;
    CLTAdvanceTable.valueSize = SIZE_BYTE;
    CLTAdvanceTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    CLTAdvanceTable.xSize = 6;
    CLTAdvanceTable.values = (byte*)configPage4.cltAdvValues;
    CLTAdvanceTable.axisX = configPage4.cltAdvBins;
    idleTargetTable.valueSize = SIZE_BYTE;
    idleTargetTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    idleTargetTable.xSize = 10;
    idleTargetTable.values = configPage6.iacCLValues;
    idleTargetTable.axisX = configPage6.iacBins;
    idleAdvanceTable.valueSize = SIZE_BYTE;
    idleAdvanceTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    idleAdvanceTable.xSize = 6;
    idleAdvanceTable.values = (byte*)configPage4.idleAdvValues;
    idleAdvanceTable.axisX = configPage4.idleAdvBins;
    rotarySplitTable.valueSize = SIZE_BYTE;
    rotarySplitTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    rotarySplitTable.xSize = 8;
    rotarySplitTable.values = configPage10.rotarySplitValues;
    rotarySplitTable.axisX = configPage10.rotarySplitBins;

    flexFuelTable.valueSize = SIZE_BYTE;
    flexFuelTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    flexFuelTable.xSize = 6;
    flexFuelTable.values = configPage10.flexFuelAdj;
    flexFuelTable.axisX = configPage10.flexFuelBins;
    flexAdvTable.valueSize = SIZE_BYTE;
    flexAdvTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    flexAdvTable.xSize = 6;
    flexAdvTable.values = configPage10.flexAdvAdj;
    flexAdvTable.axisX = configPage10.flexAdvBins;
    flexBoostTable.valueSize = SIZE_INT;
    flexBoostTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins (NOTE THIS IS DIFFERENT TO THE VALUES!!)
    flexBoostTable.xSize = 6;
    flexBoostTable.values = configPage10.flexBoostAdj;
    flexBoostTable.axisX = configPage10.flexBoostBins;
    fuelTempTable.valueSize = SIZE_BYTE;
    fuelTempTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    fuelTempTable.xSize = 6;
    fuelTempTable.values = configPage10.fuelTempValues;
    fuelTempTable.axisX = configPage10.fuelTempBins;

    knockWindowStartTable.valueSize = SIZE_BYTE;
    knockWindowStartTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    knockWindowStartTable.xSize = 6;
    knockWindowStartTable.values = configPage10.knock_window_angle;
    knockWindowStartTable.axisX = configPage10.knock_window_rpms;
    knockWindowDurationTable.valueSize = SIZE_BYTE;
    knockWindowDurationTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    knockWindowDurationTable.xSize = 6;
    knockWindowDurationTable.values = configPage10.knock_window_dur;
    knockWindowDurationTable.axisX = configPage10.knock_window_rpms;

    oilPressureProtectTable.valueSize = SIZE_BYTE;
    oilPressureProtectTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    oilPressureProtectTable.xSize = 4;
    oilPressureProtectTable.values = configPage10.oilPressureProtMins;
    oilPressureProtectTable.axisX = configPage10.oilPressureProtRPM;

    coolantProtectTable.valueSize = SIZE_BYTE;
    coolantProtectTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    coolantProtectTable.xSize = 6;
    coolantProtectTable.values = configPage9.coolantProtRPM;
    coolantProtectTable.axisX = configPage9.coolantProtTemp;


    fanPWMTable.valueSize = SIZE_BYTE;
    fanPWMTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    fanPWMTable.xSize = 4;
    fanPWMTable.values = configPage9.PWMFanDuty;
    fanPWMTable.axisX = configPage6.fanPWMBins;

    wmiAdvTable.valueSize = SIZE_BYTE;
    wmiAdvTable.axisSize = SIZE_BYTE; //Set this table to use byte axis bins
    wmiAdvTable.xSize = 6;
    wmiAdvTable.values = configPage10.wmiAdvAdj;
    wmiAdvTable.axisX = configPage10.wmiAdvBins;

    cltCalibrationTable.valueSize = SIZE_INT;
    cltCalibrationTable.axisSize = SIZE_INT;
    cltCalibrationTable.xSize = 32;
    cltCalibrationTable.values = cltCalibration_values;
    cltCalibrationTable.axisX = cltCalibration_bins;

    iatCalibrationTable.valueSize = SIZE_INT;
    iatCalibrationTable.axisSize = SIZE_INT;
    iatCalibrationTable.xSize = 32;
    iatCalibrationTable.values = iatCalibration_values;
    iatCalibrationTable.axisX = iatCalibration_bins;

    o2CalibrationTable.valueSize = SIZE_BYTE;
    o2CalibrationTable.axisSize = SIZE_INT;
    o2CalibrationTable.xSize = 32;
    o2CalibrationTable.values = o2Calibration_values;
    o2CalibrationTable.axisX = o2Calibration_bins;
}

unsigned int makeWord(unsigned char h, unsigned char l) { return (h << 8) | l; }
