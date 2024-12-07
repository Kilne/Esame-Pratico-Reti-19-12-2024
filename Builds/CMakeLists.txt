cmake_minimum_required(VERSION 3.10)

# Project name
project(GameServerClient)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/../lib)

# Source files
set(SERVER_SOURCES ${CMAKE_SOURCE_DIR}/../Server/Server.c)
set(CLIENT_SOURCES ${CMAKE_SOURCE_DIR}/../Client/Client.c)

# Library files
set(LIB_SOURCES
    ${CMAKE_SOURCE_DIR}/../lib/wrappers/customRecvFrom.c
    ${CMAKE_SOURCE_DIR}/../lib/wrappers/basicWrappers.c
    ${CMAKE_SOURCE_DIR}/../lib/wrappers/pollUtils.c
    ${CMAKE_SOURCE_DIR}/../lib/wrappers/customSendTo.c
    ${CMAKE_SOURCE_DIR}/../lib/wrappers/customErrorPrinting.c
    ${CMAKE_SOURCE_DIR}/../lib/wrappers/addressTools.c
    ${CMAKE_SOURCE_DIR}/../lib/argChecker.c
    ${CMAKE_SOURCE_DIR}/../lib/wrappers/customRecvFrom.h
    ${CMAKE_SOURCE_DIR}/../lib/wrappers/basicWrappers.h
    ${CMAKE_SOURCE_DIR}/../lib/wrappers/pollUtils.h
    ${CMAKE_SOURCE_DIR}/../lib/wrappers/customSendTo.h
    ${CMAKE_SOURCE_DIR}/../lib/wrappers/customErrorPrinting.h
    ${CMAKE_SOURCE_DIR}/../lib/wrappers/addressTools.h
    ${CMAKE_SOURCE_DIR}/../lib/wrappers/bufHandlers.c
    ${CMAKE_SOURCE_DIR}/../lib/wrappers/bufHandlers.h
    ${CMAKE_SOURCE_DIR}/../lib/wrappers/customConnection.c
    ${CMAKE_SOURCE_DIR}/../lib/wrappers/customConnection.h
    ${CMAKE_SOURCE_DIR}/../lib/wrappers/customUDPTransmission.c
    ${CMAKE_SOURCE_DIR}/../lib/wrappers/customUDPTransmission.h
    ${CMAKE_SOURCE_DIR}/../lib/gamelogics/meteorites.c
    ${CMAKE_SOURCE_DIR}/../lib/gamelogics/meteorites.h
    ${CMAKE_SOURCE_DIR}/../lib/argChecker.h
)

# Add executable targets
add_executable(Server ${SERVER_SOURCES} ${LIB_SOURCES})
add_executable(Client ${CLIENT_SOURCES} ${LIB_SOURCES})
