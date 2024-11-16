#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 64
#define WAITING_CLIENTS_LOOP 1
#define GAME_COMMS_LOOP 1

int main() {

	struct sockaddr_in serverAddr, 
			   clientAddr,
			   client_black,
			   client_white;	
	int serverSocket;
	char buffer[BUFFER_SIZE];
	int clientAddrLen = (int)sizeof(clientAddr);
	int clientCount = 0;
	memset(&serverAddr, 0x00, sizeof(serverAddr));
	
	if ((serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		fprintf(stderr, "Socket creation failure");
		return -1;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(PORT);

	if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
		fprintf(stderr, "Socket bind failure");
		return -1;
	}

	printf("Waiting for clients...\n");

	while (WAITING_CLIENTS_LOOP) {

		ssize_t messageLength = 0;

		if (clientCount >= 2) {
			fprintf(stderr, "Excess client connections");
			break;
		}

		messageLength = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, 
				(struct sockaddr *)&clientAddr, &clientAddrLen);

		if (clientCount == 0) {
			client_black = clientAddr;
			printf("Client 1 connected: %s:%d\n", inet_ntoa(clientAddr.sin_addr),
							      ntohs(clientAddr.sin_port));
			clientCount++;
		}
		else if (clientCount == 1) {
			client_white = clientAddr;
			printf("Client 2 connected: %s:%d\n", inet_ntoa(clientAddr.sin_addr),
							      ntohs(clientAddr.sin_port));
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

	while (GAME_COMMS_LOOP) {
		memset(buffer, 0, BUFFER_SIZE);
		recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_white, &clientAddrLen);
		printf("%s\n", buffer);
		sendto(serverSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_white, clientAddrLen);
	
		memset(buffer, 0, BUFFER_SIZE);
		recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_black, &clientAddrLen);
		printf("%s\n", buffer);
		sendto(serverSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_white, clientAddrLen);
	}
	close(serverSocket);


	return 0;
}
