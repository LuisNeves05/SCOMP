#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#define SIZE 10

typedef struct{
	char desc[SIZE];
	int codigo;
	int preco;
} Produto;

int ex02_writer(){
	Produto *p;
	int fd, tamanho = sizeof(Produto);

	// Cria e verifica se a memória partilhada foi devidamente aberta 
	// permissões de read, write, execute/search para user, grupo e owner 
	if((fd = shm_open("/shm1191421", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
		perror("Failed to create shared memory");
		
	// Ajusta o tamanho da memória partilhada
	if(ftruncate(fd, tamanho)<0)
        perror("Failed to adjust memory size");

	// Mapea a memória partilhada 
	p = (Produto *)mmap(NULL, tamanho, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	// Verifica se a memória partilhada foi devidamente mapeada 
	
	 if (p == NULL) {
        perror("No mmap()");
        exit(0);
    }

	//É escrita a descricao da memória partilhada 
	printf("\nInsert the products description ");
	fgets(p->desc, sizeof(p->desc), stdin);
	//Limpa o buffer
	fflush(stdin);
	
	//É escrito o codigo do produto na memória partilhada 
	printf("\nInsert the products's code: ");
	scanf("%d", &(p->codigo));
	printf("\n");

	//É escrito o preco do produto na memória partilhada 
	printf("\nInsert the products's price: ");
	scanf("%d", &(p->preco));
	printf("\n");

	//Desfaz o mapeamento 
	if(munmap(p, tamanho)<0){ 
        perror("No munmap()");
        exit(0);
    }

	//Fecha o descritor 
	if(close(fd)<0){ 
        perror("No close()");
        exit(0);
    }

	return 0;
}


int ex02_reader(){
	Produto *p;
	int fd, tamanho=sizeof(Produto);

	//Cria e verifica se a memória partilhada foi devidamente aberta 
	//permissões de read, write, execute/search para user, grupo e owner 
	if((fd = shm_open("/shm1191421", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
		perror("Failed to create shared memory");

	//Ajusta o tamanho da memória partilhada
	if(ftruncate(fd, tamanho)<0)
        perror("Failed to adjust memory size");
        
	//Mapea a memória partilhada 
	p = (Produto *)mmap(NULL, tamanho, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	//Verifica se a memória partilhada foi devidamente mapeada 
    if (p == NULL) {
        perror("No mmap()");
        exit(0);
    }
    
    //São impressos: nome e número do aluno lidos da memória partilhada 
	printf("\nDescricao = %s", p->desc);
	printf("\nCodigo = %d", p->codigo);
	printf("\nPreco = %d", p->preco);
	printf("\n");

	//Desfaz o mapeamento 
	if(munmap(p, tamanho)<0){ 
        perror("No munmap()");
        exit(0);
    }
    
    	//Fecha o descritor 
	if(close(fd)<0){ 
        perror("No close()");
        exit(0);
    }

	//Apaga a memoria partilhada do sistema 
    if(shm_unlink("/shm1191421")<0){ 
        perror("No unlink()");
        exit(1);
    }

	return 0;
}

int main(){
	ex02_writer();
	ex02_reader();
}
