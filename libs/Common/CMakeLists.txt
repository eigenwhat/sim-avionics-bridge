message("Reading ./libs/Common/CMakeLists.txt.")

add_library(Common OBJECT Network.cpp Utilities.cpp)
target_include_directories(Common PUBLIC .)
if(MINGW)
    target_compile_definitions(Common PRIVATE _WIN32=1)
endif()
