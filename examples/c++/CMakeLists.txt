cmake_minimum_required(VERSION 3.0.0)
project(ets2-telemetry-udp-example-c++ VERSION 0.1.0)

add_executable(ets2-telemetry-udp-example-c++
    "src/TelemetryClient.cpp"
    "src/main.cpp"
)

include_directories(ets2-telemetry-udp-example-c++ "./include")

if(WIN32)
    target_link_libraries(ets2-telemetry-udp-example-c++ wsock32 ws2_32)
endif()
target_compile_options(ets2-telemetry-udp-example-c++ PRIVATE -Wall -Wextra)

set_property(TARGET ets2-telemetry-udp-example-c++ PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
