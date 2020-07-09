#include "TelemetryClient.hpp"
#include <iostream>

const int SOCKET_TIMEOUT_MS =  100;

TelemetryClient::TelemetryClient(){
    common = telemetry_common_s();
    truck = telemetry_truck_s();
    trailer = telemetry_trailer_s();

    config_truck = telemetry_config_truck_s();
    for(size_t i = 0; i < TELE_TRAILER_COUNT; i++){
        config_trailer[i] = telemetry_config_trailer_s();
    }
    config_job = telemetry_config_job_s();

    net_socket = -1;
    heartbeat_last_time = 0;
}

TelemetryClient::~TelemetryClient(){
    this->close();
}

bool TelemetryClient::connect(const char* ip, uint16_t port){
    if(net_socket >= 0){
        #ifdef _WIN32
            closesocket(net_socket);
        #else
            close(net_socket);
        #endif
        net_socket = -1;
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

    int set_timeout_success = -1;
    #ifdef WIN32
        DWORD timeout = SOCKET_TIMEOUT_MS;
        set_timeout_success = setsockopt(net_socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout));
    #else
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = SOCKET_TIMEOUT_MS * 1000;
        set_timeout_success = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    #endif
    if(set_timeout_success != 0){	
        std::cerr << "Failed to set socket timeout." << std::endl;
        closesocket(net_socket);	
        net_socket = -1;	
        return false;	
    }

    address = sockaddr_in();
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip);
    address.sin_port = htons(port);

    heartbeat_last_time = 0;

    return true;
}

void TelemetryClient::update(){
    time_t time_now = time(NULL);
    double delta_time = difftime(time_now, heartbeat_last_time);
    if(delta_time > 1.0){
        uint8_t heartbeat_packet[] = {TELE_PACKET_HEARTBEAT, 0xFF};
        sendto(
            net_socket,
            reinterpret_cast<char*>(heartbeat_packet),
            sizeof(heartbeat_packet),
            0,
            reinterpret_cast<sockaddr*>(&address),
            sizeof(address)
        );
        heartbeat_last_time = time_now;
    }

    static uint8_t buffer[4096];
    ssize_t recived = recv(net_socket, (char *)buffer, 4096, 0);

    if(recived <= 0){
        return;
    }

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