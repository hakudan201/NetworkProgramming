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

void send_file(FILE *fp, int sockfd)
{
    char data[BUFF_SIZE] = {0};

    while (fgets(data, BUFF_SIZE, fp) != NULL)
    {
        if (send(sockfd, data, sizeof(data), 0) == -1)
        {
            perror("[-]Error in sending file.");
            exit(1);
        }
        bzero(data, BUFF_SIZE);
    }
}

int main(int argc, char *argv[])
{

    if (argc < 3 || argc > 3 || (strcmp(argv[1], "127.0.0.1")) != 0 || (strcmp(argv[2], "5500")) != 0)
    {
        fprintf(stderr, "Please provide the IP/hostname and port of the server\n");
        fprintf(stderr, "Usage : ./client <127.0.0.1> <5500>\n");
        exit(1);
    }
    else
    {
        int client_sock;
        char buff[BUFF_SIZE + 1];
        struct sockaddr_in server_addr; /* server's address information */
        int msg_len, bytes_sent, bytes_received;
        FILE *fp;

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
        // take file path
        printf("\nInsert file path:\n");
        memset(buff, '\0', (strlen(buff) + 1));
        fgets(buff, BUFF_SIZE, stdin);
        msg_len = strlen(buff);
        buff[msg_len - 1] = '\0';
        fp = fopen(buff, "r");
        if (fp == NULL)
        {
            perror("[-]Error in reading file.");
            exit(1);
        }
        bytes_sent = send(client_sock, buff, strlen(buff), 0);
        if (bytes_sent < 0)
            perror("\nError: ");

        // receive echo reply
        bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
        if (bytes_received < 0)
            perror("\nError: ");
        else if (bytes_received == 0)
            printf("Connection closed.\n");

        buff[bytes_received] = '\0';
        printf("Size of file sent: %s byte(s)\n", buff);

        // send data

        send_file(fp, client_sock);

        // Step 4: Close socket
        close(client_sock);
        return 0;
    }
}
