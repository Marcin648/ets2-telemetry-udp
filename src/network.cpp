#include "network.hpp"

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

#include "defines.hpp"
#include "telemetry.hpp"
#include "log.hpp"

int net_socket = -1;
bool net_initialized = false;

#ifndef _WIN32
#define closesocket(socket) close(socket)
#endif

sockaddr_in bind_addr;

bool net_init(){
    if(net_initialized){
        return true;
    }

    #ifdef _WIN32
    WORD wVersionRequested = MAKEWORD(2, 0);
    WSADATA wsaData;

    int err = WSAStartup(wVersionRequested, &wsaData);
    if(err != 0){
        log(SCS_LOG_TYPE_error, "WinSock2 failed to initialized!");
        return false;
    }
    #endif

    if(net_socket != -1){
        closesocket(net_socket);
        net_socket = -1;
    }

    net_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(net_socket == -1){
        #ifdef _WIN32
            log(SCS_LOG_TYPE_error, "Failed to create socket. (%s)", WSAGetLastError());
        #else
            log(SCS_LOG_TYPE_error, "Failed to create socket.");
        #endif
        return false;
    }

    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = inet_addr(BIND_ADDRESS);
    bind_addr.sin_port = htons(BIND_PORT);
    
    log(SCS_LOG_TYPE_message, "Server start on port %i.", BIND_PORT);

    net_initialized = true;
    return true;
}

void net_close(){
    if(net_initialized){
        if(net_socket != -1){
            closesocket(net_socket);
            net_socket = -1;
        }
        #ifdef _WIN32
        WSACleanup();
        #endif
        net_initialized = false;
        log(SCS_LOG_TYPE_message, "Socket closed.");
    }
}

void net_send(uint8_t* data, size_t size){
    if(net_initialized){
        sendto(net_socket, reinterpret_cast<char*>(data), size, 0, (sockaddr*)(&bind_addr), sizeof(bind_addr));
    }
}

template <class T>
void net_send(uint8_t type, T &data){
    NetPacket<T> packet;
    packet.type = type;
    packet.data = data;
    net_send(reinterpret_cast<uint8_t*>(&packet), sizeof(packet));
    log(SCS_LOG_TYPE_message, "net_send: %i, %zi", type, sizeof(packet));
}

template void net_send(uint8_t type, telemetry_common_s &data);
template void net_send(uint8_t type, telemetry_truck_s &data);
template void net_send(uint8_t type, telemetry_trailer_s &data);
template void net_send(uint8_t type, telemetry_config_truck_s &data);
template void net_send(uint8_t type, telemetry_config_trailer_s &data);
template void net_send(uint8_t type, telemetry_config_job_s &data);