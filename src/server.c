

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <netdb.h>
#include <poll.h>
#include <errno.h>

#define _GNU_SOURCE
#include <poll.h>

#define HOST "127.0.0.1"
#define DEFAULT_PORT "9090"

struct s_client {
	int fd;
	int port;
	char ip[INET_ADDRSTRLEN];
	char name[64];
};

typedef struct s_client Client;


bool isRunning= true;

int init_server(unsigned int port) {

	struct sockaddr_in sock_addr;
	memset(&sock_addr, 0, sizeof(struct sockaddr_in));

	int socket_file_descriptor;
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
	
	return socket_file_descriptor;
}

void addClientToList(Client* client, Client* client_list[]) {

	for (int i = 0; i<10; i++) {
		if (client_list[i] == 0) {
			client_list[i] = client;
			break;
		}
    	}
}

Client* createClientSocket(int fd) {
	//client TCP config and connection accept
	struct sockaddr_in client;
	unsigned int len = sizeof(client);
	Client* client_s = (Client *)malloc(sizeof(struct s_client)); // mozliwe ze bedzie trzeba wyzerowac
	memset(client_s, 0, sizeof(struct s_client));

	struct pollfd pfd = {
		.fd = fd,
		.events = POLLIN
	};

	
	int ret = poll(&pfd, 1, 10000);

	if (ret < 0) {
		perror("poll");
	}
	if (ret == 0) {
		printf("Brak nowych polaczen\n");
	}
	if (pfd.revents & POLLIN) {
		client_s->fd = accept(fd, (struct sockaddr *)&client, &len);
	}
	
	return client_s;
}


void main_loop(int fd, Client* client_list[]) { 

	Client* client = createClientSocket(fd);
	addClientToList(client, client_list);
	
	//getting a message
	char buf[512] = {0};

	//check clients for messages

	for (int i=0; i<10; i++) {

		printf("Curennt filed descriptor %p\n", (Client *)client_list[i]);
		memset(buf, 0, sizeof buf);
		if (!client_list[i]) return;
		int conn = recv(client_list[i]->fd, buf, 511, 0);
		
		if (conn > 0) { 
			printf("\nMessage from client: \n%s\n", buf);
			printf("Client with descriptor id %d \n", client_list[i]->fd);
			send(client_list[i]->fd, buf, 511, 0);
		} else if (errno == EINTR || conn == 0) {
			printf("Connection closed\n");
			close(client_list[i]->fd);
			memset(client_list[i], 0, sizeof(struct s_client));
		}

	}

//	for (int i = 0; i<10; i++) {
//		if (client_list[i] > 0) printf("Client connected nr %d, client file desc %d \n", i, client_list[i]->fd);
//	}
	//printf("End of the listening TCP loop\n");
}

/* TO-DO: Add htons or other combination to convert from network byte order to host or vice versa */
int main(int argc, char *argv[])
{
	Client* client_list[10] = {0};
	memset(client_list, 0, sizeof(client_list));
	unsigned int port;
	
	int socket_file_descriptor;
	if (argc < 2) {
	 printf("Port NOT provided, defaulting to %s \n", DEFAULT_PORT);
	 port = atoi((char *) DEFAULT_PORT);
	} else {
	port = atoi(argv[1]);
	}

	printf("Port that was provided %d \n", port);
	
	socket_file_descriptor = init_server(port);

	while(isRunning) {
		main_loop(socket_file_descriptor, client_list);
	}

	close(socket_file_descriptor);
	printf("It works\n");
	return 0;
}
