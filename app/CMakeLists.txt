cmake_minimum_required(VERSION 3.10)

# 1. Name your project
project(NanoPacketBridge)

# 2. Set C++ Standard (Required for modern features like threads/atomics)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 3. Add 'include/' to the compiler's search path
#    This lets you do #include "ConfigModule.hpp" instead of #include "../include/ConfigModule.hpp"
include_directories(include)

# 4. Automatically find all .cpp files in src/ AND its subdirectories
#    GLOB_RECURSE will find src/main.cpp, src/HTTPServer.cpp, src/utils/helpers.cpp, etc.
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 5. Find external libraries (Pcap for capture, Threads for multitasking)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PCAP REQUIRED libpcap)
find_package(Threads REQUIRED)

# 6. Build the executable
add_executable(packet_bridge ${SOURCES})

# 7. Link the libraries to your executable
target_link_libraries(packet_bridge PRIVATE
    ${PCAP_LIBRARIES}
    Threads::Threads
)