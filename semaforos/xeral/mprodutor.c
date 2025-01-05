/*Exercicio 3 - m produtores*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>

#define N 5 /*Tamaño do buffer. Para a práctica esíxese que sexa 10*/

int main(int argc, char **argv){

	/*Elimínanse os semáforos creados na última execución do programa*/

	sem_unlink("EMPTY");
	sem_unlink("FULL");
	sem_unlink("MUTEX");

	sem_t *empty; /*Semáforo que indica os espazos baleiros do buffer*/
       	sem_t *full; /*Semáforo que indica os espazos ocupados do buffer*/
	sem_t *mutex; /*Semáforo que controla o acceso á rexión crítica*/

	int m = 1; /*Número de produtores*/

	if(argc == 2) m = atoi(argv[1]); /*Asígnase o número de produtores (por defecto 1)*/

	pid_t produtor[m]; /*PID dos produtores*/

	struct stat buffer; /*Buffer do ficheiro*/

	int fd; /*File descriptor*/

	srand(75); /*Set seed*/

	fd = open("produtos.txt", O_RDWR | O_CREAT); /*A función open recibe como argumentos a ruta do ficheiro e modo no que se abre, que se pode usar en combinación con outros flags (por exemplo, crear o ficheiro se non existe xa)*/

	ftruncate(fd, (N+1)*sizeof(int)); /*Dáselle ao ficheiro o tamaño do buffer pedido (10 elementos, neste caso enteiros). A maiores, almacénase a conta dos elementos que hai no buffer en todo momento*/

	int *mapeo = NULL; /*Punteiro á proxección de memoria do ficheiro*/

	if( fstat(fd, &buffer) == 0 ){
	
		if( (mapeo = mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){ /*Ademais de reservar memoria para o buffer, tamén se fai o mesmo para a variable que leva a conta dos elementos*/
		
		perror("Erro de mapeo do ficheiro en memoria"); /*Mensaxe en caso de erro*/

		exit(-1);
		
		}

		else{

			for(int i = 0; i < N; i++) *(mapeo + i) = 0; /*Inicialización do buffer*/
		
			int elemento; /*Elemento a producir*/
			
			int *conta = mapeo + N; /*A variable que leva a conta do número de produtos é compartida*/

			*conta = 0; /*Inicialízase a conta*/

			/*Inicialízanse os semáforos*/
		
			empty = sem_open("EMPTY", O_CREAT, 0700, N);

			full = sem_open("FULL", O_CREAT, 0700, 0);

			mutex = sem_open("MUTEX", O_CREAT, 0700, 1);

			for(int k = 0; k < m; k++){

				if( (produtor[k] = fork()) == 0 ){

					for(int i = 0; i < 5; i++){	

						sleep((int)rand() % 5); /*Dórmese o proceso*/	

						elemento = 1; /*Produción do elemento*/ 
					
						if( sem_wait(empty) != 0 ){ /*Decremento do valor do semáforo*/
						
							perror("Fallo no decremento do semáforo");

							exit(-1);
						
						}

						sleep((int)rand() % 5);

						if( sem_wait(mutex) != 0){ /*Decremento do valor do semáforo*/
						
							perror("Fallo no decremento do semáforo");
						
							exit(-1);

						}
						
						printf("\n\nPRODUTOR %d EN EXECUCIÓN %d\n", k + 1, i+1);

						printf("Buffer antes da inserción: ");

						for(int j = 0; j < N; j++) printf("%d ", *(mapeo + j));
						
						*(mapeo + *conta) = elemento; /*Inserción do elemento no buffer*/

						printf("\nBuffer despois da inserción: ");

						for(int j = 0; j < N; j++) printf("%d ", *(mapeo + j));

						(*conta)++; /*Modifícase o número de elementos do buffer*/

						printf("\nElemento producido. Quedan %d elementos\n", *conta);

						if( sem_post(mutex) != 0 ){ /*Incremento do valor do semáforo*/
						
						
							perror("Fallo no incremento do semáforo");

							exit(-1);	
						
						}
						
						sleep((int)rand() % 5);

						if( sem_post(full) != 0 ){ /*Incremento do valor do semáforo*/
						
							perror("Fallo no incremento do semáforo");

							exit(-1);
						
						}
					
						sleep((int) rand() % 5);

					}


					exit(0); /*Remata a execución do produtor*/
				}


			}

			for(int k = 0; k < m; k++){

				waitpid(produtor[k], NULL, 0);

			}			
		
		}

		munmap(mapeo, buffer.st_size); /*Libérase a proxección do ficheiro en memoria e a variable conta*/
	
		close(fd); /*Péchase o ficheiro*/

		/*Peche dos semáforos*/

		sem_close(empty);

		sem_close(full);

		sem_close(mutex);
	}

return(EXIT_SUCCESS);

}
