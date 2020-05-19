#pragma once

#include <cstdint>

#ifndef _WIN32
#define closesocket(socket) close(socket)
#endif

extern bool net_initialized;

bool net_init();
void net_close();
void net_send(uint8_t* data, size_t size);