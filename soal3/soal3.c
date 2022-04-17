#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <wait.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <pthread.h>
#include <limits.h>
#include <ctype.h>


pthread_t thread_ID[500];
char temp_file[2000][2000];

int is_regular(const char *path){
  struct stat reg_path_stat;
  stat(path, &reg_path_stat);
  return S_ISREG(reg_path_stat.st_mode);
}


void recursive_file(char *base_path, int *i)
{
  char path[1000];
  struct dirent *struct_dir;
  DIR *dir = opendir(base_path);

  while ((struct_dir = readdir(dir)) != NULL)
  {
    if (strcmp(struct_dir->d_name, "..") != 0 && strcmp(struct_dir->d_name, ".") != 0)
    {
        strcpy(path, base_path), strcat(path, "/"),strcat(path, struct_dir->d_name) ;

        if(is_regular(path)){
          strcpy(temp_file[*i], path);
          *i = *i + 1;
        }
        else{
          recursive_file(path, i);
        }
    }
  }
    closedir(dir);
}


void *move_file(void *arg){
  char str[999], temp[999], temp_2[999], temp_3[999], temp_4[999], path[1000], tempDest[1000], cwd[1000], file_name[1000], ext_1[1000], ext_2[1000];

  int isFile = is_regular(path);
  getcwd(cwd, sizeof(cwd)), strcpy(path, (char*) arg), strcpy(temp_4, path);
  
  char *ext_3, dot1 = '.';

  ext_3 = strchr(temp_4, dot1);
  strcpy(temp, path);

  char *token=strtok(temp, "/");
  do{
       sprintf(file_name, "%s", token);
      token = strtok(NULL, "/");
  }
  while(token != NULL);

  strcpy(temp, path), strcpy(temp_2, file_name), strcpy(temp_3, file_name);

  int total = 0;
  char *token2=strtok(temp_2, ".");

  sprintf(ext_2, "%s", token2);
    do
    {
      total++;

      sprintf(ext_1, "%s", token2);

      token2=strtok(NULL, ".");
    }

    while(token2 != NULL);

  char dot = '.', first = temp_3[0];

  if(dot == first) 
    {
     strcpy(ext_1, "Hidden");
    }

  else if(total >= 3)
      {
        strcpy(ext_1, ext_3+1);
      }

  else if (total <=1 )
      {
        strcpy(ext_1, "Unknown");
      }

  int i = 0;
  do
  {
    ext_1[i] = tolower(ext_1[i]);
    i++;
  }
  while(i < sizeof(ext_1));

  strcpy(temp, (char*) arg);
  mkdir(ext_1, 0777);
  strcat(cwd, "/"), strcat(cwd,ext_1),  strcat(cwd, "/"), strcat(cwd, file_name), strcpy(tempDest, cwd), rename(temp, tempDest);
  return (void *) 1;
}

int main(int argc, char *argv[]) {
  void *status;
  char base_path[1000], cwd[1000];
  int iter = 0;

    getcwd(cwd, sizeof(cwd));  strcpy(base_path, cwd); 
    recursive_file(base_path, &iter);
    pthread_t thread_ID[iter];
      int i = 0;
      do
      {
          pthread_create(&thread_ID[i], NULL, move_file, (void*)temp_file[i]);
          pthread_join(thread_ID[i], &status);
          i++;
      }
      while (i<iter);
      exit(EXIT_FAILURE);

}
