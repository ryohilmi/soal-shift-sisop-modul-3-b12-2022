## Kelompok B12:
- [Alya Shofarizqi Inayah](https://gitlab.com/https://gitlab.com/alyashofarizqi): 5025201113
- [Ryo Hilmi Ridho](https://gitlab.com/ryohilmi): 5025201192
- [Surya Abdillah](https://gitlab.com/Surya_Abdillah): 5025201229

# Soal 1
### Penjelasan Soal
Pada soal nomor 1 ini diberikan dua folder zip yaitu quote.zip dan music.zip, kita diperintahkan untuk mendownload dan membuat program untuk memecahkan kode-kode di dalam file zip tersebut dengan cara decoding dengan base64. agar lebih cepat, disarankan untuk menggunakan thread.

## 1.a
Download dua file zip dan unzip file zip tersebut di dua folder yang berbeda dengan
nama quote untuk file zip quote.zip dan music untuk file zip music.zip dilakukan bersamaan menggunakan thread.

```c
char cur_dir[256];
char *namafile[] = {"quote","music"};
char *linkzip[] = {
		"https://drive.google.com/u/0/uc?id=1jR67_JAyozZPs2oYtYqEZxeBLrs-k3dt&export=download",
        "https://drive.google.com/u/0/uc?id=1_djk0z-cx8bgISFsMiUaXzty2cT4VZp1&export=download"
}
```
Sebelumnya saya menginisialisasi `namafile` dan `linkzip` untuk memudahkan membuat program download dan unzip file.

```c
void download(){
	for(int i = 0; i < 2; i++){
		...
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
```
Setelah itu, membuat fungsi `download()` sebagai berikut dan akan mengeluarkan output "DOWNLOAD music.zip BERHASIL" dan "DOWNLOAD quote.zip BERHASIL" setelah memanggil fungsi pada `int main()`

```c
void *unzip(void *index){
	int i = *((int*) index);
	...
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
	
	...
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
```
Lalu, membuat fungsi `unzip(void *index)` untuk extract atau unzip kedua file tersebut sekaligus membuat folder music dan quote untuk hasil zip kedua file berdasarkan nama file nya. Apabile thread berhasil, akan ada output "MEMBUAT DIREKTORI music/quote BERHASIL" dan "UNZIP music.zip/quote.zip KE DIREKTORI music/quote BERHASIL"

## 1.b
Diperintahkan untuk decoding semua file.txt yang ada dengan base64 dan memasukkan hasilnya dalam satu file.txt yang baru untuk masing-masing folder yaitu music.txt dan quote.txt pada saat yang sama dengan menggunakan thread dan masing-masing kalimat dipisahkan dengan newline/enter

```c
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
		...
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

```
Membuat fungsi `decode(void *index)` untuk decoding semua file txt pada masing masing folder dengan menggunakan base64 dan menggunakan strcpy strcat untuk membuat file txt yang nantinya berisi hasil dari decoding semua file.txt

## 1.c
Memindahkan kedua file.txt yang berisi hasil decoding ke folder yang baru bernama hasil.

```c
void folderhasil(){
	...
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
		...
		if(child == 0){
			char *argv[] = {"mv",name,dirtemp,NULL};
			execv("/usr/bin/mv",argv);
		}
		waitpid(child,NULL,0);
	}
}
```
Membuat fungsi `folderhasil()` untuk membuat folder hasil menggunakan mkdir dan pada iterasi selanjutnya digunakan untuk memindahkan file .txt kedalam direktori hasil.

## 1.d
Zip folder hasil menjadi hasil.zip dengan set password 'mihinomenest[nama user]'.
contoh password : mihinomenestnovak

```c
void zip_hasil(char *password){
	...
	if(child == 0){
		char *argv[] = {"zip","-q","-P",password,"-r","hasil.zip","hasil",NULL};
		execv("/usr/bin/zip",argv);
	}
	...
	puts("ZIP HASIL KE HASIL.ZIP BERHASIL");
}
```
Membuat fungsi `zip_hasil(char *password)` untuk zip folder hasil menjadi hasil.zip. apabila thread berhasil akan mengeluarkan output "ZIP HASIL KE HASIL.ZIP BERHASIL"

```c
uid_t uid;
	struct passwd *pw;
	uid = geteuid();
	pw = getpwuid(uid);
	if(pw == NULL) exit(EXIT_FAILURE);
	char password[strlen(pw->pw_name) + strlen("mihinomenest") + 1];
	strcpy(password,"mihinomenest");strcat(password, pw->pw_name);
```
> Set password pada `int main` 

## 1.e
Dikarenakan ada yang kurang, kami diminta untuk meng-unzip file hasil.zip dan buat file no.txt dengan tulisan 'No' pada saat yang bersamaan, lalu zip kedua file hasil dan file no.txt menjadi hasil.zip dengan password yang sama seperti sebelumnya.

```c
int unzipped_hasil = 0;
void *unzip_hasil(void *_password){
	char *password = (char*) _password;
	...
	if(child == 0){
		char *argv[] = {"unzip","-q","-P",password,"hasil.zip",NULL};
		execv("/usr/bin/unzip",argv);
	}
	waitpid(child,NULL,0);
	unzipped_hasil = 1;
}
```
Membuat fungsi `unzip_hasil(void *_password)` yang digunakan untuk unzip hasil.zip 

```c
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
	...
	if(child == 0){
		char *argv[] = {"mv","no.txt",dirtemp,NULL};
		execv("/usr/bin/mv",argv);
	}
	waitpid(child,NULL,0);
}
```
Membuat fungsi `no_txt()` yang digunakan untuk membuat file no.txt sekaligus tulisan 'No' pada saat yang bersamaan

## Dokumentasi Pengerjaan
- Pembuatan script c
![script_c](/uploads/a1656f0e9a668afc6fcaec9934e3f810/script_c.png)
- Hasil file.txt sebelum decoding
![sebelum_decoding](/uploads/482973bbc82b36298a1d175caf8a1b13/sebelum_decoding.png)
- Hasil file.txt setelah decoding
![setelah_decoding](/uploads/afc5a64068f161cc9244e53b2e7924a2/setelah_decoding.png)
- Memasukkan password untuk membuka hasil.zip
![set_password](/uploads/484a12c17afca7f90d5e7790e34c8a39/set_password.png)

## Kendala
Kendala-kendala saat progress pengerjaan soal nomor 2 :
1. Pada poin 2b, sulit untuk mengimplementasikan format base64, karena sebelumnya saya membuat base46_map tetapi mengalami error.
2. Mengalami error saat set password pada int main

## SOAL 3
Nami adalah seorang pengoleksi harta karun handal. Karena Nami memiliki waktu luang, Nami pun mencoba merapikan harta karun yang dimilikinya berdasarkan jenis/tipe/kategori/ekstensi harta karunnya. Setelah harta karunnya berhasil dikategorikan, Nami pun mengirimkan harta karun tersebut ke kampung halamannya.
Contoh jika program pengkategorian dijalankan
## A.
Hal pertama yang perlu dilakukan oleh Nami adalah mengextract zip yang diberikan ke dalam folder “/home/[user]/shift3/”. Kemudian working directory program akan berada pada folder “/home/[user]/shift3/hartakarun/”. Karena Nami tidak ingin ada file yang tertinggal, program harus mengkategorikan seluruh file pada working directory secara rekursif
> Untuk unzip dapat dilakukan secara manual, folder hartakarun hasil unzip diletakkan pada folder yang dengan folder soal3.c
```sh
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
```
## B.
Semua file harus berada di dalam folder, jika terdapat file yang tidak memiliki
ekstensi, file disimpan dalam folder “Unknown”. Jika file hidden, masuk folder
“Hidden”
```sh
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
```
## C.
Agar proses kategori bisa berjalan lebih cepat, setiap 1 file yang dikategorikan dioperasikan oleh 1 thread.
```sh
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
```
## D.
Untuk mengirimkan file ke Cocoyasi Village, nami menggunakan program client-server. Saat program client dijalankan, maka folder /home/[user]/shift3/hartakarun/” akan di-zip terlebih dahulu dengan nama
“hartakarun.zip” ke working directory dari program client
```sh
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
```
## E.
Client dapat mengirimkan file “hartakarun.zip” ke server dengan mengirimkan
command berikut ke server
```sh
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
```
## Dokumentasi Pengerjaan
![image](/uploads/479c1ba8f00a4c3b7b35cffdbefa710e/image.png)
![image](/uploads/995cf74c343c3557293a72c17b2e04ac/image.png)
![image](/uploads/08af74b513a1cba38a6ce6b3982227c7/image.png)
![image](/uploads/32762c02da30f26f44fe5ec9843c03ff/image.png)
## Kendala
- Praktikan mengalami kesulitan dalam menemukan bagaimana cara melakukan unzip tanpa menggunakan bash, sehingga waktu pengerjaan terhabis dalam kendala ini
- Koneksi client menuju server terhambat
- Pembuatan fungsi memindahkan file cukup mengalami kendala
