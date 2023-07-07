
# Add source to this project's library.
add_library (TSCpp
	# bare minimum source files
	"src/main.cpp"
	"src/thread.cpp"
	"src/legacy_comms.cpp"
	"src/comms.cpp"
	
	# bare minimum header files
	"include/main.h"
	"include/tscpp.h"

	# logging
	"src/logging.cpp"
	"include/logging.h"

	# pages
	"src/pages.cpp"
	"src/page_crc.cpp"
	"include/pages.h"
	"include/page_crc.h"

	# eeprom?
	"include/eeprom.h"
	"src/eeprom.cpp"

	# ?
	"src/storage.cpp"
	"include/storage.h"
	"include/statuses.h"
	"src/statuses.cpp"
	"include/globals.h"

	# ported speeduino source
	# "src/speeduino/globals.cpp"
	# "src/speeduino/utilities.cpp"
	# 
	# "src/speeduino/pages.cpp"
	"src/table3d.cpp"
	"src/table3d_axis_io.cpp"
	"src/table3d_interpolate.cpp"

	# ported speeduino headers
	# "include/speeduino/globals.h"
	# "include/speeduino/utilities.h"
	# 
	"include/int16_byte.h"
	# "include/speeduino/pages.h"
	"include/table2d.h"
	"include/table3d.h"
	"include/table3d_axes.h"
	"include/table3d_axis_io.h"
	"include/table3d_interpolate.h"
	"include/table3d_typedefs.h"
	"include/table3d_values.h"
	"include/utilities.h" "include/types.h")

include_directories (GLOBAL "include")

target_link_libraries (TSCpp Serialib CRCpp)

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET TSCpp PROPERTY CXX_STANDARD 20)
endif()

if (MSVC)
	add_compile_options ("/utf-8")
	add_compile_options ("/utf-8")
endif(MSVC)

if (MINGW)
	add_compile_options ("-Wno-narrowing")
	add_compile_options ("-mfp16-format=ieee")
endif(MINGW)
# Wno-narrowing

#add_compile_options("$<$<C_COMPILER_ID:GCC>:-mfp16-format=ieee>")
#add_compile_options("$<$<CXX_COMPILER_ID:GCC>:-mfp16-format=ieee>")
