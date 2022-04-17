#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     
#include <arpa/inet.h>
#include <sys/types.h>  
#include <sys/socket.h> 
#include <sys/stat.h>   
#include <fcntl.h>      
#include <errno.h>

#define PORT    5500
#define MAXBUF  1024

int main() {
    int server_sock;
    int client_sock;
    int des; 
    struct sockaddr_in server_addr, client_addr;
    int client_len, read_len, fileRead_len;   
    char buf[MAXBUF];

    int check_bind;
    client_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock == -1) {
        perror("socket error : ");
        exit(0);
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family       = AF_INET;
    server_addr.sin_addr.s_addr  = htonl(INADDR_ANY);
    server_addr.sin_port         = htons(PORT);

    if(bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) > 0) {
        perror("bind error : ");
        exit(0);
    }

    if(listen(server_sock, 5) != 0) {
        perror("listen error : ");
    }

    while(1) {
        char file_name[MAXBUF]; 
        memset(buf, 0x00, MAXBUF);

        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        printf("New Client Connect : %s\n", inet_ntoa(client_addr.sin_addr));

        read_len = read(client_sock, buf, MAXBUF);
        if(read_len > 0) {
            strcpy(file_name, buf);
            printf("%s > %s\n", inet_ntoa(client_addr.sin_addr), file_name);
        } else {
            close(client_sock);
            break;
        }

        des = open(file_name, O_WRONLY | O_CREAT | O_EXCL, 0700);
        if(!des) {
            perror("file open error : ");
            break;
        }   
        while(1) {
            memset(buf, 0x00, MAXBUF);
            fileRead_len = read(client_sock, buf, MAXBUF);
            write(des, buf, fileRead_len);
            if(fileRead_len == EOF | fileRead_len == 0) {
                printf("finish file\n");
                break;
            }
        }

        close(client_sock);
        close(des);
    }
    close(server_sock);
    return 0;
}
