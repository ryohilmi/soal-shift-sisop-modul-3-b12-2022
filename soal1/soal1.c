#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/wait.h>

pthread_t tid[5];
pid_t child;

/*
void* downloadzip(void *arg){
	child = fork();
	int status;

	if(child < 0){
		exit(EXIT_FAILURE);
	}
	// {"wget", "--no-check-certificate", "-q", "https://docs.google.com/uc?export=download&id=1_djk0z-cx8bgISFsMiUaXzty2cT4VZp1", "-O", "music.zip", NULL};
	
	if(child == 0){
		char *download1[] = { "wget", "--no-check-certificate", "-q", "https://drive.google.com/uc?export=download&id=1_djk0z-cx8bgISFsMiUaXzty2cT4VZp1/view?usp=drivesdk"}; 
		char *download2[] = { "wget", "--no-check-certificate", "-q", "https://drive.google.com/uc?export=download&id=1jR67_JAyozZPs2oYtYqEZxeBLrs-k3dt/view?usp=drivesdk"};
		
		pthread_t id = pthread_self();
		if(pthread_equal(id, tid[0])){
			if(child==0){
				execv("/usr/bin/wget", download1);
			}
		} else if (pthread_equal(id, tid[1])){
                        if(child==0){
                                execv("/usr/bin/wget", download2);
                        }
		}
		return NULL;
	}
}*/

void make_folder(char *file){
	child = fork();
	int status;

	if(child <0){
		exit(EXIT_FAILURE);
	}
	if(child == 0){
		char *argv[] = { "touch", file, NULL};
		execv("/bin/touch", argv);
	} else {
		while((wait(&status)) > 0);
		return;
	}
}
 
void* unzip (void *arg){
	char *argv1[] = { "unzip", "-o", "/home/alya/Downloads/quote.zip" , "-d", "home/alya/shift3", NULL };
	char *argv2[] = { "unzip", "-o", "/home/alya/Downloads/music.zip", "-d", "/home/alya/shift3" , NULL };
	//execv ("/usr/bin/unzip", argv);

	pthread_t id=pthread_self();
	if(pthread_equal(id, tid[0])){
		child = fork();
		if(child==0) {
			execv("/usr/bin/unzip", argv1);
		}
	} else if(pthread_equal(id, tid[1])){
		child = fork();
		if(child==0){
			execv("/usr/bin/unzip", argv2);
		}
	}
	return NULL;
}


int main(){
	char path[50];
	strcpy(path, "/home");
	strcat(path, "/alya/shift3/");

	//pthread_t threads[NUM_THREADS];
	int i=0;
	int err;
	pid_t child_id;
	child_id = fork();
	int status;
	
	if(child_id == 0){
		char *argv[] = {"mkdir", "-p", path, NULL};
		execv("/bin/mkdir", argv);
	} else {
		wait(NULL);
	}

	while(i<3){
		err=pthread_create(&(tid[i]), NULL, &unzip, NULL);
		if(err!=0){
			printf("\n cant create thread ; [%s]", strerror(err));
		} else {
			printf("\n create thread success\n");
		}
		i++;
	}}
	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);
	exit(0);
	return 0;
}
