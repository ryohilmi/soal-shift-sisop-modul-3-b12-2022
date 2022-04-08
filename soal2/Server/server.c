#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#define PORT 8080

int setup_sock();
void read_str();
void send_str();
void handle_register();
void handle_login();

int server_fd,
    new_socket, valread;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
char buffer[5120] = {0};

int main(int argc, char const *argv[])
{
    if (setup_sock() < 0)
        return -1;

    while (true)
    {
        read_str(buffer);
        if (strcmp(buffer, "register") == 0)
        {
            handle_register();
        }
        else if (strcmp(buffer, "login") == 0)
        {
            handle_login();
        }
    }

    return 0;
}

int setup_sock()
{
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
}

void send_str(char *str)
{
    send(new_socket, str, sizeof(str), 0);
}

void read_str(char *str)
{
    read(new_socket, str, sizeof(str));
}

void handle_register()
{
    while (true)
    {
        char c_username[50];
        char c_password[50];

        read(new_socket, buffer, 5120);
        strcpy(c_username, buffer);

        FILE *users_file;
        users_file = fopen("users.txt", "a+");
        char currentline[100];

        bool user_exist = false;
        while (fgets(currentline, sizeof(currentline), users_file) != NULL)
        {
            char *username = strtok(currentline, ":");
            char *password = strtok(NULL, ":");
            if (strcmp(username, c_username) == 0)
            {
                printf("%s", password);
                user_exist = true;
            }
        }

        if (user_exist)
        {
            send(new_socket, "exist", sizeof("exist"), 0);
            continue;
        }
        else
        {
            send(new_socket, "regist", sizeof("regist"), 0);

            read(new_socket, buffer, 5120);
            strcpy(c_password, buffer);

            fprintf(users_file, "%s:%s\n", c_username, c_password);
            fclose(users_file);

            printf("User %s is registered", c_username);
            break;
        }
    }
}

void handle_login()
{
    char c_username[50];
    char c_password[50];

    read(new_socket, buffer, 5120);
    strcpy(c_username, buffer);

    read(new_socket, buffer, 5120);
    strcpy(c_password, buffer);

    FILE *users_file;
    users_file = fopen("users.txt", "a+");
    char currentline[100];

    bool is_loggedin = false;
    while (fgets(currentline, sizeof(currentline), users_file) != NULL)
    {
        char *username = strtok(currentline, ":");
        char *password = strtok(NULL, ":");
        if (strcmp(username, c_username) == 0 && strcmp(password, c_password))
        {
            is_loggedin = true;
        }
    }
    fclose(users_file);

    if (is_loggedin)
    {
        send(new_socket, c_username, sizeof(c_username), 0);
        printf("User %s has logged in", c_username);
    }
    else
    {
        send(new_socket, "failed", sizeof("failed"), 0);
    }
}
