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

typedef struct
{
    int numbers[ARRAY_SIZE];
    int sem;
} st;

int main(){

   int i, j, fd, tamanho = sizeof(st);
    pid_t pid;
    st *mem_area;

    fd = shm_open("/shm1191421", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(fd, tamanho);
    mem_area = (struct st *)mmap(NULL, tamanho, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    mem_area->sem = 1;

    pid = fork();

    if (pid == 0) /* Producer */
    {
        for (i = 0; i < NUM_TRANSFER; i++)
        {
            // Waiting for sem to turn to 1
            while (!mem_area->sem)
                ;
            // Writing numbers in memory area
            for (j = 0; j < ARRAY_SIZE; j++)
            {
                mem_area->numbers[j] = (i * 10) + j + 1;
            }
            // Turn switch to allow consumer to read
            mem_area->sem = 0;
        }
    }
    else /* Consumer */
    {
        for (i = 0; i < NUM_TRANSFER; i++)
        {
            // Waiting for sem to turn to 0
            while (mem_area->sem)
                ;
            // Reading numbers from memory area
            for (j = 0; j < ARRAY_SIZE; j++)
            {
                printf("%d\n", mem_area->numbers[j]);
            }
            // Turn switch to allow producer to write
            mem_area->sem = 1;
        }
        // Disconnect map
        if (munmap(mem_area, tamanho) < 0)
        {
            printf("Error  munmap()!\n");
            exit(EXIT_FAILURE);
        }
        // Close fd
        if (close(fd) < 0)
        {
            printf("Error  close()!\n");
            exit(EXIT_FAILURE);
        }
    }

    wait(NULL);

    if (pid > 0) /* Parent process */
    {
        // Unlink shm
        if (shm_unlink("/shm1191421") < 0)
        {
            printf("Error at shm_unlink()!\n");
            exit(EXIT_FAILURE);
        }
        // Disconnect map
        if (munmap(mem_area, tamanho) < 0)
        {
            printf("Error  munmap()!\n");
            exit(EXIT_FAILURE);
        }
        // Close fd
        if (close(fd) < 0)
        {
            printf("Error  close()!\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
