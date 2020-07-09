#pragma once
#include <cstdint>
#include <ctime>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "telemetry.hpp"

class TelemetryClient{
private:
    int net_socket;
    sockaddr_in address;
    time_t heartbeat_last_time;
public:
    telemetry_common_s common;
    telemetry_truck_s truck;
    telemetry_trailer_s trailer;

    telemetry_config_truck_s config_truck;
    telemetry_config_trailer_s config_trailer[TELE_TRAILER_COUNT];
    telemetry_config_job_s config_job;

    TelemetryClient();
    ~TelemetryClient();
    bool connect(const char* ip, uint16_t port);
    void update();
    void close();
};