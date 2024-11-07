message("Reading ./libs/Common/CMakeLists.txt.")

add_library(Common OBJECT Network.cpp Utilities.cpp)
target_include_directories(Common PUBLIC .)
if(MINGW)
    target_compile_definitions(Common PUBLIC _WIN32=1)
    target_link_libraries(Common PUBLIC ws2_32)
endif()
