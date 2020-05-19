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
#include "log.hpp"

int net_socket = -1;
bool net_initialized = false;

#ifndef _WIN32
#define closesocket(socket) close(socket)
#endif

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

    int non_blocking_success = -1;
    #ifdef WIN32
    unsigned long non_blocking = 1;
    non_blocking_success = ioctlsocket(net_socket, FIONBIO, &non_blocking);
    #else
    non_blocking_success = fcntl(net_socket, F_SETFL, O_NONBLOCK);
    #endif

    if(non_blocking_success != 0){
        log(SCS_LOG_TYPE_error, "Failed to set socket non blocking.");
        closesocket(net_socket);
        net_socket = -1;
        return false;
    }

    /*sockaddr_in bind_addr;
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = INADDR_ANY;
    bind_addr.sin_port = htons(BIND_PORT);
    
    int bind_success = bind(net_socket, (sockaddr*)&bind_addr, sizeof(bind_addr));
    if(bind_success == -1){
        #ifdef _WIN32
            log(SCS_LOG_TYPE_error, "Failed to assign address to socket. (%s)", WSAGetLastError());
        #else
            log(SCS_LOG_TYPE_error, "Failed to assign address to socket.");
        #endif
        closesocket(net_socket);
        net_socket = -1;
        return false;
    }*/
    
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