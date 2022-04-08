#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>
#include <regex.h>

#define PORT 8080
#define HOST "127.0.0.1"

void clear_screen();
void send_str();
void read_str();

struct sockaddr_in address;
int sock = 0, valread;
struct sockaddr_in serv_addr;
char buffer[1024] = {0};

int main(int argc, char const *argv[])
{
    char username[50];
    char password[50];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, HOST, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    while (true)
    {
        printf("===== Welcome to Bluemary Online Judge =====\n");
        printf("Menu:\n");
        printf("1. Register\n");
        printf("2. Login\n");

        char input[10];
        scanf("%s", input);

        clear_screen();

        if (strcmp(input, "1") == 0)
        {
            bool done_register = false;
            while (!done_register)
            {
                printf("Username: ");
                scanf("%s", username);
                printf("Password: ");
                scanf("%s", password);

                send_str(username);

                read_str(buffer);
                if (strcmp(buffer, "exist") == 0)
                {
                    clear_screen();
                    printf("Username is already registered\n");
                    continue;
                }

                send_str(password);

                clear_screen();
                printf("Register success\n");
                done_register = true;
            }
        }
        else if (strcmp(input, "2") == 0)
        {
            ;
        }
    }

    return 0;
}

void clear_screen()
{
    printf("\e[1;1H\e[2J");
}

void send_str(char *str)
{
    send(sock, str, sizeof(str), 0);
}

void read_str(char *str)
{
    read(sock, str, sizeof(str));
}