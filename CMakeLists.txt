cmake_minimum_required(VERSION 3.0.0)
project(ets2-telemetry-udp VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(ets2-telemetry-udp SHARED "ets2-telemetry-udp.cpp")

include_directories(ets2-telemetry-udp "./sdk")

if(WIN32)
    target_link_libraries(ets2-telemetry-udp wsock32 ws2_32)
endif()
target_compile_options(ets2-telemetry-udp PRIVATE -Wall -Wextra)

set(CMAKE_SHARED_LIBRARY_PREFIX "")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
