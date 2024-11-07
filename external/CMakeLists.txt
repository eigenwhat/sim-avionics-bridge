
message("Reading ./external/CMakeLists.txt.")

#
# X-Plane SDK
#

macro(add_xpsdk_library TARGET)
    if(APPLE)
        add_library(${TARGET} SHARED IMPORTED GLOBAL)
    elseif(WIN32)
        add_library(${TARGET} STATIC IMPORTED GLOBAL)
    else()
        message(FATAL_ERROR "Unsupported target OS")
    endif()
endmacro()

add_xpsdk_library(XPLM)
target_include_directories(XPLM INTERFACE XPSDK/CHeaders/XPLM)

add_xpsdk_library(XPWidgets)
target_include_directories(XPWidgets INTERFACE XPSDK/CHeaders/Widgets)

if (APPLE)
    set_target_properties(XPLM PROPERTIES
            FRAMEWORK TRUE
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/XPSDK/Libraries/Mac/XPLM.framework")
    set_target_properties(XPWidgets PROPERTIES
            FRAMEWORK TRUE
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/XPSDK/Libraries/Mac/XPWidgets.framework")
elseif(WIN32)
    set_target_properties(XPLM PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/XPSDK/Libraries/Win/XPLM_64.lib")
    set_target_properties(XPWidgets PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/XPSDK/Libraries/Win/XPWidgets_64.lib")
endif()

# Combined X-Plane SDK target
add_library(XPSDK INTERFACE)
target_link_libraries(XPSDK INTERFACE XPLM XPWidgets)

#
# MSFS SimConnect SDK
#
if (WIN32)
    add_library(SimConnect STATIC IMPORTED GLOBAL)
    set_target_properties(SimConnect PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/FSSDK/SimConnect.dll"
        IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/FSSDK/SimConnect.lib")
    target_include_directories(SimConnect INTERFACE FSSDK)
endif()
