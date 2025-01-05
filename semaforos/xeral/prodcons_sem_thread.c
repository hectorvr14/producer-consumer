/*O problema do produtor-consumidor solucionado con semáforos e fíos*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>

/*Estrutura dos argumentos que se lle pasarán a cada fío*/

struct args{

	int indice;

	int iter; /*Número de iteracións*/

};

/*Definición de constantes*/

#define N 10 /*Tamaño máximo do buffer predeterminado*/
#define MAX 100 /*Número máximo de elementos a producir*/

/*Variables globais*/

int conta = 0; /*Número de elementos que se teñen actualmente no buffer*/

int NPROD; /*Número de produtores - modificables por liña de comandos*/
int NCONS; /*Número de consumidores - modificables por liña de comandos*/

int id = 1; /*Contido do elemento*/

int buffer[N]; /*Buffer compartido entre os fíos*/

sem_t *empty; /*Semáforo que indica os espazos baleiros do buffer*/
sem_t *full; /*Semáforo que indica os espazos ocupados do buffer*/
sem_t *mutex; /*Semáforo que controla o acceso á rexión crítica*/

/*Produtor*/

void *produtor (void *ptr){

	sleep(rand() % 5);

	struct args *a = (struct args*) ptr; /*Recóllese o argumento*/

	int item = 0; /*Elemento a producir*/

	for (int k = 0; k < a->iter; k++){

		/*Intento de entrada na rexión crítica*/

		if( sem_wait(empty) != 0 ){ /*Decremento do valor do semáforo*/
						
			perror("Fallo no decremento do semáforo");

			exit(-1);
						
		}

		sleep(rand() % 3);

		if( sem_wait(mutex) != 0){ /*Decremento do valor do semáforo*/
						
			perror("Fallo no decremento do semáforo");
						
			exit(-1);

		}

		/*Rexión crítica*/
						
		printf("\n\nProdutor %d na execución %d\n", a->indice + 1, k+1);

		printf("\nEstado inicial do buffer: "); /*Imprímese o contido do buffer*/

		for(int j = 0; j < N; j++) printf("%d ", buffer[j]);
						
		item = id; /*Prodúcese o elemento*/

		buffer[conta] = item; /*Introdúcese o elemento no buffer*/

		conta++; /*Modifícase o número de elementos do buffer*/

		printf("\n\nBuffer despois da inserción: "); /*Imprímese o estado do buffer de novo*/

		for(int j = 0; j < N; j++) printf("%d ", buffer[j]);

		printf("\nActualmente hai %d elementos no buffer", conta);

		id++; /*O seguinte elemento producido tomará o valor id + 1*/

		/*Saída da rexión critica*/

		if( sem_post(mutex) != 0 ){ /*Incremento do valor do semáforo*/
						
			perror("Fallo no incremento do semáforo");

			exit(-1);	
						
		}

		sleep(rand() % 4);
						
		if( sem_post(full) != 0 ){ /*Incremento do valor do semáforo*/
						
			perror("Fallo no incremento do semáforo");

			exit(-1);
						
		}
					
	}

	pthread_exit(0);

}

/*Consumidor*/

void *consumidor (void *ptr){

	sleep(rand() % 5);

	struct args *a = (struct args*) ptr; /*Recóllese o argumento*/

	for (int k = 0; k < a->iter; k++){

		/*Intento de entrada na rexión crítica*/

		if( sem_wait(full) != 0 ){ /*Decremento do valor do semáforo*/
						
			perror("Fallo no decremento do semáforo");

			exit(-1);
						
		}

		sleep(rand() % 2);

		if( sem_wait(mutex) != 0){ /*Decremento do valor do semáforo*/
						
			perror("Fallo no decremento do semáforo");
						
			exit(-1);

		}

		/*Rexión crítica*/
						
		printf("\n\nConsumidor %d na execución %d\n", a->indice + 1, k+1);

		printf("\nEstado inicial do buffer: "); /*Imprímese o buffer no intre actual*/

		for(int j = 0; j < N; j++) printf("%d ", buffer[j]);

		buffer[conta - 1] = 0; /*Consúmese o elemento e actualízase o buffer*/

		conta--; /*Modifícase o número de elementos do buffer*/

		printf("\n\nBuffer despois da inserción: "); /*Imprímese o estado do buffer de novo*/

		for(int j = 0; j < N; j++) printf("%d ", buffer[j]);

		printf("\nActualmente hai %d elementos no buffer", conta);

		/*Saída da rexión critica*/

		if( sem_post(mutex) != 0 ){ /*Incremento do valor do semáforo*/
						
			perror("Fallo no incremento do semáforo");

			exit(-1);	
						
		}
			
		sleep(rand() % 5);			

		if( sem_post(empty) != 0 ){ /*Incremento do valor do semáforo*/
						
			perror("Fallo no incremento do semáforo");

			exit(-1);
						
		}
					
	}

	pthread_exit(0);

}

int main(int argc, char **argv){

	srand(14); /*Semilla fijada*/

	/*Elimínanse os semáforos creados na última execución do programa*/

	sem_unlink("EMPTY");

	sem_unlink("FULL");

	sem_unlink("MUTEX");

	/*Determínase o número de produtores e consumidores*/	

	if(argc == 3){ /*Por liña de comandos*/
	
		NPROD = atoi(argv[1]);

		NCONS = atoi(argv[2]);
	
	}

	else{ /*Aleatorio*/
	
		NPROD = (rand() % 15) + 1; /*Entre 1 e 15 produtores*/

		NCONS = (rand() % 15) + 1; /*Entre 1 e 15 consumidores*/ 
	
	}

	/*Declaración de variables*/

	struct args p[NPROD], c[NCONS]; /*Argumentos que van recibir os fíos*/

	pthread_t prod[NPROD], cons[NCONS]; /*Identificadores dos fíos a crear*/

	int status;

	int repartoprod = MAX, repartocons = MAX; /*Variables que almacenan o número de iteracións restantes a repartir entre produtores e consumidores, respectivamente*/

	/*Inicialización das estruturas e reparto de iteracións entre os fíos*/

	for(int i = 0; i < NPROD; i++){
	
		p[i].indice = i;

		if(i != NPROD - 1){
		       
			p[i].iter = (rand() % (repartoprod - (NPROD - (i + 1)))) + 1;

			repartoprod -= p[i].iter;

		}

		else p[i].iter = repartoprod; 
		
		printf("Número de iteracións do produtor %d = %d\n", i+1, p[i].iter);
	}

	for(int j = 0; j < NCONS; j++){
	
		c[j].indice = j;

		if( j != NCONS - 1){
		
			c[j].iter = (rand() % (repartocons - (NCONS - (j+1)))) + 1;

			repartocons -= c[j].iter;
		
		}

		else c[j].iter = repartocons;
	
		printf("\nNúmero de iteracións do consumidor %d = %d", j + 1, c[j].iter);
	}

	/*Inicialízanse os semáforos*/
		
	empty = sem_open("EMPTY", O_CREAT, 0700, N);

	full = sem_open("FULL", O_CREAT, 0700, 0);

	mutex = sem_open("MUTEX", O_CREAT, 0700, 1);

	/*Creación dos fíos*/

	for(int i = 0; i < NPROD; i++){
	
		if( (status = pthread_create(&prod[i], NULL, produtor, (void*)&p[i])) != 0){
		
			printf("pthread_create: Erro. Código: %d0\n", status);

			exit(-1);
		
		}
	
	}

	for(int j = 0; j < NCONS; j++){
	
		if( (status = pthread_create(&cons[j], NULL, consumidor, (void*)&c[j])) != 0){
		
			printf("pthread_create: Erro. Código: %d0\n", status);

			exit(-1);
		
		}
	
	}

	/*Reunión dos fíos*/

	for(int i = 0; i < NPROD; i++) pthread_join(prod[i], NULL);

	for(int j = 0; j < NCONS; j++) pthread_join(cons[j], NULL);

	/*Péchanse os semáforos*/

	sem_close(empty);

	sem_close(full);

	sem_close(mutex);


return(EXIT_SUCCESS);

}


