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
#include <stdbool.h>

#define PORT 5550
#define BACKLOG 20
#define BUFF_SIZE 1024
#define INCORRECT "Password incorrect or account doesn't exist"
#define SIGNIN "Sign in successfully"
#define ENDOFFILE "End of file"
#define SEE_MESSAGE "See message"
#define SEND_MESSAGE "Send message"

/* Handler process signal*/
void sig_chld(int signo);

/*
 * Receive and echo message to client
 * [IN] sockfd: socket descriptor that connects to client
 */
void echo(int sockfd);

struct node
{
	char username[20];
	char password[20];
	int status;
	struct node *next;
};
// read linked list
struct node *readLinkedList(char filename[]);
// search user name
bool searchUsername(struct node *head, char *username);
// search password
bool searchPassword(struct node *head, char *password);
// send message log
void sendLogMessage(int sockfd);
// write message log
void wrileFile(char username[20], char chat[BUFF_SIZE]);

int main()
{
	int listen_sock, conn_sock; /* file descriptors */
	struct sockaddr_in server;	/* server's address information */
	struct sockaddr_in client;	/* client's address information */
	pid_t pid;
	int sin_size;

	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{ /* calls socket() */
		printf("socket() error\n");
		return 0;
	}

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY puts your IP address automatically */

	if (bind(listen_sock, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		perror("\nError: ");
		return 0;
	}

	if (listen(listen_sock, BACKLOG) == -1)
	{
		perror("\nError: ");
		return 0;
	}

	/* Establish a signal handler to catch SIGCHLD */
	signal(SIGCHLD, sig_chld);

	while (1)
	{
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size)) == -1)
		{
			if (errno == EINTR)
				continue;
			else
			{
				perror("\nError: ");
				return 0;
			}
		}

		/* For each client, fork spawns a child, and the child handles the new client */
		pid = fork();

		/* fork() is called in child process */
		if (pid == 0)
		{
			close(listen_sock);
			printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */
			echo(conn_sock);
			exit(0);
		}

		/* The parent closes the connected socket since the child handles the new client */
		close(conn_sock);
	}
	close(listen_sock);
	return 0;
}

void sig_chld(int signo)
{
	pid_t pid;
	int stat;

	/* Wait the child process terminate */
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("\nChild %d terminated\n", pid);
}

void echo(int sockfd)
{
	char buff[BUFF_SIZE];
	char username[20], password[20];
	int bytes_sent, bytes_received;
	struct node *newHead = readLinkedList("taikhoan.txt");

	bytes_received = recv(sockfd, buff, BUFF_SIZE, 0); // blocking
	if (bytes_received < 0)
		perror("\nError: ");
	else if (bytes_received == 0)
		printf("Connection closed.");
	buff[bytes_received] = '\0';

	sscanf(buff, "%s %s", username, password);

	if (searchUsername(newHead, username) == false || searchPassword(newHead, password) == false)
	{
		bytes_sent = send(sockfd, INCORRECT, strlen(INCORRECT), 0);
	}
	else
	{
		bytes_sent = send(sockfd, SIGNIN, strlen(SIGNIN), 0);
		bytes_received = recv(sockfd, buff, BUFF_SIZE, 0); // blocking
		if (bytes_received < 0)
			perror("\nError: ");
		else if (bytes_received == 0)
			printf("Connection closed.");
		buff[bytes_received] = '\0';
		if (strcmp(buff, SEE_MESSAGE) == 0)
		{
			sendLogMessage(sockfd);
		}
		else
		{
			wrileFile(username, buff);
		}

	} /* echo to the client */
	if (bytes_sent < 0)
		perror("\nError: ");

	close(sockfd);
}

struct node *readLinkedList(char filename[])
{
	struct node *temp = (struct node *)malloc(sizeof(struct node));
	struct node *head;
	struct node *last;
	last = head = NULL;
	FILE *file;
	file = fopen(filename, "r");

	if (file == NULL)
	{
		fprintf(stderr, "\nCouldn't Open File'\n");
		exit(1);
	}
	while (fscanf(file, "%s %s", temp->username, temp->password) != EOF)
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

void sendLogMessage(int sockfd)
{
	int bytes_sent;
	char const *const fileName = "groupchat.txt";

	FILE *file = fopen(fileName, "r");

	char line[500];

	while (fgets(line, sizeof(line), file))
	{
		bytes_sent = send(sockfd, line, strlen(line), 0);
	}
	fclose(file);
};

void wrileFile(char username[20], char chat[BUFF_SIZE])
{
	FILE *fptr;
	fptr = fopen("groupchat.txt", "a");
	char text_to_append[BUFF_SIZE];
	strcpy(text_to_append, username);
	strcat(text_to_append, ": ");
	strcat(text_to_append, chat);
	strcat(text_to_append, "\n");
	fputs(text_to_append, fptr);

	fclose(fptr);
}
