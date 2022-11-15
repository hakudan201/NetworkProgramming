#include <stdio.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#define PORT 5550 /* Port that will be opened */
#define BACKLOG 2 /* Number of allowed connections */
#define BUFF_SIZE 1024

struct node
{
	char username[20];
	char password[20];
	int status;
	struct node *next;
};

void wrileFile(struct node *root)
{
	FILE *outfile;
	outfile = fopen("account.txt", "w");
	struct node *temp = root;
	while (temp != NULL)
	{

		fprintf(outfile, "%s %s %d\n", temp->username, temp->password, temp->status);
		temp = temp->next;
	}
	fclose(outfile);
}

struct node *readLinkedList(char filename[])
{
	struct node *temp = (struct node *)malloc(sizeof(struct node));
	struct node *head;
	struct node *last;
	last = head = NULL;
	FILE *file;
	file = fopen("account.txt", "r");

	if (file == NULL)
	{
		fprintf(stderr, "\nCouldn't Open File'\n");
		exit(1);
	}
	while (fscanf(file, "%s %s %d", temp->username, temp->password, &temp->status) != EOF)
	{
		if (head == NULL)
		{
			head = last = (struct node *)malloc(sizeof(struct node));
		}
		else
		{
			last->next = (struct node *)malloc(sizeof(struct node));
			last = last->next;
		}
		strcpy(last->username, temp->username);
		strcpy(last->password, temp->password);
		last->status = temp->status;
		last->next = NULL;
	}
	fclose(file);
	return head;
}

bool searchUsername(struct node *head, char *username)
{
	struct node *current = head; // Initialize current
	while (current != NULL)
	{
		if (strcmp(current->username, username) == 0)
			return true;
		current = current->next;
	}
	return false;
}

bool searchPassword(struct node *head, char *password)
{
	struct node *current = head; // Initialize current
	while (current != NULL)
	{
		if (strcmp(current->password, password) == 0)
			return true;
		current = current->next;
	}
	return false;
}

bool checkBlock(struct node *head, char *username)
{
	struct node *current = head; // Initialize current
	while (current != NULL)
	{
		if (strcmp(current->username, username) == 0)
			return !current->status;
		current = current->next;
	}
	return false;
}

void changeStatus(struct node *head, char *username)
{
	struct node *current = head; // Initialize current
	while (current != NULL)
	{
		if (strcmp(current->username, username) == 0)
			current->status = 0;
		current = current->next;
	}
	wrileFile(head);
}

int main()
{

	int listen_sock, conn_sock; /* file descriptors */
	char recv_data[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;

	char flag1[BUFF_SIZE], flag2[BUFF_SIZE];
	struct node *newHead = readLinkedList("account.txt");
	struct node *signedInAccounts = NULL;
	bool check = false;

	// Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{ /* calls socket() */
		perror("\nError: ");
		return 0;
	}

	// Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);				/* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY puts your IP address automatically */
	if (bind(listen_sock, (struct sockaddr *)&server, sizeof(server)) == -1)
	{ /* calls bind() */
		perror("\nError: ");
		return 0;
	}

	// Step 3: Listen request from client
	if (listen(listen_sock, BACKLOG) == -1)
	{ /* calls listen() */
		perror("\nError: ");
		return 0;
	}

	// Step 4: Communicate with client
	while (1)
	{
		// accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size)) == -1)
			perror("\nError: ");

		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */

		// start conversation
		while (1)
		{
			// receives message from client
			bytes_received = recv(conn_sock, flag1, BUFF_SIZE - 1, 0); // blocking

			if (bytes_received <= 0)
			{
				printf("\nConnection closed");
				break;
			}
			else
			{
				flag1[bytes_received] = '\0';
				if (searchUsername(newHead, flag1) == false)
				{
					printf("\nReceive: %s ", flag1);
					bytes_sent = send(conn_sock, "non_exist", 9, 0); /* send to the client welcome message */
					if (bytes_sent <= 0)
					{
						printf("\nConnection closed");
						break;
					}
				}
				else if (checkBlock(newHead, flag1) == true)
				{
					printf("\nReceive: %s ", flag1);
					bytes_sent = send(conn_sock, "blocked", 7, 0); /* send to the client welcome message */
					if (bytes_sent <= 0)
					{
						printf("\nConnection closed");
						break;
					}
				}
				else
				{
					printf("\nReceive: %s ", flag1);
					bytes_sent = send(conn_sock, "correct", 6, 0); /* send to the client welcome message */
					if (bytes_sent <= 0)
					{
						printf("\nConnection closed");
						break;
					}

					int num = 0;
					do
					{
						bytes_received = recv(conn_sock, flag2, BUFF_SIZE - 1, 0); // blocking
						if (bytes_received <= 0)
						{
							printf("\nConnection closed");
							break;
						}
						flag2[bytes_received] = '\0';

						if (searchPassword(newHead, flag2) == false)
						{
							num++;
							if (num < 3)
							{
								bytes_sent = send(conn_sock, "incorrect", 9, 0); /* send to the client welcome message */
								if (bytes_sent <= 0)
								{
									printf("\nConnection closed");
									break;
								}
							}
							else
							{
								bytes_sent = send(conn_sock, "blocking", 8, 0); /* send to the client welcome message */
								if (bytes_sent <= 0)
								{
									printf("\nConnection closed");
									break;
								}
								changeStatus(newHead, flag1);
								break;
							}
						}
					} while (searchPassword(newHead, flag2) == false);
					if (num < 3)
					{
						bytes_sent = send(conn_sock, "correct", 7, 0); /* send to the client welcome message */
						if (bytes_sent <= 0)
						{
							printf("\nConnection closed");
							break;
						}
					}
				}
			}

		} // end conversation
		close(conn_sock);
	}

	close(listen_sock);
	return 0;
}
