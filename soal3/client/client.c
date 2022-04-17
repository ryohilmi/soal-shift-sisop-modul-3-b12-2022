#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT    5500
#define IP      "127.0.0.1"
#define MAXBUF  1024

int main() {
    struct sockaddr_in server_addr;
    int     s;
    int         sourse_fd;
    char        buf[MAXBUF];
    int         file_name_len, read_len;
    /* socket() */
    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s == -1) {
        return 1;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP);
    server_addr.sin_port = htons(PORT);

    if(connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect : ");
        printf("fail to connect.\n");
        close(s);
        return 1;
    }

    memset(buf, 0x00, MAXBUF);
    printf("write file name to send to the server:  ");
    scanf("%s", buf);

    printf(" > %s\n", buf);
    file_name_len = strlen(buf);

    send(s, buf, file_name_len, 0);
    sourse_fd = open(buf, O_RDONLY);
    if(!sourse_fd) {
        perror("Error : ");
        return 1;
    }

    while(1) {
        memset(buf, 0x00, MAXBUF);
        read_len = read(sourse_fd, buf, MAXBUF);
        send(s, buf, read_len, 0);
        if(read_len == 0) {
            break;
        }

    }

    return 0;
}