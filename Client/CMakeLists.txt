cmake_minimum_required(VERSION 3.10)

# Set the project name
project(ClientUDP C)

# Add the executable
add_executable(ClientUDP 
    Client.c
    lib/wrappers/addressTools.c 
    lib/wrappers/addressTools.h
    lib/wrappers/basicWrappers.c
    lib/wrappers/basicWrappers.h
    lib/wrappers/customErrorPrinting.c
    lib/wrappers/customErrorPrinting.h
    lib/wrappers/pollUtils.c
    lib/wrappers/pollUtils.h
    lib/argChecker.c
    lib/argChecker.h
    )

# Multi platform
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_options(ClientUDP PRIVATE /W4)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(ClientUDP PRIVATE -Wall -Wextra -Wpedantic)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_compile_options(ClientUDP PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Add standard
target_compile_features(ClientUDP PRIVATE c_std_99)