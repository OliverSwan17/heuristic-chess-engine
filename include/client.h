#ifndef CLIENT_H
#define CLIENT_H

#ifdef _WIN32
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <stdint.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

// Constants
#define PORT 12345

#ifdef _WIN32
DWORD WINAPI client_thread(LPVOID lpParam);
SOCKET client_sock;
struct sockaddr_in server_addr;
HANDLE hEvent;
int client_colour;
extern uint8_t* board;
#else
int client_thread(void *lpParam);
int client_sock;
struct sockaddr_in server_addr;
int client_colour;
extern uint8_t* board;
#endif

#endif
