#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#define FILHOS 10
#define SIZEARRAY 1000
#define SIZE 10

int babyMaker(int n){
	int p, i;
	for(i=0; i<n; i++){
		p = fork();
		if(p<0) exit(-1); //ERRO
		if(p==0) return i+1; //FILHO i+1
	}
	return 0; //PAI
}

void babyFuneral(int n){
	int i, status, exitcode, p;
	for(i=0; i<n; i++){
		p = waitpid(-1,&status,0);
		if(WIFEXITED(status)){
			exitcode = WEXITSTATUS(status);
		}
	}
}

typedef struct{
	int array[SIZE];
} Struct;

int ex11(){

	Struct *s;
	int fd, tamanho = sizeof(Struct);

	srand ((unsigned) time (NULL));
	int bigArray[SIZEARRAY];
	    for (int i = 0; i < 1000; ++i){
    	bigArray[i] =  rand () % 1000;
    }

    int fda[2]; //descritor
    //criar o pipe, antes de criar o filho, para ser o mesmo pipe no pai e no filho
    if(pipe(fda)==-1){
		perror("Pipe failed");
	}

	// Cria e verifica se a memória partilhada foi devidamente aberta 
	// permissões de read, write, execute/search para user, grupo e owner 
	if((fd = shm_open("/shm1", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
		perror("Failed to create shared memory");
		
	// Ajusta o tamanho da memória partilhada
	if(ftruncate(fd, tamanho)<0)
        perror("Failed to adjust memory size");

	// Mapea a memória partilhada 
	s = (Struct *)mmap(NULL, tamanho, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	
	// Verifica se a memória partilhada foi devidamente mapeada 
	 if (s == NULL) {
        perror("No mmap()");
        exit(0);
    }

	int pr = babyMaker(FILHOS);
	
  	if(pr != 0){ //FILHOS (WRITERS)
			int max = -1;
  			for (int w = (pr - 1)*100; w < pr*100; ++w){
  				if(bigArray[w] > max){
  					max = bigArray[w];
  				}
  			}
  			close(fda[0]); //fecha a leitura
        	write(fda[1], &max, sizeof(max)); 
	}else{//PAI (READER)

	babyFuneral(FILHOS);

	//Cria e verifica se a memória partilhada foi devidamente aberta 
	//permissões de read, write, execute/search para user, grupo e owner 
	if((fd = shm_open("/shm1", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
		perror("Failed to create shared memory");

	//Ajusta o tamanho da memória partilhada
	if(ftruncate(fd, tamanho)<0)
        perror("Failed to adjust memory size");
        
	//Mapea a memória partilhada 
	s = (Struct *)mmap(NULL, tamanho, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	//Verifica se a memória partilhada foi devidamente mapeada 
    if (s == NULL) {
        perror("No mmap()");
        exit(0);
    }

    int maxMax = -1;
    int valor;
    for (int i = 0; i < FILHOS; ++i) {
    	close(fda[1]);
    	read(fda[0], &valor, sizeof(valor));
    	s->array[i] = valor;
    	printf("INDEX: %d com o valor %d\n", i, s->array[i]);
    	if(s->array[i] > maxMax){
    		maxMax = s->array[i];
    	}
    }

    printf("Máximo Global %d\n", maxMax);

	//Desfaz o mapeamento 
	if(munmap(s, tamanho)<0){ 
        perror("No munmap()");
        exit(0);
    }
    
    	//Fecha o descritor 
	if(close(fd)<0){ 
        perror("No close()");
        exit(0);
    }

	//Apaga a memoria partilhada do sistema 
    if(shm_unlink("/shm1")<0){ 
        perror("No unlink()");
        exit(1);
    }

	}
	return 0;
}

int main(){
	ex11();
}
