#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/wait.h>

#define SERVER_PORT 8877
#define SERVER_ADDRES INADDR_ANY

#define BUFFER_SIZE 1024

#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }


int main(int argc, char *argv[]) {
    int server_fd, client_fd, err;
    
    // max allowed clients, after which dropping begins
	int wait_size = 16;

    int read = 0;

	char buffer[BUFFER_SIZE];

	// create struct socker addres 
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;

    // socket address used to store client address
	struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

	// htons: host to network short: transforms a value in host byte
	// ordering format to a short value in network byte ordering format
	server_address.sin_port = htons(SERVER_PORT);

	// htonl: host to network long: same as htons but to long
	server_address.sin_addr.s_addr = htonl(SERVER_ADDRES);

	// create a TCP socket, creation returns -1 on failure
	if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        on_error("Could not create listen socket\n");
	}

	// bind it to listen to the incoming connections on the created server
	// address, will return -1 on error
	if ((bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address))) < 0) {
        on_error("Could not bind socket\n");
	} 

	if (listen(server_fd, wait_size) < 0) {
        on_error("Could not open socket for listening\n");
	}

	while (true) {
		// open a new socket to transmit data per connection
		if ((client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_address_len)) < 0) {
            on_error("Could not open a socket to accept data\n");
		}

		printf("client connected with ip address: %s\n", inet_ntoa(client_address.sin_addr));

		// keep running as long as the client keeps the connection open
        while(true){
            read = recv(client_fd, buffer, BUFFER_SIZE, 0);

            if (!read) {
                break;
            }

            if (read < 0) {
                on_error("Client read failed\n");
            }

			printf("received: '%s'\n", buffer);

			// echo received content back
			err = send(client_fd, buffer, read, 0);
            if (err < 0) {
                on_error("Client write failed\n");
            }
		}

		close(client_fd);
	}

	close(server_fd);
	return 0;
}

