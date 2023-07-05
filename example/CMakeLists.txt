
# Add source to this project's executable.
add_executable (TSCppExample
	"src/main.cpp"
	"include/main.h"
)

# Add library to project.
target_link_libraries (TSCppExample TSCpp)

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET TSCppExample PROPERTY CXX_STANDARD 20)
endif()
