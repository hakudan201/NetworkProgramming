/*UDP Echo Server*/
#include <stdio.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

// #define PORT 5550  /* Port that will be opened */
#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("No PortNumber specified");
		exit(1);
	}
	int *PORT = atoi(argv[1]);
	printf("Port number: %d\n", PORT);
	int server_sock; /* file descriptors */
	char buff[BUFF_SIZE];
	char temp[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;

	// Step 1: Construct a UDP socket
	if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{ /* calls socket() */
		perror("\nError: ");
		exit(0);
	}

	// Step 2: Bind address to socket
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);		 /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = INADDR_ANY; /* INADDR_ANY puts your IP address automatically */
	bzero(&(server.sin_zero), 8);		 /* zero the rest of the structure */

	if (bind(server_sock, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
	{ /* calls bind() */
		perror("\nError: ");
		exit(0);
	}
	printf("Done with binding\n");

	printf("Listening for incoming messages...\n\n");

	// Step 3: Communicate with clients
	while (1)
	{
		sin_size = sizeof(struct sockaddr_in);

		bytes_received = recvfrom(server_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&client, &sin_size);

		if (bytes_received < 0)
			perror("\nError: ");
		else
		{
			buff[bytes_received] = '\0';
			printf("[%s:%d]: %s", inet_ntoa(client.sin_addr), ntohs(client.sin_port), buff);
		}

		// bytes_sent = sendto(server_sock, "Your message is sent", 20, 0, (struct sockaddr *)&client, sin_size); /* send to the client welcome message */
		bytes_received = recvfrom(server_sock, temp, BUFF_SIZE - 1, 0, (struct sockaddr *)&client, &sin_size);
		int i = 0;
		char alp[100];
		char digit[100];
		int error = 0;
		strcpy(alp, "");
		strcpy(digit, "");
		while (buff[i] != '\0')
		{
			if ((buff[i] >= 'a' && buff[i] <= 'z') || (buff[i] >= 'A' && buff[i] <= 'Z'))
			{

				strncat(alp, &buff[i], 1);
			}
			else if (buff[i] >= '0' && buff[i] <= '9')
			{

				strncat(digit, &buff[i], 1);
			}
			else if (buff[i] == '\n' || buff[i] == " ")
			{
			}
			else
			{

				error = 1;
			}

			i++;
		}

		if (error == 1)
		{
			bytes_sent = sendto(server_sock, "!!!", 3, 0, (struct sockaddr *)&client, sin_size);
			bytes_sent = sendto(server_sock, "!!!", 3, 0, (struct sockaddr *)&client, sin_size);
		}
		bytes_sent = sendto(server_sock, alp, BUFF_SIZE, 0, (struct sockaddr *)&client, sin_size);

		bytes_sent = sendto(server_sock, digit, BUFF_SIZE, 0, (struct sockaddr *)&client, sin_size);
	}

	close(server_sock);
	return 0;
}
