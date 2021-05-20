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
#include "../lib/SharedMemory/shared.h"
#include "../lib/Pipes/pipes.h"
#include "../lib/Arrays/arrays.h"

#define size_array 1000000




void usePipes(int array[size_array]){

    int fd[2];

    if(pipe(fd) == -1){
        perror("Pipe failed");
    }

   
    int id = babyMaker(1);

    if(id==0){

        int arrCopy[size_array];
        
        close(fd[1]);
        read(fd[0], arrCopy, sizeof(int) * size_array);
    
    }
    else{
        close(fd[0]);
        write(fd[1],array,sizeof(int) * size_array);
        exit(1);
    }
}

void useSharedMem(int array[size_array]){


    int id = babyMaker(1);

    if(id == 0){   //READER 

        babyFuneral(1);

        int fd, tamanho = sizeof(int) * size_array;

        fd = createAndAdjustMemoryReader(fd, tamanho);

        int *arrCpyReader = (int *)mmap(NULL, tamanho, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);


        if (size_array < 20)
        {
            printf("SHM: ");
            showArray(0, size_array, arrCpyReader);
        }


        verifyIfMapped(arrCpyReader);
        endAll(arrCpyReader, fd, tamanho);

    }else{          //WRITER


        int fd, tamanho = sizeof(int) * size_array;
        fd = createAndAdjustMemoryWriter(fd, tamanho);

        int *arrCpyWriter = (int *)mmap(NULL, tamanho, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        for (int i = 0; i < size_array; ++i)
        {
            arrCpyWriter[i] = array[i];
        }

        verifyIfMapped(arrCpyWriter);

        unmapAll(arrCpyWriter, tamanho);
        closeDescriptor(fd);

        exit(1);
    }

}


int main(){

    int arr[size_array];

    populateArray(size_array, arr, 100);
    //showArray(0, TAMANHO_ARRAY, arr);

    clock_t begin = clock();
    usePipes(arr);
    clock_t end = clock();
    double time_spent_pipes = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("TIME TAKEN BY PIPES:%f\n", time_spent_pipes);

    begin = clock();
    useSharedMem(arr);
    end = clock();
    double time_spent_shm = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("TIME TAKEN BY SHARED MEMORY:%2f\n", time_spent_shm);

    return 0;
}
