#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
int input();

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

struct node *insertAtBegin(struct node *head, char *username, char *password)
{
    struct node *ptr;
    struct node *temp = (struct node *)malloc(sizeof(struct node));

    strcpy(temp->username, username);
    strcpy(temp->password, password);
    temp->status = 1;
    temp->next = NULL;

    if (head == NULL)
    {
        head = temp;
    }
    else
    {
        temp->next = head;
        head = temp;
    }

    wrileFile(head);
    return head;
}

struct node *signIn(struct node *head, char *username, char *password)
{
    struct node *ptr;
    struct node *temp = (struct node *)malloc(sizeof(struct node));

    strcpy(temp->username, username);
    strcpy(temp->password, password);
    temp->status = 1;
    temp->next = NULL;

    if (head == NULL)
    {
        head = temp;
    }
    else
    {
        temp->next = head;
        head = temp;
    }
    return head;
}

void signOut(struct node **head_ref, char *username)
{
    // Store head node
    struct node *temp = *head_ref, *prev;

    // If head node itself holds the key to be deleted
    if (temp != NULL && (strcmp(temp->username, username) == 0))
    {
        *head_ref = temp->next; // Changed head
        free(temp);             // free old head
        return;
    }

    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && (strcmp(temp->username, username) == 0))
    {
        prev = temp;
        temp = temp->next;
    }
    // Unlink the node from linked list
    prev->next = temp->next;

    free(temp); // Free memory
}

int input()
{
    int number;
    scanf("%d", &number);
    return (number);
}

int main()
{
    int choice;
    char flag1[20], flag2[20];
    struct node *newHead = readLinkedList("account.txt");
    struct node *signedInAccounts = NULL;
    bool check = false;
    while (1)
    {
        printf("USER MANAGEMENT PROGRAM\n");
        printf("---------------------------------------\n");
        printf("1. Register\n");
        printf("2. Sign in\n");
        printf("3. Search\n");
        printf("4. Sign out\n");
        printf("Your choice (1-4, other to quit):\n");
        choice = input();

        switch (choice)
        {
        case 1:
        {

            fflush(stdin);
            printf("Enter your username:");
            gets(flag1);
            if (searchUsername(newHead, flag1) == true)
            {
                printf("Account existed\n\n");
                break;
            }
            else
                printf("Enter your password:");

            fflush(stdin);
            gets(flag2);
            printf("Seccessful registration\n\n");
            newHead = insertAtBegin(newHead, flag1, flag2);
            break;
        }
        case 2:
        {
            fflush(stdin);
            printf("Enter your username:");
            gets(flag1);
            if (searchUsername(newHead, flag1) == false)
            {
                printf("Cannot find account\n\n");
                break;
            }
            else if (checkBlock(newHead, flag1) == true)
            {
                printf("Account is blocked\n\n");
                break;
            }
            else
            {
                int num = 0;
                do
                {
                    printf("Enter your password:");
                    fflush(stdin);
                    gets(flag2);
                    if (searchPassword(newHead, flag2) == false)
                    {
                        num++;
                        if (num < 3)
                            printf("Password is incorrect\n");
                        else
                        {
                            printf("Password is incorrect. Account is blocked\n\n");
                            changeStatus(newHead, flag1);
                            break;
                            ;
                        }
                    }
                } while (searchPassword(newHead, flag2) == false);

                if (num < 3)
                {
                    signedInAccounts = signIn(signedInAccounts, flag1, flag2);
                    printf("Hello hust\n\n");
                }
            }
            break;
        }
        case 3:
        {
            printf("Search by username: ");
            fflush(stdin);
            gets(flag1);
            if (searchUsername(newHead, flag1) == false)
            {
                printf("Cannot find account\n\n");
            }
            else if (checkBlock(newHead, flag1) == true)
            {
                printf("Account is blocked\n\n");
            }
            else
                printf("Account is active\n\n");
            break;
        }
        case 4:
        {
            printf("Sign out by username: ");
            fflush(stdin);
            gets(flag1);
            if (searchUsername(newHead, flag1) == false)
            {
                printf("Cannot find account\n\n");
            }
            else if (searchUsername(signedInAccounts, flag1) == false)
            {
                printf("Account is not sign in \n\n");
            }
            else
            {
                printf("Goodbye %s\n\n", flag1);
                signOut(&signedInAccounts, flag1);
            }
            break;
        }
        break;
        default:
            exit(0);
        }
    }
}
