
#include "statuses.h"

struct statuses currentStatus;
struct config2 configPage2;
struct config4 configPage4;
struct config6 configPage6;
struct config9 configPage9;
struct config10 configPage10;
struct config13 configPage13;
struct config15 configPage15;

struct table3d16RpmLoad fuelTable; ///< 16x16 fuel map
struct table3d16RpmLoad fuelTable2; ///< 16x16 fuel map
struct table3d16RpmLoad ignitionTable; ///< 16x16 ignition map
struct table3d16RpmLoad ignitionTable2; ///< 16x16 ignition map
struct table3d16RpmLoad afrTable; ///< 16x16 afr target map
struct table3d8RpmLoad stagingTable; ///< 8x8 fuel staging table
struct table3d8RpmLoad boostTable; ///< 8x8 boost map
struct table3d8RpmLoad boostTableLookupDuty; ///< 8x8 boost map lookup table
struct table3d8RpmLoad vvtTable; ///< 8x8 vvt map
struct table3d8RpmLoad vvt2Table; ///< 8x8 vvt2 map
struct table3d8RpmLoad wmiTable; ///< 8x8 wmi map
trimTable3d trim1Table; ///< 6x6 Fuel trim 1 map
trimTable3d trim2Table; ///< 6x6 Fuel trim 2 map
trimTable3d trim3Table; ///< 6x6 Fuel trim 3 map
trimTable3d trim4Table; ///< 6x6 Fuel trim 4 map
trimTable3d trim5Table; ///< 6x6 Fuel trim 5 map
trimTable3d trim6Table; ///< 6x6 Fuel trim 6 map
trimTable3d trim7Table; ///< 6x6 Fuel trim 7 map
trimTable3d trim8Table; ///< 6x6 Fuel trim 8 map
struct table3d4RpmLoad dwellTable; ///< 4x4 Dwell map
struct table2D taeTable; ///< 4 bin TPS Acceleration Enrichment map (2D)
struct table2D maeTable;
struct table2D WUETable; ///< 10 bin Warm Up Enrichment map (2D)
struct table2D ASETable; ///< 4 bin After Start Enrichment map (2D)
struct table2D ASECountTable; ///< 4 bin After Start duration map (2D)
struct table2D PrimingPulseTable; ///< 4 bin Priming pulsewidth map (2D)
struct table2D crankingEnrichTable; ///< 4 bin cranking Enrichment map (2D)
struct table2D dwellVCorrectionTable; ///< 6 bin dwell voltage correction (2D)
struct table2D injectorVCorrectionTable; ///< 6 bin injector voltage correction (2D)
struct table2D injectorAngleTable; ///< 4 bin injector angle curve (2D)
struct table2D IATDensityCorrectionTable; ///< 9 bin inlet air temperature density correction (2D)
struct table2D baroFuelTable; ///< 8 bin baro correction curve (2D)
struct table2D IATRetardTable; ///< 6 bin ignition adjustment based on inlet air temperature  (2D)
struct table2D idleTargetTable; ///< 10 bin idle target table for idle timing (2D)
struct table2D idleAdvanceTable; ///< 6 bin idle advance adjustment table based on RPM difference  (2D)
struct table2D CLTAdvanceTable; ///< 6 bin ignition adjustment based on coolant temperature  (2D)
struct table2D rotarySplitTable; ///< 8 bin ignition split curve for rotary leading/trailing  (2D)
struct table2D flexFuelTable;  ///< 6 bin flex fuel correction table for fuel adjustments (2D)
struct table2D flexAdvTable;   ///< 6 bin flex fuel correction table for timing advance (2D)
struct table2D flexBoostTable; ///< 6 bin flex fuel correction table for boost adjustments (2D)
struct table2D fuelTempTable;  ///< 6 bin flex fuel correction table for fuel adjustments (2D)
struct table2D knockWindowStartTable;
struct table2D knockWindowDurationTable;
struct table2D oilPressureProtectTable;
struct table2D wmiAdvTable; //6 bin wmi correction table for timing advance (2D)
struct table2D coolantProtectTable;
struct table2D fanPWMTable;

uint16_t cltCalibration_bins[32];
uint16_t cltCalibration_values[32];
uint16_t iatCalibration_bins[32];
uint16_t iatCalibration_values[32];
uint16_t o2Calibration_bins[32];
uint8_t  o2Calibration_values[32]; // Note 8-bit values
struct table2D cltCalibrationTable; /**< A 32 bin array containing the coolant temperature sensor calibration values */
struct table2D iatCalibrationTable; /**< A 32 bin array containing the inlet air temperature sensor calibration values */
struct table2D o2CalibrationTable; /**< A 32 bin array containing the O2 sensor calibration values */

