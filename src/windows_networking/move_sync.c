#include "chess.h"

DWORD WINAPI move_sync_thread(LPVOID lpParam) {
    extern SOCKET client_sock;
    extern struct sockaddr_in server_addr;
    extern HANDLE hEvent;
    
    uint8_t *board = (uint8_t *)lpParam;
    int recv_len;
    char initialServerResponse[2];
    int clientColour;

    // First message to establish connection
    sendto(client_sock, 0, 0, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    
    // Server tells the client what colour they are
    recv_len = recvfrom(client_sock, initialServerResponse, 2, 0, NULL, NULL);
    initialServerResponse[recv_len] = '\0';
    if (strncmp("0", initialServerResponse, 2) == 0)
        clientColour = 0; // Black
    else 
        clientColour = 1; // White
    
    // Synchronise both clients so the while loop is in sync.
    if (clientColour == 0) {
        recvfrom(client_sock, (char *) board, 64, 0, NULL, NULL);

        WaitForSingleObject(hEvent, INFINITE);
            sendto(client_sock, (char *) board, 64, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        ResetEvent(hEvent);
    } else {
        WaitForSingleObject(hEvent, INFINITE);
            sendto(client_sock, (char *) board, 64, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        ResetEvent(hEvent);
    }

    while (1) {
        recvfrom(client_sock, (char *) board, 64, 0, NULL, NULL);
        
        WaitForSingleObject(hEvent, INFINITE);
            sendto(client_sock, (char *) board, 64, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        ResetEvent(hEvent);
    }

    return 0;
}
