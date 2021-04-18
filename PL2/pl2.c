#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "hash.h"

#define NBABIES 1
#define NSALES 50000

struct sales
{
	int customer_code;
	int product_code;
	int quantity;
};

int babyMaker(int n)
{
	int p, i;
	for (i = 0; i < n; i++)
	{
		p = fork();
		if (p < 0)
			exit(-1); //ERRO
		if (p == 0)
			return i + 1; //FILHO i+1
	}
	return 0; //PAI
}

void babyFuneral(int n)
{
	int i, status, exitcode, p;
	for (i = 0; i < n; i++)
	{
		p = waitpid(-1, &status, 0);
		if (WIFEXITED(status))
		{
			exitcode = WEXITSTATUS(status);
			printf("Baby %d has gone with exit code %d\n", p, exitcode);
		}
	}
}

void doWork(int id, int *fd)
{
	pid_t p;
	close(fd[1]);						//fecha a escrita
	read(fd[0], &p, sizeof(p));			//lê do pipe com o descritor fd (ESPERA que exista algo para ler)
	printf("The child reads: %d\n", p); //escreve no ecra
	close(fd[0]);						//fecha a leitura
	exit(id);
}

void pl2_ex01()
{
	int fd[2]; //descritor

	//criar o pipe, antes de criar o filho, para ser o mesmo pipe no pai e no filho
	if (pipe(fd) == -1)
	{
		perror("Pipe failed");
	}

	int id = babyMaker(NBABIES);
	if (id == 0)
	{ //PAI
		pid_t p = getpid();
		printf("The father writes: %d\n", getpid());
		close(fd[0]);				 //fecha a leitura
		write(fd[1], &p, sizeof(p)); //escreve no pipe com o descritor
		close(fd[1]);				 //fecha a escrita
		babyFuneral(NBABIES);
	}
	else
	{ //FILHOS
		doWork(id, fd);
	}
	printf("Father has gone too\n");
}

void pl2_ex03()
{
	int fd[2]; //descritor
	char frase[20] = {"Hello World"};
	char frase1[20] = {"Goodbye"};

	//criar o pipe, antes de criar o filho, para ser o mesmo pipe no pai e no filho
	if (pipe(fd) == -1)
	{
		perror("Pipe failed");
	}

	int id = babyMaker(NBABIES);
	if (id == 0)
	{										 //PAI
		close(fd[0]);						 //fecha a leitura
		write(fd[1], frase, sizeof(frase));	 //escreve no pipe com o descritor
		write(fd[1], frase1, sizeof(frase)); //escreve no pipe com o descritor
		close(fd[1]);						 //fecha a escrita
		babyFuneral(NBABIES);
	}
	else
	{ //FILHOS
		char frase[20] = {};
		char frase1[20] = {};

		close(fd[1]);							 //fecha a escrita
		read(fd[0], frase, sizeof(frase));		 //lê do pipe com o descritor fd (ESPERA que exista algo para ler)
		read(fd[0], frase1, sizeof(frase1));	 //lê do pipe com o descritor fd (ESPERA que exista algo para ler)
		printf("The child reads: %s\n", frase);	 //escreve no ecra
		printf("The child reads: %s\n", frase1); //escreve no ecra
		close(fd[0]);							 //fecha a leitura
		exit(id);
	}
	printf("Father has gone too\n");
}

void pl2_ex05()
{
	int fd[2]; //descritor

	//criar o pipe, antes de criar o filho, para ser o mesmo pipe no pai e no filho
	if (pipe(fd) == -1)
	{
		perror("Pipe failed");
	}

	int id = babyMaker(NBABIES);
	if (id == 0)
	{ //PAI
		char frase[20] = {};
		babyFuneral(NBABIES);
		close(fd[1]);
		read(fd[0], frase, sizeof(frase));
		printf("The client reads: %s\n", frase);
		close(fd[0]);
	}
	else
	{ //FILHOS
		char frase[256] = {};

		printf("Write a message in the server: ");
		scanf("%[^\n]", frase);
		fflush(stdin);
		close(fd[0]); //fecha a leitura

		for (int i = 0; frase[i] != '\0'; i++)
		{
			if (frase[i] >= 'a' && frase[i] <= 'z')
			{
				frase[i] = frase[i] - 32;
			}
		}

		write(fd[1], frase, sizeof(frase));
		close(fd[1]);

		exit(id);
	}
	printf("Father has gone too\n");
}

int sumpartArray(int id, int *vec1, int *vec2)
{
	int temp;
	int finish = id * 200 - 1;
	int start = finish - 200 + 1;
	for (int i = start; i < finish; i++)
	{
		//printf("%d	%d\n", start, finish);
		temp += vec1[i] + vec2[i];
	}

	//printf("O filho %d somou %d\n\n", getpid(), temp);
	return temp;
}

void pl2_ex06()
{

	int vec1[1000];
	int vec2[1000];
	int fd[2]; //file descriptor
	time_t t;

	srand((unsigned)time(&t));
	// initialize array with random numbers (rand(): stdlib.h)
	for (int i = 0; i < 1000; i++)
	{
		vec1[i] = rand() % 10000;
		vec2[i] = rand() % 10000;
	}

	int id;

	//criar o pipe, antes de criar o filho, para ser o mesmo pipe no pai e no filho
	if (pipe(fd) == -1)
	{
		perror("Pipe failed");
	}

	id = babyMaker(5);

	if (id == 0)
	{ //PAI
		close(fd[1]);
		int final = 0;
		int tmp;
		for (int i = 0; i < 5; i++)
		{
			int status;
			wait(&status);
			if (WIFEXITED(status))
			{
				read(fd[0], &tmp, sizeof(int));
				final += tmp;
			}
		}
		printf("Final result is %d\n", final);
	}
	else
	{ //FILHOS
		int soma = sumpartArray(id, vec1, vec2);
		close(fd[0]);
		write(fd[1], &soma, sizeof(int));
		close(fd[1]);
	}
}

void pl2_ex09()
{
	int fd[2]; //file descriptor
	struct sales productSales[NSALES];
	time_t t;
	srand((unsigned)time(&t));
	int nVal = 0;

	for (int i = 0; i < NSALES; i++)
	{
		productSales[i].customer_code = i;
		productSales[i].product_code = i;
		productSales[i].quantity = rand() % 30;

		if (productSales[i].quantity > 20)
		{
			nVal++;
		}
	}

	if (pipe(fd) == -1)
	{
		perror("Pipe failed");
	}

	int id = 0;
	id = babyMaker(10);

	if (id == 0)
	{ //PAI
		int i = 0;
		int code = 0;
		int customerArr[nVal];
		babyFuneral(10);

		close(fd[1]);

		printf("ALL CODES: ");
		while (1)
		{
			if (read(fd[0], &code, sizeof(code)) < 1)
				break;
			printf(" %d ", code);
			customerArr[i] = code;
			i++;
		}

		close(fd[0]);
		printf("\nAFTER ALL\n");
	}
	else
	{ //FILHOS
		int start = 0;

		if (id == 1)
		{
			start = 0;
		}
		else
		{
			start = (id - 1) * 5000;
		}

		for (int i = start; i < id * NSALES / 10 - 1; i++)
		{

			int quantity = productSales[i].quantity;
			if (quantity > 20)
			{
				close(fd[0]);
				write(fd[1], &productSales[i].customer_code, sizeof(productSales[i].customer_code));
			}
		}
		close(fd[1]);

		exit(1);
	}
}

int pl2_ex10()
{
	int p1[2]; // C > P
	int p2[2]; // P > C
	time_t t;
	srand((unsigned)time(&t));

	//criar o pipe, antes de criar o filho, para ser o mesmo pipe no pai e no filho
	if (pipe(p1) == -1 || pipe(p2) == -1)
	{
		perror("Pipe failed");
		return 1;
	}

	int id = babyMaker(1);

	if (id == 0)
	{ //PAI
		int creditChilde;

		if (read(p1[0], &creditChilde, sizeof(creditChilde)) == -1)
		{
			return 4;
		};
		printf("CHILDE MONEY: %d\n", creditChilde);

		int randNum = (rand() % 4) + 1;

		if (creditChilde > 5)
		{
			int not = 1;
			if (write(p2[1], &not, sizeof(not )) == -1)
			{
				return 4;
			};
		}

		int childePred;
		if (read(p1[0], &childePred, sizeof(childePred)) == -1)
		{
			return 5;
		};

		if (childePred == randNum)
		{
			creditChilde += 10;
		}
		else
		{
			creditChilde -= 5;
		}

		if (write(p2[1], &creditChilde, sizeof(creditChilde)) == -1)
		{
			return 6;
		};
	}
	else
	{ //FILHO
		int credit = 20;
		if (write(p1[1], &credit, sizeof(credit)) == -1)
		{
			return 3;
		};

		int notification;
		if (read(p2[0], &notification, sizeof(notification)) == -1)
		{
			return 4;
		};

		if (notification == 1)
		{
			srand((unsigned)time(NULL));
			int randNum = (rand() % 4) + 1;
			randNum = (rand() % 4) + 1;
			if (write(p1[1], &randNum, sizeof(randNum)) == -1)
			{
				return 3;
			};
		}

		if (read(p2[0], &credit, sizeof(credit)) == -1)
		{
			return 7;
		};
		printf("FINAL RESULT: %d\n", credit);

		exit(id);
	}
}

char *my_itoa(int num, char *str)
{
	if (str == NULL)
	{
		return NULL;
	}
	sprintf(str, "%d", num);
	return str;
}

int pl2_ex12()
{
	int p1[2];

	if (pipe(p1) == -1)
	{
		perror("Pipe failed");
	}

	int nb = 2;
	int id = babyMaker(nb);

	if (id == 0) //PAI
	{
		ht_t *ht = ht_create();

		ht_set(ht, "1", "iogurtes 1.50 €");
		ht_set(ht, "2", "pao 0.50 €");
		ht_set(ht, "3", "melao 0.75 €");
		ht_set(ht, "4", "carne 4.50 €");
		ht_set(ht, "5", "gengibre 0.70 €");
		ht_set(ht, "6", "macas 1.00 €");
		ht_set(ht, "7", "pimentos 1.20 €");
		ht_set(ht, "8", "cenoura 0.80 €");
		ht_set(ht, "9", "peixe 3.50 €");
		ht_set(ht, "10", "brinquedos 12.50 €");
		ht_set(ht, "11", "livros 25.00 €");
		ht_set(ht, "12", "revistas 2.50 €");

		//ht_dump(ht);
		while (1)
		{
			int childeNum;
			char str[33];

			sleep(1);
			close(p1[1]);

			if (read(p1[0], &childeNum, sizeof(childeNum)) == -1)
			{
				return 2;
			};

			my_itoa(childeNum, str);
			char result[50];
			strcpy(result, ht_get(ht, str));

			printf("OBTAINED : %s\n", result);

			// if (write(p1[1], result, sizeof(result)) == -1)
			// {
			// 	return 3;
			// };
		}
	}
	else //CHILD
	{

		srand(time(NULL) ^ getpid());
		float sl = (float)rand() / (float)(RAND_MAX / 5);

		while (1)
		{
			sleep(sl);
			int randomProduct = (rand() % 11) + 1;
			// printf("RANDOM NUM:%.6f FROM %d\n", sl, getpid());
			// printf("RANDOM NUM:%d FROM %d\n", rand() % 10, getpid());
			if (write(p1[1], &randomProduct, sizeof(randomProduct)) == -1)
			{
				return 1;
			};
			//printf("ID: %d WRITING: %d\n", getpid(), randomProduct);
			// char productDesc[50] = {};
			// if (read(p1[0], productDesc, sizeof(productDesc)) == -1)
			// {
			// 	return 4;
			// };

			// printf("PROD:%s\n", productDesc);
			// break;
		}
		exit(id);
	}
}

int main()
{
	//pl2_ex03();
	//pl2_ex05();
	//pl2_ex06();
	//pl2_ex09();
	//pl2_ex10();
	pl2_ex12();
}