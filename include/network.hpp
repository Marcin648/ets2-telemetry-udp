#pragma once

#ifndef _WIN32
#define closesocket(socket) close(socket)
#endif

bool net_init();
void net_close();