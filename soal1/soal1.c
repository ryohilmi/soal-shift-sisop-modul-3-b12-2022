#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<pwd.h>

pthread_t tid[5];
pid_t child;

char cur_dir[256];
char *namafile[] = {"quote","music"};
char *linkzip[] = {
		"https://drive.google.com/u/0/uc?id=1jR67_JAyozZPs2oYtYqEZxeBLrs-k3dt&export=download",
        "https://drive.google.com/u/0/uc?id=1_djk0z-cx8bgISFsMiUaXzty2cT4VZp1&export=download"
};

//download file zip
void download(){
	for(int i = 0; i < 2; i++){
		child = fork();
		if(child < 0) exit(EXIT_FAILURE);
		if(child == 0){
			//namafile + .zip
			char name[strlen(namafile[i]) + strlen(".zip") + 1];
			strcpy(name, namafile[i]);
			strcat(name, ".zip");
			printf("DOWNLOAD %s BERHASIL\n",name);
			char *argv[] = {"wget", "-q", "--no-check-certificate",linkzip[i],"-O",name,NULL};
			execv("/usr/bin/wget",argv);
		}
		waitpid(child,NULL,0);
	}

}

//bikin folder soal1
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


//unzip file zip
void *unzip(void *index){
	int i = *((int*) index);
	child = fork();
	if(child < 0) exit(EXIT_FAILURE);
	if(child == 0){
		DIR *temp = opendir(namafile[i]);
		if(temp){
			int temp_id = fork();
			if(temp_id < 0) exit(EXIT_FAILURE);
			if(temp_id == 0){
				char *argv[] = {"rm","-r",namafile[i],NULL};
				execv("/usr/bin/rm",argv);
			}
			waitpid(temp_id,NULL,0);
		}
		char *argv[] = {"mkdir","-p",namafile[i],NULL};
		execv("/usr/bin/mkdir",argv);
	}
	waitpid(child,NULL,0);
	printf("MEMBUAT DIREKTORI %s BERHASIL\n",namafile[i]);
	
	child = fork();
	if(child < 0) exit(EXIT_FAILURE);
	if(child == 0){
		char name[strlen(namafile[i]) + strlen(".zip") + 1];
		strcpy(name, namafile[i]);
		strcat(name, ".zip");
		printf("UNZIP %s KE DIREKTORI %s BERHASIL\n",name,namafile[i]);
		char *argv[] = {"unzip","-qq",name,"-d",namafile[i],NULL};
		execv("/usr/bin/unzip",argv);
	}
	waitpid(child,NULL,0);
	
}

/*
char base46_map[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                     'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                     'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                     'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

char* base64_decode(char* cipher) {

    char counts = 0;
    char buffer[4];
    char* plain = malloc(strlen(cipher) * 3 / 4);
    int i = 0, p = 0;

    for(i = 0; cipher[i] != '\0'; i++) {
        char k;
        for(k = 0 ; k < 64 && base46_map[k] != cipher[i]; k++);
        buffer[counts++] = k;
        if(counts == 4) {
            plain[p++] = (buffer[0] << 2) + (buffer[1] >> 4);
            if(buffer[2] != 64)
                plain[p++] = (buffer[1] << 4) + (buffer[2] >> 2);
            if(buffer[3] != 64)
                plain[p++] = (buffer[2] << 6) + buffer[3];
            counts = 0;
        }
    }

    plain[p] = '\0';
    return plain;
}
*/


void *decode(void *index){
	int i = *((int*) index);
	char name[strlen(namafile[i]) + strlen(".txt") + 1];
	strcpy(name, namafile[i]); strcat(name, ".txt");
	FILE *output = fopen(name, "w");
	int link[2];
	
	DIR *dir_scan;
	struct dirent *entry;
	if((dir_scan = opendir(namafile[i])) == NULL) exit(EXIT_FAILURE);
	while((entry = readdir(dir_scan)) != NULL){
		if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0) continue;
		char *foo = (char*) malloc(sizeof(char) * 4096);
		if(pipe(link) == -1) exit(EXIT_FAILURE);
		child = fork();
		if(child < 0) exit(EXIT_FAILURE);
		if(child == 0){
			char temp[strlen(entry->d_name) + strlen(namafile[i]) + 2];
			strcpy(temp, namafile[i]);
			strcat(temp,"/");
			strcat(temp, entry->d_name);
			
			dup2(link[1], STDOUT_FILENO);
			close(link[0]);close(link[1]);
			
			char *argv[] = {"base64","-d",temp,NULL};
			execv("/usr/bin/base64",argv);
		}
		waitpid(child,NULL,0);
		close(link[1]);
		read(link[0],foo,4096);
		fprintf(output, "%s\n",foo);
	}
	fclose(output);
}

void folderhasil(){
	child = fork();
	if(child < 0) exit(EXIT_FAILURE);
	if(child == 0){
		DIR *temp = opendir("hasil");
		if(temp){
			int temp_id = fork();
			if(temp_id < 0) exit(EXIT_FAILURE);
			if(temp_id == 0){
				char *argv[] = {"rm","-r","hasil",NULL};
				execv("/usr/bin/rm",argv);
			}
			waitpid(temp_id,NULL,0);
		}
		char *argv[] = {"mkdir","-p","hasil",NULL};
		execv("/usr/bin/mkdir",argv);
	}
	waitpid(child, NULL, 0);
	
	//moving hasil to hasil directory
	for(int i = 0; i < 2; i++){
		char *name = (char*) malloc(strlen(namafile[i]) + strlen(".txt") + 1);
		strcpy(name, namafile[i]); strcat(name, ".txt");
		char *dirtemp = (char*) malloc(strlen(cur_dir) + strlen("hasil") + 2);
		strcpy(dirtemp, cur_dir);
		strcat(dirtemp,"/");strcat(dirtemp,"hasil");
		child = fork();
		if(child < 0) exit(EXIT_FAILURE);
		if(child == 0){
			char *argv[] = {"mv",name,dirtemp,NULL};
			execv("/usr/bin/mv",argv);
		}
		waitpid(child,NULL,0);
	}
}

void zip_hasil(char *password){
	//zip hasil
	child = fork();
	if(child < 0) exit(EXIT_FAILURE);
	if(child == 0){
		char *argv[] = {"zip","-q","-P",password,"-r","hasil.zip","hasil",NULL};
		execv("/usr/bin/zip",argv);
	}
	waitpid(child,NULL,0);
	if((child = fork()) == -1) exit(EXIT_FAILURE);
	if(child == 0){
		char *argv[] = {"rm", "-r", "hasil", NULL};
		execv("/usr/bin/rm",argv);
	}
	waitpid(child,NULL,0);
	puts("ZIP HASIL KE HASIL.ZIP BERHASIL");
}

int unzipped_hasil = 0;

void *unzip_hasil(void *_password){
	char *password = (char*) _password;
	child = fork();
	if(child < 0) exit(EXIT_FAILURE);
	if(child == 0){
		char *argv[] = {"unzip","-q","-P",password,"hasil.zip",NULL};
		execv("/usr/bin/unzip",argv);
	}
	waitpid(child,NULL,0);
	unzipped_hasil = 1;
}
void *no_txt(){
	FILE *no = fopen("no.txt","w");
	fprintf(no,"No\n");
	fclose(no);
	DIR *hasil;
	while(!unzipped_hasil){
	}
	char *dirtemp = (char*) malloc(strlen(cur_dir) + strlen("hasil") + 2);
	strcpy(dirtemp, cur_dir);
	strcat(dirtemp,"/");strcat(dirtemp,"hasil");
	child = fork();
	if(child < 0) exit(EXIT_FAILURE);
	if(child == 0){
		char *argv[] = {"mv","no.txt",dirtemp,NULL};
		execv("/usr/bin/mv",argv);
	}
	waitpid(child,NULL,0);
}


int main(){
	char path[50];
	strcpy(path, "/home");
	strcat(path, "/alya/soal-shift-modul3/soal1");

	//pthread_t threads[NUM_THREADS];
	getcwd(cur_dir, sizeof(cur_dir) - 1);
	pid_t child_id;
	child_id = fork();
	int status;
	
	if(child_id < 0){
		exit(EXIT_FAILURE);
	} 
	
	if (child_id == 0){
		download();
	} else{
			wait(NULL);
	}

	pthread_t threads[2];
	int int_threads[2] = {0,1};
	
	//mkdir and unzip
	pthread_create(&threads[0],NULL,unzip,(void *) &int_threads[0]);
	pthread_create(&threads[1],NULL,unzip,(void *) &int_threads[1]);
	for(int i = 0; i < 2; i++) pthread_join(threads[i], NULL);

	
	//decode
	pthread_create(&threads[0],NULL,decode,(void *) &int_threads[0]);
	pthread_create(&threads[1],NULL,decode,(void *) &int_threads[1]);
	for(int i = 0; i < 2; i++) pthread_join(threads[i], NULL);
	

	//set up password
	uid_t uid;
	struct passwd *pw;
	uid = geteuid();
	pw = getpwuid(uid);
	if(pw == NULL) exit(EXIT_FAILURE);
	char password[strlen(pw->pw_name) + strlen("mihinomenest") + 1];
	strcpy(password,"mihinomenest");strcat(password, pw->pw_name);
	
	folderhasil(); //mkdir and move hasil to directory hasil
	zip_hasil(password); //zip directory hasil
	
	//no.txt
	pthread_create(&threads[0],NULL,no_txt,NULL);
	pthread_create(&threads[1],NULL,unzip_hasil,(void*)password);
	for(int i = 0; i < 2; i++) pthread_join(threads[i],NULL);
	zip_hasil(password);

	return 0;
}
