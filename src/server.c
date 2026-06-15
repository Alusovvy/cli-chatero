

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>

#define HOST "127.0.0.1"
#define DEFAULT_PORT "9090"

/* TO-DO: Add htons or other combination to convert from network byte order to host or vice versa */
int main(int argc, char *argv[])
{
	unsigned short int port;
	int socket_file_descriptor;
	struct sockaddr_in sock_addr;
	bool isListening = true;	
	if (argc < 2) {
	 printf("Port NOT provided, defaulting to %s \n", DEFAULT_PORT);
	 port = atoi((char *) DEFAULT_PORT);
	} else {

	port = atoi(argv[1]);
	}


	printf("Port that was provided %d \n", port);

	socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_file_descriptor < 0) {
	 	printf("Error while creating a socket\n");
		return -1;
	}

	printf("Socket was created: %i\n", socket_file_descriptor);

	// preapre sock addr data for binding
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(port);

	// converts ipv4 from text to binary
	inet_pton(AF_INET, HOST, &sock_addr.sin_addr);
	if (bind(socket_file_descriptor,(struct sockaddr *) &sock_addr, sizeof(sock_addr)) < 0) {
		printf("Socket binding failed \n");
		return -1;
	}
	
	listen(socket_file_descriptor, 20);

	while(isListening) {
		struct sockaddr_in client;
		unsigned int len = sizeof(client);
		int client_socket;
		char ip[INET_ADDRSTRLEN];
		int port;

		client_socket = accept(socket_file_descriptor, (struct sockaddr *)&client, &len);

		if (client_socket < 0) {
		 sleep(1);
		} else {
		 printf("Client connected\n");
		 isListening = false;
		}


		printf("End of the listening TCP loop\n");
	}

	close(socket_file_descriptor);
	printf("It works\n");
	return 0;
}
