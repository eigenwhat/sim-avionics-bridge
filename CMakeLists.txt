cmake_minimum_required(VERSION 3.25)
project(sim-avionics-bridge)

set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Build fat binary" FORCE)

add_subdirectory(external)
add_subdirectory(libs)
