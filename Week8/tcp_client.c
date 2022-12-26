#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <errno.h>
#include <libgen.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5550
#define BUFF_SIZE 1024
#define SEE_MESSAGE "See message"
#define SEND_MESSAGE "Send message"

int main(int argc, char *argv[])
{

	if (argc < 5 || argc > 5 || (strcmp(argv[1], "127.0.0.1")) != 0 || (strcmp(argv[2], "5500")) != 0)
	{
		fprintf(stderr, "Please provide information as the format\n");
		fprintf(stderr, "Usage : ./client <127.0.0.1> <5500> <username> <password>\n");
		exit(1);
	}
	else
	{
		int client_sock;
		char buff[BUFF_SIZE + 1];
		struct sockaddr_in server_addr; /* server's address information */
		int msg_len, bytes_sent, bytes_received;

		char username[20], password[20];
		strcpy(username, argv[3]);
		strcpy(password, argv[4]);
		int namelen = strlen(username), passlen = strlen(password);

		// Step 1: Construct socket
		client_sock = socket(AF_INET, SOCK_STREAM, 0);

		// Step 2: Specify server address
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(SERVER_PORT);
		server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

		// Step 3: Request to connect server
		if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
		{
			printf("\nError!Can not connect to sever! Client exit imediately! ");
			return 0;
		}

		// Step 4: Communicate with server

		// send message
		// printf("\nInsert string to send:");
		// memset(buff, '\0', (strlen(buff) + 1));
		// fgets(buff, BUFF_SIZE, stdin);

		strcpy(buff, username);
		strcat(buff, " ");
		strcat(buff, password);
		msg_len = strlen(buff);

		bytes_sent = send(client_sock, buff, msg_len, 0);
		if (bytes_sent < 0)
			perror("\nError: ");

		// receive echo reply
		bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
		if (bytes_received < 0)
			perror("\nError: ");
		else if (bytes_received == 0)
			printf("Connection closed.\n");

		buff[bytes_received] = '\0';
		printf("\n%s\n\n", buff);

		while (1)
		{
			int choice = -1;
			printf("1. See message log \n");
			printf("2. Send message\n");
			printf("Your choice (1-2, other to quit):  ");
			scanf("%d", &choice);
			switch (choice)
			{
			case 1:
				bytes_sent = send(client_sock, SEE_MESSAGE, strlen(SEE_MESSAGE), 0);
				if (bytes_sent < 0)
					perror("\nError: ");

				bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
				if (bytes_received < 0)
					perror("\nError: ");
				else if (bytes_received == 0)
					printf("Connection closed.\n");

				buff[bytes_received] = '\0';
				printf("%s\n", buff);

				exit(0);
			case 2:
				printf("Enter message:\n");
				while (1)
				{
					memset(buff, '\0', (strlen(buff) + 1));
					fgets(buff, BUFF_SIZE, stdin);
					msg_len = strlen(buff) - 1;
					if (msg_len > 0)
					{
						break;
					}
				}
				bytes_sent = send(client_sock, buff, msg_len, 0);
				if (bytes_sent < 0)
					perror("\nError: ");
				exit(0);
			default:
				printf("Goodbye!");
				exit(0);
			}
		}
		// Step 4: Close socket
		close(client_sock);
		return 0;
	}
}
