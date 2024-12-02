cmake_minimum_required(VERSION 3.10)

# Set the project name
project(test C)

# Add the executable
add_executable(ServerUDP 
    Server.c
    lib/wrappers/addressTools.c 
    lib/wrappers/addressTools.h
    lib/wrappers/basicWrappers.c
    lib/wrappers/basicWrappers.h
    lib/argChecker.c
    lib/argChecker.h
    )

# Multi platform
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_options(ServerUDP PRIVATE /W4)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(ServerUDP PRIVATE -Wall -Wextra -Wpedantic)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_compile_options(ServerUDP PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Add standard
target_compile_features(ServerUDP PRIVATE c_std_99)