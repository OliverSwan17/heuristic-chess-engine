#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <stdint.h>

//Client thread definition
DWORD WINAPI client_thread(LPVOID lpParam);

// Constants
#define PORT 12345

// Global variables
SOCKET client_sock;
struct sockaddr_in server_addr;
HANDLE hEvent;
int client_colour;
extern uint8_t* board;


#endif