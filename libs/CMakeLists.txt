message("Reading ./libs/CMakeLists.txt.")

add_subdirectory(Common)

if (WIN32)
    add_subdirectory(FSAvionicsBridge)
endif()

add_subdirectory(XPAvionicsBridge)
