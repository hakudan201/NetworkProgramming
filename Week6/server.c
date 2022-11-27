#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#define PORT 5550   /* Port that will be opened */
#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 1024
#define MESSAGE1 "File Already Exists!"
#define MESSAGE2 "File Transfer Is Completed..."
#define MESSAGE3 "Error: File Not Found!"
#define MESSAGE4 "Error: Wrong File Format!"

typedef struct File {
    char filename[50];
    char name[20];
    char type[20];
    char path[50];
} File;

typedef struct node {
    File infor;
    struct node* next;
} node;

struct node* head = NULL;
struct node* current = NULL;

void insertFirst(File file) {
    struct node* link = (struct node*)malloc(sizeof(struct node));


    strcpy(link->infor.filename, file.filename);
    strcpy(link->infor.name, file.name);
    strcpy(link->infor.type, file.type);
    strcpy(link->infor.path, file.path);

    link->next = head;
    head = link;
}

struct node* deleteFirst() {
    struct node* tempLink = head;
    head = head->next;
    return tempLink;
}

bool isEmpty() {
    return head == NULL;
}

void readFile()
{
    FILE* file;
    file = fopen("database.txt", "r");
    File temp;
    if (file == NULL)
    {
        fprintf(stderr, "\nCouldn't Open File'\n");
        exit(1);
    }
    while (fscanf(file, "%s %s", temp.filename, temp.path) != EOF)
    {
        int j;
        for (int i = 0; i < strlen(temp.filename); i++)
        {
            if (temp.filename[i] == '.') {
                j = i;
            }
        }
        strncpy(temp.name, temp.filename, j);
        for (int i = j + 1, k = 0; i < strlen(temp.filename); i++, k++) {
            temp.type[k] = temp.filename[i];
        }
        //printf("name: %s length:%d, type: %s\n", temp.name, strlen(temp.name), temp.type);
        insertFirst(temp);
    }
    fclose(file);
}

node* checkFilename(char* filename)
{
    node* temp = head;
    while (temp != NULL)
    {
        if (strncmp(temp->infor.filename, filename, strlen(temp->infor.filename)) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

node* checkName(char* name) {
    node* temp = head;
    while (temp != NULL)
    {
        if (strncmp(temp->infor.name, name, strlen(name)) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

int main()
{
    readFile();
    int listen_sock, conn_sock; /* file descriptors */
    char recv_data[BUFF_SIZE];
    int bytes_sent, bytes_received;
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client; /* client's address information */
    int sin_size;

    //Step 1: Construct a TCP socket to listen connection request
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  /* calls socket() */
        perror("\nError: ");
        return 0;
    }

    //Step 2: Bind address to socket
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);   /* Remember htons() from "Conversions" section? =) */
    server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */
    if (bind(listen_sock, (struct sockaddr*)&server, sizeof(server)) == -1) { /* calls bind() */
        perror("\nError: ");
        return 0;
    }

    //Step 3: Listen request from client
    if (listen(listen_sock, BACKLOG) == -1) {  /* calls listen() */
        perror("\nError: ");
        return 0;
    }

    //Step 4: Communicate with client
    //accept request
    sin_size = sizeof(struct sockaddr_in);
    if ((conn_sock = accept(listen_sock, (struct sockaddr*)&client, &sin_size)) == -1) {
        perror("\nError: ");
    }

    printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */

    //receives message from client
    bytes_received = recv(conn_sock, recv_data, BUFF_SIZE - 1, 0); //blocking
    if (bytes_received <= 0) {
        printf("\nConnection closed");
    }
    else {
        recv_data[bytes_received] = '\0';
        printf("\nReceive: %s \n", recv_data);
        //echo to client
        if (checkFilename(recv_data) != NULL) {
            bytes_sent = send(conn_sock, MESSAGE2, strlen(MESSAGE2), 0); /* send to the client welcome message */
            if (bytes_sent <= 0) {
                printf("\nConnection closed");
            }
            //printf("Getting Picture Size\n");
            FILE* picture;
            if ((picture = fopen(checkFilename(recv_data)->infor.path, "r")) == NULL)
            {
                perror("Error opening file picture");
                exit(1);
            }
            int size;
            fseek(picture, 0, SEEK_END);
            size = ftell(picture);
            fseek(picture, 0, SEEK_SET);

            //Send Picture Size
            //printf("Sending Picture Size: %d\n", size);


            //Send Picture as Byte Array
            //printf("Sending Picture as Byte Array\n");
            char send_buffer[size];

            while (!feof(picture)) {
                fread(send_buffer, 1, sizeof(send_buffer), picture);
                //printf("Send_buffer: %s, Length:%d \n", send_buffer, strlen(send_buffer));
                bytes_sent = send(conn_sock, send_buffer, size, 0); /* send to the client welcome message */
                if (bytes_sent <= 0) {
                    printf("\nConnection closed");
                }
            }
            fclose(picture);
        }
        else {
            char* b = malloc(strlen(recv_data));
            char* c = malloc(strlen(recv_data));
            int j;
            for (int i = 0; i < strlen(recv_data); i++)
            {
                if (recv_data[i] == '.') {
                    j = i;
                }
            }
            for (int i = j + 1, k = 0; i < strlen(recv_data); i++, k++) {
                c[k] = recv_data[i];
            }
            strncpy(b, recv_data, j);
            //printf("b:%s length: %d, c: %s", b, strlen(b), c);
            if (checkName(b) != NULL)
            {
                bytes_sent = send(conn_sock, MESSAGE4, strlen(MESSAGE4), 0); /* send to the client welcome message */
                if (bytes_sent <= 0) {
                    printf("\nConnection closed");
                }
            }
            else {
                bytes_sent = send(conn_sock, MESSAGE3, strlen(MESSAGE3), 0); /* send to the client welcome message */
                if (bytes_sent <= 0) {
                    printf("\nConnection closed");
                }
            }
            free(b);
            free(c);

        }

    }
    close(conn_sock);
    close(listen_sock);
    return 0;
}
