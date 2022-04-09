#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

int main(int argc, char* argv[]) 
{ 
 char* ext;
 char* p;

    int status;
    DIR* dir = opendir("./hartakarun");
    if(dir == NULL){
        return 1;
    }

    struct dirent* entity;
    entity = readdir(dir);
    while(entity != NULL){

        char fullname[200];
        strcpy(fullname, entity->d_name);

        ext = strchr(fullname,'.');
        
        char temp[100];
        strcpy(temp, "./");
        strcat(temp, ext+1);
        printf("\n%s", temp);

        status = mkdir(temp, S_IRWXU);

        entity = readdir(dir);
    }

    closedir(dir);

   return 0;
}