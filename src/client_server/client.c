#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 12345
#define BUF_SIZE 1024

void error_handling(char *message) {
    perror(message);
    exit(1);
}

int main() {
    WSADATA wsaData;
    SOCKET client_sock;
    struct sockaddr_in server_addr;
    char message[] = "Hello, Server!";
    char buffer[BUF_SIZE];
    int recv_len;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        error_handling("WSAStartup() failed");
    }

    client_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client_sock == INVALID_SOCKET) {
        error_handling("Socket creation failed");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (sendto(client_sock, message, strlen(message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        error_handling("sendto failed");
    }

    recv_len = recvfrom(client_sock, buffer, BUF_SIZE, 0, NULL, NULL);
    if (recv_len == SOCKET_ERROR) {
        error_handling("recvfrom failed");
    }

    buffer[recv_len] = '\0';
    
    int colour;

    if(strncmp("0", buffer, 2) == 0)
        colour = 0; // Black
    else 
        colour = 1; // White
    
    printf("%i\n", colour);


    closesocket(client_sock);
    WSACleanup();
    return 0;
}
