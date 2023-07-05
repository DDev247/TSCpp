cmake_minimum_required (VERSION 3.8)

option(BUILD_EXAMPLES "Compile Examples" OFF)

project ("TSC++")

# Include submodules.
add_subdirectory ("submodules")

# Include projects.
add_subdirectory ("TSCpp")

if(BUILD_EXAMPLES)
	add_subdirectory ("example")
	add_subdirectory ("es-client-example")
endif(BUILD_EXAMPLES)
