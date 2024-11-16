#include "client.h" // Extern/globals
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int client_thread(void *lpParam) {
	uint8_t *board = (uint8_t *)lpParam;
	int recv_len;
	char initialServerResponse[2];

	// First message to establish connection
    	sendto(client_sock, 0, 0, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    
    	// Server tells the client what colour they are
	    recv_len = recvfrom(client_sock, initialServerResponse, 2, 0, NULL, NULL);
	    initialServerResponse[recv_len] = '\0';
    	if (strncmp("0", initialServerResponse, 2) == 0)
        	client_colour = 0; // Black
    	else 
        	client_colour = 1; // White
    
   	 // Synchronise both clients so the while loop is in sync.
    	if (client_colour == 0) {
        	recvfrom(client_sock, (char *) board, 64, 0, NULL, NULL);

        	//WaitForSingleObject(hEvent, INFINITE);
            	sendto(client_sock, (char *) board, 64, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        	//ResetEvent(hEvent);
    	} else {
        	//WaitForSingleObject(hEvent, INFINITE);
            	sendto(client_sock, (char *) board, 64, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        	//ResetEvent(hEvent);
    	}

    	while (1) {
        	recvfrom(client_sock, (char *) board, 64, 0, NULL, NULL);
       		// Is there some weird IPC/mutex thing happening with WaitForSingleObject? 
            	sendto(client_sock, (char *) board, 64, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    	}

    	return 0;
}

