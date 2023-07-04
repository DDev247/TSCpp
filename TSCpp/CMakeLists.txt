
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
	"include/pages.h"

	# ported speeduino source
	# "src/speeduino/globals.cpp"
	# "src/speeduino/utilities.cpp"
	# 
	# "src/speeduino/pages.cpp"
	# "src/speeduino/table3d.cpp"
	# "src/speeduino/table3d_axis_io.cpp"
	# "src/speeduino/table3d_interpolate.cpp"

	# ported speeduino headers
	# "include/speeduino/globals.h"
	# "include/speeduino/utilities.h"
	# 
	# "include/speeduino/int16_byte.h"
	# "include/speeduino/pages.h"
	# "include/speeduino/table3d.h"
	# "include/speeduino/table3d_axes.h"
	# "include/speeduino/table3d_axis_io.h"
	# "include/speeduino/table3d_interpolate.h"
	# "include/speeduino/table3d_typedefs.h"
	# "include/speeduino/table3d_values.h"
 "include/eeprom.h" "src/eeprom.cpp" "include/storage.h" "include/status.h")

include_directories (GLOBAL "include")

target_link_libraries (TSCpp Serialib CRCpp)

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET TSCpp PROPERTY CXX_STANDARD 20)
endif()

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
