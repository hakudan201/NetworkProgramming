/*UDP Echo Client*/
#include <stdio.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>

// #define SERV_PORT 5550
// #define SERV_IP "127.0.0.1"
#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{

	if (argc < 2)
	{
		printf("No hostname was provided to resolve the");
		exit(1);
	}
	char *SERV_IP = argv[1];
	int *SERV_PORT = atoi(argv[2]);

	printf("Client: %s\n", SERV_IP);
	printf("Server: %d\n", SERV_PORT);
	printf("client 1 send message first then \n");
	printf("client 2 enter '/' to receive message\n");
	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr;
	int bytes_sent, bytes_received, sin_size;

	// Step 1: Construct a UDP socket
	if ((client_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{ /* calls socket() */
		perror("\nError: ");
		exit(0);
	}

	// Step 2: Define the address of the server
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERV_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERV_IP);

	// Step 3: Communicate with server

	while (1)
	{
		printf("\nInsert string:");
		memset(buff, '\0', (strlen(buff) + 1));
		fgets(buff, BUFF_SIZE, stdin);

		sin_size = sizeof(struct sockaddr);
		bytes_sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *)&server_addr, sin_size);
		if (buff[0] == '\n')
		{
			printf("Error!\n");
			close(client_sock);
			break;
		}
		if (buff[0] == '/')
		{

			bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);
			buff[bytes_received] = '\0';
			if (strcmp(buff, "!!!") == 0)
			{
				printf("Error");
			}
			if (strcmp(buff, "") != 0 && strcmp(buff, "!!!") != 0)
			{
				printf("%s\n", buff);
			}
			bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);
			buff[bytes_received] = '\0';
			if (strcmp(buff, "") != 0 && strcmp(buff, "!!!") != 0)
			{
				printf("%s\n", buff);
			}
		}
	}
	// while (1)
	// {
	// 	bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);
	// 	if (bytes_received < 0)
	// 	{
	// 		perror("Error: ");
	// 		close(client_sock);
	// 		return 0;
	// 	}
	// 	buff[bytes_received] = '\0';
	// 	printf("Reply from server: %s", buff);
	// }
	close(client_sock);
	return 0;
}
