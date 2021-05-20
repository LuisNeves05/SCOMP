#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

#define STR_SIZE 50
#define NR_DISC 10

struct aluno{
    int numero;
    char nome[STR_SIZE];
    int disciplinas[NR_DISC];
    int flag;
    };

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


int main(){

     int fd, tamanho = sizeof(struct aluno *);
     struct aluno *a;
     int soma = 0;


     if((fd = shm_open("/shm1191421", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
        perror("Failed to create shared memory");
        
    // Ajusta o tamanho da memória partilhada
    if(ftruncate(fd, tamanho)<0)
        perror("Failed to adjust memory size");

    // Mapea a memória partilhada 
    a = (struct aluno *)mmap(NULL, tamanho, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    // Verifica se a memória partilhada foi devidamente mapeada 
     if (a == NULL) {
        perror("No mmap()");
        exit(0);
    }

    a->flag = 1;

    int id = babyMaker(1);


    if(id == 0){    //PAIZÃO      

    printf("Introduza o nome: \n");
    scanf("%[^\n]", &a->nome);

    printf("Introduza o numero: \n");
    scanf("%d", &a->numero);



    printf("Notas do aluno: (Insert %d)\n", NR_DISC);
    
        for (int i = 0; i < NR_DISC; i++)
        {
            printf("%d. ", i + 1);
            scanf("%d", &a->disciplinas[i]);
        }


        a->flag = 0;  //para dizer ao filho para avançar
        wait(NULL);


        if(munmap(a, tamanho)<0){ 
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

    }else{            //FILHOTE

        if((fd = shm_open("/shm1191421", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
        perror("Failed to create shared memory");
        
         // Ajusta o tamanho da memória partilhada
        if(ftruncate(fd, tamanho)<0)
            perror("Failed to adjust memory size");

        // Mapea a memória partilhada 
        a = ( struct aluno *)mmap(NULL, tamanho, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        // Verifica se a memória partilhada foi devidamente mapeada 
        
         if (a == NULL) {
            perror("No mmap()");
            exit(0);
        }



        int maior= 0;
        int menor = 20;


        while(a->flag == 1);

        for (int b = 0; b < NR_DISC; b++)
        {
           
            if (a->disciplinas[b] > maior)
                maior = a->disciplinas[b];
         
            if (a->disciplinas[b] < menor)
                menor = a->disciplinas[b];
           
            soma += a->disciplinas[b];
        }

        double media = ((double)soma) / NR_DISC;


        printf("\nNumero estudante: %d\n", a->numero);
        printf("Nome estudante: %s\n", a->nome);
        printf("Nota mais alta: %d\n", maior);
        printf("Nota mais baixa: %d\n", menor);
        printf("Media de notas: %.2f\n", media);

        exit(0);

    }


    

    return 0;
}
