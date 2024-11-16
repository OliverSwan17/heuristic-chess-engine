#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 12345
#define BUF_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET client_sock;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE];
    char input[64];
    int recv_len;

    WSAStartup(MAKEWORD(2, 2), &wsaData);
    client_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // First message to establish connection
    sendto(client_sock, 0, 0, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        
    // Server tells the client what colour they are
    recv_len = recvfrom(client_sock, buffer, BUF_SIZE, 0, NULL, NULL);
    buffer[recv_len] = '\0';
    int colour;
    if(strncmp("0", buffer, 2) == 0)
        colour = 0; // Black
    else 
        colour = 1; // White
    printf("%i\n", colour);

    // Synchronises both clients so the while loop is in sync.
    if (colour == 0){
        memset(buffer, 0, BUF_SIZE);
        recvfrom(client_sock, buffer, BUF_SIZE, 0, NULL, NULL);
        printf("%s\n", buffer);

        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        sendto(client_sock, input, strlen(input), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    }else{
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        sendto(client_sock, input, strlen(input), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    }

    while (1)
    {
        memset(buffer, 0, BUF_SIZE);
        recvfrom(client_sock, buffer, BUF_SIZE, 0, NULL, NULL);
        printf("%s\n", buffer);

        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        sendto(client_sock, input, strlen(input), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    }
    
    closesocket(client_sock);
    WSACleanup();
    return 0;
}
