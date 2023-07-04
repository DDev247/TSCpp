cmake_minimum_required (VERSION 3.8)

project ("TSC++")

# Include submodules.
add_subdirectory ("submodules")

# Include projects.
add_subdirectory ("TSCpp")
add_subdirectory ("example")
add_subdirectory ("es-client-example")
