#pragma once

#define _HAS_STD_BYTE = 0

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#define CLOSESOCK(s) closesocket(s)
#define SOCKTYPE SOCKET
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define SOCKTYPE int
#define CLOSESOCK(s) close(s)
#endif