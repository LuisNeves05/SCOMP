#include "ex01.h"

int main(void)
{
    int i = 0, j, num;
    FILE *inputFile, *outputFile;
    int id;
    sem_t *sem;

    // Create semaphore to close and clean
    if ((sem = sem_open(SEMAPHORE, O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
    {
        perror("Error in sem_open()");
        exit(1);
    }

    //Cria ficheiro output
    outputFile = fopen(OUTPUT_FILE, "w");

    // Cria 8 processos
    id = babyMaker(8);

    if (id == 0) /* Filho */
    {
        sem_wait(sem);          //bloqueia

        inputFile = fopen(INPUT_FILE, "r");

        if (inputFile != NULL)
        {
            // Abre file output
            outputFile = fopen(OUTPUT_FILE, "a");

            for (j = 0; j < NUMEROS; j++)
            {
                fscanf(inputFile, "%d", &num);
                fprintf(outputFile, "%d\n", num);
            }

            // Fecha file output
            fclose(outputFile);

            // Fecha file input
            fclose(inputFile);
        }
        sem_post(sem);          //Dá up no semaforo para desbloquear o proximo
        exit(0);
    }
    else /* Pai */
    {
        babyFuneral(8);         //espera que todos os filhos terminem

         outputFile = fopen(OUTPUT_FILE, "r");
    
        if (outputFile == NULL)
        {
            printf("Could not open file");
        }
        else
        {   
            int z = 0;
            while (fscanf(outputFile, "%d", &num) != EOF)
            {   
                printf("%d\n", num);
                z++;
            }
        }
    }
   
    fclose(outputFile);

    sem_close(sem);
    sem_unlink(SEMAPHORE);

    return EXIT_SUCCESS;
}