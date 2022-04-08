#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>
#include <ctype.h>

#define PORT 8080
#define HOST "127.0.0.1"

int setup_sock();
void clear_screen();
void send_str();
void read_str();
bool check_password();
void register_user();
void login(char user[], bool *is_loggedin);

struct sockaddr_in address;
int sock = 0, valread;
struct sockaddr_in serv_addr;
char buffer[1024] = {0};

int main(int argc, char const *argv[])
{
    char user[50];
    bool is_loggedin = false;

    if (setup_sock() < 0)
        return -1;

    while (true)
    {
        if (is_loggedin)
        {
            char command[20];
            printf("CMD: ");
            scanf("%s", command);

            if (strcmp(command, "add") == 0)
            {
                send_str("add");
            }
        }
        else
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
                send_str("register");
                register_user();
            }
            else if (strcmp(input, "2") == 0)
            {
                send_str("login");
                login(user, &is_loggedin);
            }
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
    send(sock, str, strlen(str), 0);
}

void read_str(char *str)
{
    read(sock, str, sizeof(str));
}

bool check_password(char *password)
{
    const char *p = password;
    char c;
    int chars = 0;
    int nupper = 0;
    int nlower = 0;
    int ndigit = 0;
    while (*p)
    {
        c = *p++;
        chars++;
        if (isupper(c))
            nupper++;
        else if (islower(c))
            nlower++;
        else if (isdigit(c))
            ndigit++;
        else
            continue;
    }

    return chars > 5 && nupper && nlower && ndigit;
}

void register_user()
{
    char username[50] = {0};
    char password[50] = {0};

    bool done_register = false;

    while (!done_register)
    {
        printf("Username: ");
        scanf("%s", username);
        printf("Password: ");
        scanf("%s", password);

        if (!check_password(password))
        {
            clear_screen();
            printf("Invalid password format\n");
            continue;
        }

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

void login(char user[], bool *is_loggedin)
{
    char username[50] = {0};
    char password[50] = {0};

    bool done_login = false;

    while (!done_login)
    {
        printf("Username: ");
        scanf("%s", username);
        printf("Password: ");
        scanf("%s", password);

        if (!check_password(password))
        {
            clear_screen();
            printf("Invalid password format\n");
            continue;
        }

        send_str(username);
        send_str(password);

        read_str(buffer);

        clear_screen();
        if (strcmp(buffer, "failed") == 0)
        {
            printf("Login failed\n");
        }
        else
        {
            printf("Login user %s success\n", buffer);
            strcpy(user, buffer);
            *is_loggedin = true;
        }
        done_login = true;
    }
}

int setup_sock()
{
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
}
