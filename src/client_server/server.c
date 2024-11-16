#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 12345
#define BUFFER_SIZE 64

// gcc -Wall -g -o server.exe server.c -lws2_32 && gcc -Wall -g -o client.exe client.c -lws2_32
int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[BUFFER_SIZE];
    int clientAddrLen = sizeof(clientAddr);
    int clientCount = 0;

    WSAStartup(MAKEWORD(2, 2), &wsaData);
    serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);
    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    struct sockaddr_in client_black, client_white;

    while (1) {
        if (clientCount == 2) {
            break;
        }

        recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
       
        if (clientCount == 0) {
            client_black = clientAddr;
            printf("Client 1 connected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
            clientCount++;
        }
        else if (clientCount == 1) {
            client_white = clientAddr;
            printf("Client 2 connected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
            clientCount++;
        }
    }
    printf("All clients connected\n");

    memset(buffer, 0, BUFFER_SIZE);
    strncpy(buffer, "0", 2);
    sendto(serverSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_black, clientAddrLen);
    
    memset(buffer, 0, BUFFER_SIZE);
    strncpy(buffer, "1", 2);
    sendto(serverSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_white, clientAddrLen);
      
    while(1) {
        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_white, &clientAddrLen);
        printf("%s\n", buffer);
        sendto(serverSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_black, clientAddrLen);
            
        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_black, &clientAddrLen);
        printf("%s\n", buffer);
        sendto(serverSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_white, clientAddrLen);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
