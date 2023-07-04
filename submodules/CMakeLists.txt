
set(JSON_BuildTests OFF CACHE INTERNAL "")

# Include submodules.
add_subdirectory ("json")
add_subdirectory ("CRCpp")
add_library (Serialib "Serialib/lib/serialib.cpp" "Serialib/lib/serialib.h")

# Add include directories.
include_directories ("CRCpp/inc")
include_directories ("Serialib/lib")
include_directories ("json/include")
