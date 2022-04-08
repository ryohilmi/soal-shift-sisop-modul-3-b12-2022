#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[5120] = {0};

    char c_username[50];
    char c_password[50];

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

    while (true)
    {
        read(new_socket, buffer, 5120);
        strcpy(c_username, buffer);

        FILE *users_file;
        users_file = fopen("users.txt", "a+");
        char currentline[100];

        bool user_exist = false;
        while (fgets(currentline, sizeof(currentline), users_file) != NULL)
        {
            char *username = strtok(currentline, ":");
            if (strcmp(username, c_username) == 0)
            {
                user_exist = true;
            }
        }

        if (user_exist)
        {
            send(new_socket, "exist", sizeof("exist"), 0);
        }
        else
        {
            send(new_socket, "regist", sizeof("regist"), 0);

            read(new_socket, buffer, 5120);
            strcpy(c_password, buffer);

            fprintf(users_file, "%s:%s\n", c_username, c_password);
            fclose(users_file);

            printf("User %s is registered", c_username);
        }
    }

    return 0;
}