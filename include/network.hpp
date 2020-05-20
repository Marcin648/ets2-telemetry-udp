#pragma once

#include <cstdint>

#ifndef _WIN32
#define closesocket(socket) close(socket)
#endif

extern bool net_initialized;

#pragma pack(push, 1)
template <class T>
struct NetPacket{
    uint8_t type;
    T data;
};
#pragma pack(pop)

bool net_init();
void net_close();
void net_send(uint8_t* data, size_t size);

template <class T>
void net_send(uint8_t type, T &data);