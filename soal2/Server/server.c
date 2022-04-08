#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#define PORT 8080

int setup_sock();
void send_str();
void handle_register();
void handle_login(char user[], bool *is_loggedin);
void handle_add(char user[]);
void copy_file(char source_file[], char target_file[]);

int server_fd,
    new_socket, valread;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
char client_path[] = "/home/ryohilmi/sisop/soal-shift-sisop-modul-3-b12-2022/soal2/Client";

int main(int argc, char const *argv[])
{
    char user[50];
    bool is_loggedin = false;

    if (setup_sock() < 0)
        return -1;

    FILE *database;
    database = fopen("problems.tsv", "a+");
    fclose(database);

    while (true)
    {
        char command[20] = {0};
        read(new_socket, command, 1024);

        if (strcmp(command, "register") == 0)
        {
            printf("register\n");
            handle_register();
        }
        else if (strcmp(command, "login") == 0)
        {
            printf("login\n");
            handle_login(user, &is_loggedin);
        }
        else if (strcmp(command, "add") == 0)
        {
            handle_add(user);
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
    send(new_socket, str, strlen(str), 0);
}

void handle_register()
{
    while (true)
    {
        char c_username[50] = {0};
        char c_password[50] = {0};

        read(new_socket, c_username, 1024);

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
            send(new_socket, "exist", strlen("exist"), 0);
            continue;
        }
        else
        {
            send(new_socket, "regist", strlen("regist"), 0);

            read(new_socket, c_password, 1024);

            fprintf(users_file, "%s:%s\n", c_username, c_password);
            fclose(users_file);

            printf("User %s is registered", c_username);
            break;
        }
    }
}

void handle_login(char user[], bool *is_loggedin)
{
    printf("DEBUG 1\n");
    char c_username[50] = {0};
    char c_password[50] = {0};

    printf("DEBUG 2\n");
    read(new_socket, c_username, 1024);
    printf("DEBUG 3\n");
    read(new_socket, c_password, 1024);

    printf("DEBUG 4\n");

    FILE *users_file;
    users_file = fopen("users.txt", "a+");
    char currentline[100];

    bool success = false;
    while (fgets(currentline, sizeof(currentline), users_file) != NULL)
    {
        char *username = strtok(currentline, ":");
        char *password = strtok(NULL, ":");

        if (strncmp(username, c_username, strlen(c_username)) == 0 && strncmp(password, c_password, strlen(c_password)) == 0)
        {
            success = true;
        }
    }
    fclose(users_file);

    if (success)
    {
        send(new_socket, c_username, sizeof(c_username), 0);
        printf("User %s has logged in\n", c_username);
        strcpy(user, c_username);
        *is_loggedin = true;
    }
    else
    {
        send(new_socket, "failed", sizeof("failed"), 0);
    }
}

void handle_add(char user[])
{
    char judul[50];
    char desc[150];
    char input[150];
    char output[150];

    char dir_buffer[200];
    char temp[50];
    FILE *database;

    bool done_judul = false;
    while (!done_judul)
    {
        printf("Judul problem: ");
        scanf("%s", judul);

        database = fopen("problems.tsv", "a+");
        char currentline[100];

        bool judul_exist = false;
        while (fgets(currentline, sizeof(currentline), database) != NULL)
        {
            char *cur_judul = strtok(currentline, "\t");
            if (strcmp(judul, cur_judul) == 0)
            {
                judul_exist = true;
                printf("Problem name is used\n");
            }
        }

        if (feof(database) && !judul_exist)
        {
            done_judul = true;
        }

        fclose(database);
    }

    printf("Filepath description.txt: ");
    scanf("%s", temp);
    sprintf(desc, "%s/%s", client_path, temp);

    printf("Filepath input.txt: ");
    scanf("%s", temp);
    sprintf(input, "%s/%s", client_path, temp);

    printf("Filepath output.txt: ");
    scanf("%s", temp);
    sprintf(output, "%s/%s", client_path, temp);

    database = fopen("problems.tsv", "a+");
    fprintf(database, "%s\t%s\n", judul, user);
    fclose(database);

    mkdir(judul, 0777);

    sprintf(dir_buffer, "%s/description.txt", judul);
    copy_file(desc, dir_buffer);
    sprintf(dir_buffer, "%s/input.txt", judul);
    copy_file(input, dir_buffer);
    sprintf(dir_buffer, "%s/output.txt", judul);
    copy_file(output, dir_buffer);
}

void copy_file(char source_file[], char target_file[])
{
    char ch;
    FILE *source,
        *target;

    source = fopen(source_file, "r");
    target = fopen(target_file, "w");

    if (target == NULL)
    {
        fclose(source);
        exit(EXIT_FAILURE);
    }

    while ((ch = fgetc(source)) != EOF)
        fputc(ch, target);

    fclose(source);
    fclose(target);
}
