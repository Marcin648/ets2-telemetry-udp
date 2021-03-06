cmake_minimum_required(VERSION 3.13.0)
project(ets2-telemetry-udp VERSION 0.1.0)

#include(CTest)
#enable_testing()

add_library(ets2-telemetry-udp SHARED 
    "src/log.cpp"
    "src/network.cpp"
    "src/store.cpp"
    "src/register.cpp"
    "src/debug.cpp"
    "src/ets2-telemetry-udp.cpp"
)

include_directories(ets2-telemetry-udp "./sdk")
include_directories(ets2-telemetry-udp "./include")

if(WIN32)
    target_link_libraries(ets2-telemetry-udp wsock32 ws2_32)
endif()

if (MSVC)
    target_compile_options(ets2-telemetry-udp PUBLIC /W4 /WX /O2)
else()
    target_compile_options(ets2-telemetry-udp PUBLIC  -Wall -Wextra -O3)
    target_link_options(ets2-telemetry-udp PUBLIC -static)
endif()

set_property(TARGET ets2-telemetry-udp PROPERTY CXX_STANDARD 17)

set(CMAKE_SHARED_LIBRARY_PREFIX "")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
