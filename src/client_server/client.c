#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#define PORT 12345
#define BUF_SIZE 1024

// Global variables
SOCKET client_sock;
struct sockaddr_in server_addr;
char buffer[BUF_SIZE];
const char* input = "Affirmative";
HANDLE hEvent;
int colour;

DWORD WINAPI client_thread(LPVOID lpParam) {
    int recv_len;

    // First message to establish connection
    sendto(client_sock, 0, 0, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    
    // Server tells the client what colour they are
    recv_len = recvfrom(client_sock, buffer, BUF_SIZE, 0, NULL, NULL);
    buffer[recv_len] = '\0';
    if (strncmp("0", buffer, 2) == 0)
        colour = 0; // Black
    else 
        colour = 1; // White
    printf("%i\n", colour);

    // Synchronise both clients so the while loop is in sync.
    if (colour == 0) {
        input = "Black";
        memset(buffer, 0, BUF_SIZE);
        recvfrom(client_sock, buffer, BUF_SIZE, 0, NULL, NULL);
        printf("%s\n", buffer);

        WaitForSingleObject(hEvent, INFINITE);
        sendto(client_sock, input, strlen(input), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        ResetEvent(hEvent);
    } else {
        input = "White";
        WaitForSingleObject(hEvent, INFINITE);
        sendto(client_sock, input, strlen(input), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        ResetEvent(hEvent);
    }

    while (1) {
        memset(buffer, 0, BUF_SIZE);
        recvfrom(client_sock, buffer, BUF_SIZE, 0, NULL, NULL);
        printf("%s\n", buffer);

        WaitForSingleObject(hEvent, INFINITE);
        sendto(client_sock, input, strlen(input), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        ResetEvent(hEvent);
    }

    return 0;
}

int main() {
    WSADATA wsaData;
    HANDLE thread;
    DWORD threadId;

    hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    // Initialize Winsock
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    client_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    thread = CreateThread(NULL, 0, client_thread, NULL, 0, &threadId);
    if (thread == NULL) {
        printf("Thread creation failed.\n");
        closesocket(client_sock);
        WSACleanup();
        return 1;
    }

    while(1){
        
        SetEvent(hEvent);
    }
   
    // Cleanup
    CloseHandle(thread);
    closesocket(client_sock);
    WSACleanup();

    return 0;
}
