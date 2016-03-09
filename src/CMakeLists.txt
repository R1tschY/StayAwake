

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)
include_directories("${PROJECT_BINARY_DIR}")

file(GLOB stayawake_SOURCES "*.cpp")
file(GLOB stayawake_utils_SOURCES "utils/*.cpp")

add_executable(stayawake WIN32 ${stayawake_SOURCES} ${stayawake_utils_SOURCES})
target_link_libraries(stayawake PRIVATE 
	cpp-utils 
	lightports-base lightports-extra lightports-controls 
	comctl32
)

set_target_properties(stayawake PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
)
