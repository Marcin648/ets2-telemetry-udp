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

#include <ctime>
#include <algorithm>
#include <list>

#include "defines.hpp"
#include "telemetry.hpp"
#include "log.hpp"

int net_socket = -1;
bool net_initialized = false;

#ifndef _WIN32
#define closesocket(socket) close(socket)
#endif

struct Client{
    sockaddr_in ip;
    time_t last_time;
};

std::list<Client> clients;

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

    sockaddr_in bind_addr = sockaddr_in();
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = INADDR_ANY;
    bind_addr.sin_port = htons(BIND_PORT);
    
    int bind_success = bind(net_socket, reinterpret_cast<sockaddr*>(&bind_addr), sizeof(bind_addr));	
    if(bind_success == -1){
        #ifdef _WIN32	
            log(SCS_LOG_TYPE_error, "Failed to assign address to socket. (%s)", WSAGetLastError());	
        #else	
            log(SCS_LOG_TYPE_error, "Failed to assign address to socket.");	
        #endif	
        closesocket(net_socket);	
        net_socket = -1;	
        return false;	
    }

    log(SCS_LOG_TYPE_message, "Server start on port %i.", BIND_PORT);

    net_initialized = true;
    clients.clear();
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
        clients.clear();
        log(SCS_LOG_TYPE_message, "Socket closed.");
    }
}

void net_accept(){
    NetPacket<uint8_t> packet;
    sockaddr_in client_addr = sockaddr_in();
    int client_addr_len = sizeof(client_addr);
    ssize_t recived = recvfrom(
        net_socket,
        reinterpret_cast<char*>(&packet),
        sizeof(packet), 0,
        reinterpret_cast<sockaddr*>(&client_addr),
        &client_addr_len
    );
    if(recived == sizeof(packet)){
        if(packet.type == TELE_PACKET_HEARTBEAT && packet.data == 0xFF){
            auto it = std::find_if(clients.begin(), clients.end(), [&](Client &client){
                //TODO: compare address
                return client.ip.sin_port == client_addr.sin_port;
            });
            if(it == clients.end()){
                Client client;
                client.ip = client_addr;
                client.last_time = time(NULL);
                clients.push_back(client);
                log(
                    SCS_LOG_TYPE_message,
                    "New client connected: %hhu.%hhu.%hhu.%hhu:%hu",
                    reinterpret_cast<uint8_t*>(&client.ip.sin_addr)[0],
                    reinterpret_cast<uint8_t*>(&client.ip.sin_addr)[1],
                    reinterpret_cast<uint8_t*>(&client.ip.sin_addr)[2],
                    reinterpret_cast<uint8_t*>(&client.ip.sin_addr)[3],
                    ntohs(client.ip.sin_port)
                );
            }else{
                it->last_time = time(NULL);
            }
        }
    }
}

void net_send(uint8_t* data, size_t size){
    if(net_initialized){
        time_t time_now = time(NULL);
        for(auto it = clients.begin(); it != clients.end();){
            double delta_time = difftime(time_now, it->last_time);
            if(delta_time > CLIENT_TIMEOUT){
                log(
                    SCS_LOG_TYPE_message,
                    "Client disconnected: %hhu.%hhu.%hhu.%hhu:%hu",
                    reinterpret_cast<uint8_t*>(&it->ip.sin_addr)[0],
                    reinterpret_cast<uint8_t*>(&it->ip.sin_addr)[1],
                    reinterpret_cast<uint8_t*>(&it->ip.sin_addr)[2],
                    reinterpret_cast<uint8_t*>(&it->ip.sin_addr)[3],
                    ntohs(it->ip.sin_port)
                );
                it = clients.erase(it);
            }else{
                sendto(
                    net_socket,
                    reinterpret_cast<char*>(data),
                    size,
                    0,
                    reinterpret_cast<sockaddr*>(&it->ip),
                    sizeof(it->ip)
                );
                it++;
            }
        }
    }
}

template <class T>
void net_send(uint8_t type, T &data){
    NetPacket<T> packet;
    packet.type = type;
    packet.data = data;
    net_send(reinterpret_cast<uint8_t*>(&packet), sizeof(packet));
    
    #ifndef NDEBUG
    log(SCS_LOG_TYPE_message, "net_send: %i, %zi", type, sizeof(packet));
    #endif
}

template void net_send(uint8_t type, telemetry_common_s &data);
template void net_send(uint8_t type, telemetry_truck_s &data);
template void net_send(uint8_t type, telemetry_trailer_s &data);
template void net_send(uint8_t type, telemetry_config_truck_s &data);
template void net_send(uint8_t type, telemetry_config_trailer_s &data);
template void net_send(uint8_t type, telemetry_config_job_s &data);