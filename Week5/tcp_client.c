#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5550
#define BUFF_SIZE 8192

int main()
{
	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received;
	int mark = 0;
	char flag1[BUFF_SIZE];
	char flag2[BUFF_SIZE];

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
	while (1)
	{
		// send message
		printf("\nPlease enter the login account:\n");
		memset(flag1, '\0', (strlen(flag1) + 1));
		fgets(flag1, BUFF_SIZE, stdin);
		msg_len = strlen(flag1) - 1;
		if (msg_len == 0)
			break;
		bytes_sent = send(client_sock, flag1, msg_len, 0);
		if (bytes_sent <= 0)
		{
			printf("\nConnection closed!\n");
			break;
		}

		// receive echo reply
		bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
		if (bytes_received <= 0)
		{
			printf("\nError!Cannot receive data from sever!\n");
			break;
		}
		buff[bytes_received] = '\0';
		if (strcmp(buff, "non_exist") == 0)
		{
			printf("Account not found!\n");
		}
		else if (strcmp(buff, "blocked") == 0)
		{
			printf("Account is blocked or inactive\n");
		}
		else
		{
			int num = 0;
			do
			{
				num++;
				printf("Please enter the login password:\n");
				memset(buff, '\0', (strlen(buff) + 1));
				fgets(buff, BUFF_SIZE, stdin);
				msg_len = strlen(buff) - 1;
				if (msg_len == 0)
					break;
				bytes_sent = send(client_sock, buff, msg_len, 0);
				if (bytes_sent <= 0)
				{
					printf("\nConnection closed!\n");
					break;
				}

				bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
				buff[bytes_received] = '\0';
				if (bytes_received <= 0)
				{
					printf("\nError!Cannot receive data from sever!\n");
					break;
				}

				if (strcmp(buff, "correct") == 0)
				{
					printf("Login is successful!\n");
					while (1)
					{
						printf("Enter 'Bye' to logout\n");
						memset(buff, '\0', (strlen(buff) + 1));
						fgets(buff, BUFF_SIZE, stdin);
						msg_len = strlen(buff) - 1;
						if (strcmp(buff, "Bye\n") == 0)
						{
							printf("Goodbye %s", flag1);
							break;
						}
					}
					mark = 1;
					break;
				}
				else if (strcmp(buff, "blocking") == 0)
				{
					printf("Account is blocked!\n");
					break;
				}
				else
				{
					printf("Password is not correct. Please try again!\n");
				}
			} while (num < 3);
		}
		if(mark == 1)
		break;
	}
	// Step 4: Close socket
	pclose(client_sock);
	return 0;
}
