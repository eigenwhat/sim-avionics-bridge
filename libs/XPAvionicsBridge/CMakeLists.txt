message("Reading ./libs/XPAvionicsBridge/CMakeLists.txt.")
set(XPSRCS
        XPAvionicsBridge.cpp)
add_library(XPAvionicsBridge SHARED ${XPSRCS})

target_link_libraries(XPAvionicsBridge PUBLIC Common XPSDK)

if (APPLE)
    target_compile_definitions(XPAvionicsBridge PUBLIC XPLM200=1 XPLM210=1 XPL=1 IBM=0 APL=1 LIN=0)
    set_target_properties(XPAvionicsBridge
        PROPERTIES
            OUTPUT_NAME "XPAvionicsBridgeMac"
            OSX_ARCHITECTURES "arm64;x86_64")
endif()

set_target_properties(XPAvionicsBridge
    PROPERTIES
        PREFIX ""
        SUFFIX ".xpl")
