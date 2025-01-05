/*Exercicio 3 - Consumidores*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/wait.h>

#define N 5 /*Tamaño do buffer compartido*/

int main(int argc, char **argv){
	
	/*Declaración de variables*/

	sem_t *empty; /*Cantas posicións do buffer están baleiras?*/

	sem_t *full; /*Cantas posicións do buffer están ocupadas?*/

	sem_t *mutex; /*Que proceso está na rexión crítica*/

	int n = 1; /*Número de consumidores*/

	if(argc == 2) n = atoi(argv[1]); /*No caso de que se especifique un número de consumidores. Por defecto, é un único*/

	pid_t consumidor[n]; /*PID dos consumidores*/

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

			for(int k = 0; k < n; k++){
			
				if( (consumidor[k] = fork()) == 0 ){
													
					for(int i = 0; i < 10; i++){

						sleep((int)rand() % 5);

						if(sem_wait(full) != 0){ /*Decremento do valor do semáforo*/
				
							perror("Erro no decremento do semáforo");

							exit(-1);
				
						}

						sleep((int)rand() % 5);

						if(sem_wait(mutex) != 0){ /*Decremento do valor do semáforo*/
				
							perror("Erro no decremento do semáforo");

							exit(-1);
				
						}

						printf("\n\nCONSUMIDOR %d EN EXECUCIÓN %d\n\n", k+1, i + 1); /*Especifícase quen está consumindo elementos*/

						printf("Buffer antes da extracción: ");

						for(int j = 0; j < N; j++) printf("%d ", *(mapeo + j));
						
						*(mapeo + (*conta - 1)) = 0; /*Quítase o elemento do buffer*/

						printf("\nBuffer despois da extracción: ");

						for(int j = 0; j < N; j++) printf("%d ", *(mapeo + j));

						(*conta)--; /*Actualízase a conta*/

						printf("\nElemento consumido. Quedan %d elementos\n", *conta);

						if(sem_post(mutex) != 0){ /*Incremento do valor do semáforo*/
				
							perror("Erro no incremento do semáforo");

							exit(-1);

						}

						sleep((int)rand() % 5);

						if(sem_post(empty) != 0){ /*Incremento do valor do semáforo*/
				
							perror("Erro no incremento do semáforo");
				
							exit(-1);
						}
			
						sleep((int) rand() % 5);

					}

					exit(0); /*Remata o consumidor*/
				
				}
			
			}

			for(int k = 0; k < n; k++){
			
				waitpid(consumidor[k], NULL, 0); /*Espérase a que acaben os consumidores*/

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
