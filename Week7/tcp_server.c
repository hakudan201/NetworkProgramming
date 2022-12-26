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
#include <ctype.h>
#include <libgen.h>

#define PORT 5550
#define BACKLOG 20
#define BUFF_SIZE 1024

/* Handler process signal*/
void sig_chld(int signo);

/*
 * Receive and echo message to client
 * [IN] sockfd: socket descriptor that connects to client
 */
void echo(int sockfd);

void write_file(int sockfd, char *filename);

// function get file size
long get_file_size(char *);

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
			// close(listen_sock);
			printf("You got a connection from %s\n\n", inet_ntoa(client.sin_addr)); /* prints client's IP */
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
	int bytes_sent, bytes_received;
	char *filename;
	char result[BUFF_SIZE];
	FILE *fp;

	bytes_received = recv(sockfd, buff, BUFF_SIZE, 0); // blocking
	if (bytes_received < 0)
		perror("\nError: ");
	else if (bytes_received == 0)
		printf("Connection closed.");
	// extract file name
	filename = buff;

	char *ts2 = strdup(filename);
	char *name = basename(ts2);


	sprintf(result, "%ld", get_file_size(buff));
	bytes_sent = send(sockfd, result, strlen(result), 0); /* echo to the client */
	if (bytes_sent < 0)
		perror("\nError: ");
	buff[bytes_sent] = '\0';

	// upcase the filename
	for (int i = 0; name[i] != '\0'; i++)
	{
		if (name[i] >= 'a' && name[i] <= 'z')
		{
			name[i] = name[i] - 32;
		}
	}
	printf("Sending back file: %s\n", name);
	write_file(sockfd, name);

	close(sockfd);

}

long get_file_size(char *filename)
{
	FILE *fp = fopen(filename, "r");

	if (fp == NULL)
		return -1;

	if (fseek(fp, 0, SEEK_END) < 0)
	{
		fclose(fp);
		return -1;
	}

	long size = ftell(fp);
	// release the resources when not required
	fclose(fp);
	return size;
}

void write_file(int sockfd, char *filename)
{
	int n;
	FILE *fp;
	char buffer[BUFF_SIZE];

	fp = fopen(filename, "w");
	while (1)
	{
		n = recv(sockfd, buffer, BUFF_SIZE, 0);
		if (n <= 0)
		{
			break;
			return;
		}
		// upcase the file contents
		for (int i = 0; buffer[i] != '\0'; i++)
		{
			if (buffer[i] >= 'a' && buffer[i] <= 'z')
			{
				buffer[i] = buffer[i] - 32;
			}
		}
		fprintf(fp, "%s", buffer);
		bzero(buffer, BUFF_SIZE);
	}
	fclose(fp);
	return;
}
