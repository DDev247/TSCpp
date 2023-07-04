
# Add source to this project's executable.
add_executable (TSCppESClient
	"src/main.cpp"
	"include/main.h")

# Add library to project.
target_link_libraries (TSCppESClient TSCpp nlohmann_json::nlohmann_json)

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET TSCppExample PROPERTY CXX_STANDARD 20)
endif()
