/*Exercicio 2 - Consumidor (con semáforos)*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <pthread.h>

#define N 5 /*Tamaño do buffer compartido*/

int main(int argc, char **argv){

	/*Elimínanse os semáforos creados na última execución do programa*/

//	sem_unlink("EMPTY");
//	sem_unlink("FULL");
//	sem_unlink("MUTEX");

	/*Declaración de variables*/

	sem_t *empty; /*Cantas posicións do buffer están baleiras?*/

	sem_t *full; /*Cantas posicións do buffer están ocupadas?*/

	sem_t *mutex; /*Que proceso está na rexión crítica*/

	struct stat buffer; /*Buffer do ficheiro (NON É O BUFFER COMPARTIDO DOS PROCESOS)*/

	int fd; /*File descriptor*/

	fd = open("produtos.txt", O_RDWR | O_CREAT); /*A función open recibe como argumentos a ruta do ficheiro e o modo no que se abre, que se pode usar en combinación con outras opcións*/

	ftruncate(fd, (N+1)*sizeof(int)); /*Axústase o tamaño do ficheiro*/

	int *mapeo = NULL; /*Punteiro á proxección de memoria do ficheiro*/

	if ( fstat(fd, &buffer) == 0 ){
	
		if( (mapeo = mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
		
			perror("Erro de mapeo do ficheiro en memoria"); /*Mensaxe en caso de erro*/

			exit(-1);
		
		}

		else{
		
			int *conta = mapeo + N; /*A variable que leva a conta do número de produtos é compartida*/

			/*Ábrense os semáforos (o produtor encargouse de inicializalos*/

			empty = sem_open("EMPTY", 0);

			full = sem_open("FULL", 0);

			mutex = sem_open("MUTEX", 0);

			for(int i = 0; i < 100; i++){

				sleep((int)rand() % 5);

				if(sem_wait(full) != 0){ /*Decremento do valor do semáforo*/
				
					perror("Erro no decremento do semáforo");

					exit(-1);
				
				}

				if(sem_wait(mutex) != 0){ /*Decremento do valor do semáforo*/
				
					perror("Erro no decremento do semáforo");

					exit(-1);
				
				}

				printf("Buffer antes da extracción: ");

				for(int j = 0; j < N; j++) printf("%d ", *(mapeo + j));
				*(mapeo + (*conta - 1)) = 0; /*Quítase o elemento do buffer*/

				(*conta)--; /*Actualízase a conta*/

				printf("\nBuffer despois da extracción: ");

				for(int j = 0; j < N; j++) printf("%d ", *(mapeo + j));
				printf("\nElemento consumido. Quedan %d elementos\n", *conta);



				if(sem_post(mutex) != 0){ /*Incremento do valor do semáforo*/
				
					perror("Erro no incremento do semáforo");

					exit(-1);

				}

				if(sem_post(empty) != 0){ /*Incremento do valor do semáforo*/
				
					perror("Erro no incremento do semáforo");
				
					exit(-1);
				}
			
				sleep((int) rand() % 5);

			}

		}
	
		munmap(mapeo, buffer.st_size);

		close(fd);

		sem_close(empty);

		sem_close(full);

		sem_close(mutex);
	
	}


return(EXIT_SUCCESS);

}
