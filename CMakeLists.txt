cmake_minimum_required(VERSION 3.10)
project(zc-test)

set(CMAKE_CXX_STANDARD 17)

option(WITH_TESTS "Build test (over gtest)" OFF)

find_program(CCACHE ccache)
if(CCACHE)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE})
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE})
endif(CCACHE)

#set(SANITIZERS "-fsanitize=leak")
#set(SANITIZERS "-fsanitize=address")
#set(SANITIZERS "-fsanitize=thread")
#set(SANITIZERS "-fsanitize=undefined")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SANITIZERS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZERS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZERS}")

include_directories(src)

file(GLOB_RECURSE CPP_FILES src/*.cpp src/*.c)

add_executable(watch_addresses ${CPP_FILES})
target_link_libraries(watch_addresses pthread)
