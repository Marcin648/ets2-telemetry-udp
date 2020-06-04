#include "TelemetryClient.hpp"

#include <iostream>
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

TelemetryClient::TelemetryClient(){
    net_socket = -1;
}

TelemetryClient::~TelemetryClient(){
    this->close();
}

bool TelemetryClient::connect(const char* ip, uint16_t port){
    if(net_socket >= 0){
        return true;
    }

    #ifdef _WIN32
    WORD wVersionRequested = MAKEWORD(2, 0);
    WSADATA wsaData;

    int err = WSAStartup(wVersionRequested, &wsaData);
    if(err != 0){
        std::cerr << "WinSock2 failed to initialized!" << std::endl;
        return false;
    }
    #endif

    net_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(net_socket == -1){
        #ifdef _WIN32
            std::cerr << "Failed to create socket. ( " << WSAGetLastError() << ")" << std::endl;
        #else
            std::cerr << "Failed to create socket." << std::endl;
        #endif
        return false;
    }

    sockaddr_in bind_addr;
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = inet_addr(ip);
    bind_addr.sin_port = htons(port);
    
    const char socket_reuseaddr = 1;
    int net_reuse = setsockopt(net_socket, SOL_SOCKET, SO_REUSEADDR, &socket_reuseaddr, sizeof(socket_reuseaddr));
    if(net_reuse != 0){
        std::cerr << "Failed to set reuse socket address." << std::endl;
        this->close();
        return false;
    }

    int net_bind = bind(net_socket, (const struct sockaddr *)&bind_addr, sizeof(bind_addr));
    if(net_bind < 0){
        std::cerr << "Failed to bind socket." << std::endl;
        this->close();
        return false;
    }
    return true;
}

void TelemetryClient::update(){
    static uint8_t buffer[4096];
    recv(net_socket, (char *)buffer, 4096, 0);

    uint8_t type = buffer[0];
    uint8_t* raw_data = &buffer[1];
    switch (type)
    {
        case TELE_PACKET_COMMON: {
            this->common = *(telemetry_common_s*)raw_data;
            break;
        }

        case TELE_PACKET_TRUCK: {
            this->truck = *(telemetry_truck_s*)raw_data;
            break;
        }

        case TELE_PACKET_TRAILER: {
            this->trailer = *(telemetry_trailer_s*)raw_data;
            break;
        }

        case TELE_PACKET_CONFIG_TRUCK: {
            this->config_truck = *(telemetry_config_truck_s*)raw_data;
            break;
        }

        case TELE_PACKET_CONFIG_TRAILER: {
            uint8_t index = raw_data[0];
            if(index < TELE_TRAILER_COUNT){
                this->config_trailer[index] = *(telemetry_config_trailer_s*)raw_data;
            }
            break;
        }

        case TELE_PACKET_CONFIG_JOB: {
            this->config_job = *(telemetry_config_job_s*)raw_data;
            break;
        }
        default: break;
    }
}

void TelemetryClient::close(){
    if(net_socket > 0){
        #ifdef _WIN32
            closesocket(net_socket);
        #else
            close(net_socket);
        #endif
        net_socket = -1;
    }
}