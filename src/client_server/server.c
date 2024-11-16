#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 12345
#define BUFFER_SIZE 1024

// gcc -Wall -g -o server.exe server.c -lws2_32 && gcc -Wall -g -o client.exe client.c -lws2_32
int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[BUFFER_SIZE];
    int clientAddrLen = sizeof(clientAddr);
    int clientCount = 0;
    
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 1;

    serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        WSACleanup();
        return 1;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    struct sockaddr_in client1, client2;

    while (1) {
        if (clientCount == 2) {
            break;
        }

        int recvLen = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (recvLen == SOCKET_ERROR) {
            printf("Receive failed\n");
            continue;
        }

        if (clientCount == 0) {
            client1 = clientAddr;
            printf("Client 1 connected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
            clientCount++;
        }
        else if (clientCount == 1) {
            client2 = clientAddr;
            printf("Client 2 connected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
            clientCount++;
        }
    }

    memset(buffer, 0, BUFFER_SIZE);
    strncpy(buffer, "0", 2);

    if (sendto(serverSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&client1, clientAddrLen) == SOCKET_ERROR)
        printf("Send to Client 1 failed\n");
    
    memset(buffer, 0, BUFFER_SIZE);
    strncpy(buffer, "1", 2);

    if (sendto(serverSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&client2, clientAddrLen) == SOCKET_ERROR)
        printf("Send to Client 2 failed\n");
    
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
