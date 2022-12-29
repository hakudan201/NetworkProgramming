#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
    if (argc < 3 || argc > 3 || (strcmp(argv[1], "127.0.0.1")) != 0 || (strcmp(argv[2], "5500")) != 0)
    {
        fprintf(stderr, "Please provide information as the format\n");
        fprintf(stderr, "Usage : ./client <127.0.0.1> <5500>\n");
        exit(1);
    }
    else
    {
        // create a socket
        int network_socket;
        network_socket = socket(AF_INET, SOCK_STREAM, 0);

        // specify an address for the socket
        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(9999);
        server_address.sin_addr.s_addr = INADDR_ANY;

        int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));
        // check for connection_status
        if (connection_status == -1)
        {
            printf("The connection has error\n\n");
        }

        if (connection_status == 0)
        {
            char response[256];
            int count = 0;
            // receive data from the server
            recv(network_socket, response, sizeof(response), 0);
            printf("%s", response);
            while (1)
            {
                // char response[256];
                printf("enter a message to echo\n");
                memset(response, '\0', 256);
                fgets(response, 256, stdin);
                int send_status = send(network_socket, response, strlen(response) - 1, 0);
                count += strlen(response);
                if ((strcmp(response, "q") == 0) || (strcmp(response, "Q") == 0))
                {
                    printf("Bytes sent to server: %d", count);
                    break;
                }
                // print out the server's response
                recv(network_socket, response, sizeof(response), 0);
                printf("Here is the echo message from the server: %s\n", response);
            }
        }
        // close the socket
        close(network_socket);

        return 0;
    }
}
