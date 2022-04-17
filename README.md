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
