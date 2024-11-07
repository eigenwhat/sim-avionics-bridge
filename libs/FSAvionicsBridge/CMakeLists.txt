message("Reading ./libs/FSAvionicsBridge/CMakeLists.txt.")

set(FSBRIDGE_SOURCES
    FSAvionicsBridge.cpp
    FSFlightData.cpp FSFlightData.h)
add_executable(FSAvionicsBridge ${FSBRIDGE_SOURCES})
target_link_libraries(FSAvionicsBridge PUBLIC Common SimConnect)
if(MINGW)
    target_compile_definitions(FSAvionicsBridge PRIVATE _WIN32=1)
    target_link_options(FSAvionicsBridge PUBLIC -static-libgcc -static-libstdc++ -static)
endif()
