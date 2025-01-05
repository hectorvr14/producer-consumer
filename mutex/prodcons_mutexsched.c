/*Práctica 4 - O problema do produtor-consumidor solucionado con mutexes e sen variables de condición - Versión 2*/

/*COMENTARIOS A ESTA VERSIÓN

A versión 1 desta práctica optativa baséase na comprobación do número de elementos no buffer de cara ao bloqueo do fío, i.e. substitúese o mecanismo da variable de condición por unha estrutura condicional if-else que permite que o fío desenvolva, ou non, a súa acción. Agora ben, non se contempla a posibilidade de que o planificador de fíos, despois da execución de pthread_mutex_unlock, teña unha estratexia tal que decida outorgarlle sempre a CPU a un mesmo fío (aínda que nas execucións feitas por min nunca tivo lugar).

Por esta razón, para garantir que tal circunstancia non suceda, fíxose esta nova versión, na que se determina a estratexia de planificación de fíos a seguir, segundo se detalla nos manuais das bibliotecas <pthread.h> e <sched.h>

*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>

/*Definición do elemento*/

struct elemento{
    
    int valor; /*Contido do elemento*/
    
    struct elemento *sig; /*Punteiro ao seguinte elemento do buffer*/

};

/*Estrutura dos argumentos que se lle pasarán a cada fío*/

struct args{

	int indice;

	int ITER; /*Número de iteracións*/

};

typedef struct elemento item; /*Nome abreviado do tipo de dato*/

/*Definición de constantes*/

#define N 10 /*Tamaño máximo do buffer predeterminado*/
#define MAX 100 /*Número máximo de elementos a producir*/

/*Variables globais*/

int conta = 0; /*Número de elementos que se teñen actualmente no buffer*/
item *buffer = NULL; /*Buffer empregado entre o produtor e o consumidor*/
item *ultimo = NULL; /*Punteiro ao último elemento producido*/

int NPROD; /*Número de produtores*/
int NCONS; /*Número de consumidores*/

pthread_mutex_t the_mutex; /*Mutex - O fío que o posúa está na rexión crítica*/

int n,max;

int id = 1; /*Contido do elemento*/

/*Procedementos a executar polos fíos*/

void *produtor(void *ptr){

	sleep(rand() % 5);

	struct args *a = (struct args*) ptr; /*Recóllese o argumento*/
        
        item *x = NULL; /*Punteiro auxiliar*/

	for(int k = 0; k < a->ITER; k++){
	
		printf("\n\nProdutor %d na execución %d\n", a->indice + 1, k+1);

		pthread_mutex_lock(&the_mutex); /*Inténtase pechar o mutex - Se xa está pechado, bloquéase*/

		if( conta != n ){

			/*------------------------------------------------------------*/

			printf("\nEstado inicial do buffer: ");

			if(conta != 0){
		            
		            x = buffer; 
		            
		            while(x != NULL){
		            
		                printf("%d ", x->valor);
		                
		                x = x->sig; 
		            
		            }
		        
		        }
		        else printf("Buffer baleiro");

			/*Produción e inserción dun novo elemento*/

			if(conta == 0){ /*Se aínda non hai ningún elemento*/
		        
		            buffer = (item*)malloc(sizeof(item)); /*Creación e inserción do elemento no buffer*/
		            
		            /*Actualización do punteiro*/
		            
		            ultimo = buffer;
		            
		            /*Inicialización do elemento*/
		            
		            buffer->valor = id;
		            buffer->sig = NULL;

			    id++; /*O contido do seguinte elemento producido será diferente*/
		        
		        }
		        
		        else{ /*Se xa hai elementos*/
		        
		            ultimo->sig = (item*)malloc(sizeof(item));
		            
		            ultimo = ultimo->sig;
		            
		            ultimo->valor = id;
		            ultimo->sig = NULL;
		       
			    id++; /*O contido do seguinte elemento producido será diferente*/

		        }
		        
		        conta++; /*Actualízase o tamaño do buffer*/

			/*Impresión do estado do buffer despois da inserción*/

			printf("\n\nBuffer despois da inserción: ");

		        x = buffer; 
		            
		        while(x != NULL){
		            
		        printf("%d ", x->valor);
		                
		        x = x->sig; 
		        
		        }
	 
			printf("\nActualmente hai %d elementos no buffer", conta);

			printf("\n\n/*-----------------------------------------------------------*/");

			/*------------------------------------------------------------*/
			
		}

		else k--;

		pthread_mutex_unlock(&the_mutex); /*Libérase o acceso ó buffer - Se hai fíos bloqueados polo mutex, a estratexia de planificación decide a que fío ceder a CPU*/

		sleep(rand() % 5);

	}

	pthread_exit(0);
}

void *consumidor(void *ptr){

	sleep(rand() % 5);

	struct args *a = (struct args*) ptr; /*Recóllese o argumento*/
 
        item *x = NULL; /*Punteiro auxiliar*/
        
	for(int k = 0; k < a->ITER; k++){

		printf("\n\nConsumidor %d na execución %d\n", a->indice + 1, k+1);

		pthread_mutex_lock(&the_mutex); /*Inténtase pechar o mutex - Se xa está pechado, bloquéase*/
		
		if( conta != 0 ){
		
			/*------------------------------------------------------------*/

			printf("\nEstado inicial do buffer: ");

		        x = buffer; 
		            
		        while(x != NULL){
		            
		        printf("%d ", x->valor);
		                
		        x = x->sig; 
		        
		        }

			/*Extracción e consumición dun elemento do buffer*/

			item *aux;
		        
		        aux = buffer;
		        
		        buffer = buffer->sig;
		        
		        free(aux);

			conta--; /*Actualízase o número de elementos*/
		        
		        if(conta == 0) ultimo = NULL;

			/*Impresión do buffer despois da extracción*/

			printf("\n\nBuffer despois da extracción: ");

			if(conta != 0){
		        
		            x = buffer; 

		            while(x != NULL){

		            printf("%d ", x->valor);

		            x = x->sig; 

		            }              
		        
		        }
		        else printf("Buffer baleiro");
		        
			printf("\n Actualmente quedan %d elementos no buffer", conta);

			printf("\n\n/*-----------------------------------------------------------*/");

			/*------------------------------------------------------------*/

		}

		else k--;

		pthread_mutex_unlock(&the_mutex); /*Libérase o acceso ó buffer - Se hai fíos bloqueados polo mutex, a estratexia de planificación decide a que fío ceder a CPU*/

		sleep(rand() % 5);
	}

	pthread_exit(0);
}

int main(int argc, char **argv){

        srand(time(NULL)); /*Semilla*/
    
	/*Declaración de variables*/

	if(argc == 3){
	
		n = atoi(argv[1]);

		max = atoi(argv[2]);
	
	}

	else{
	
		n = N; 

		max = MAX;
	
	}

	NPROD = (rand() % 5) + 1; /*Entre 1 e 5 produtores*/
        NCONS = (rand() % 4) + 1; /*Entre 1 e 4 consumidores*/ 
	
	struct args p[NPROD], c[NCONS];

	pthread_t prod[NPROD], cons[NCONS]; /*Identificadores dos fíos a crear*/

	int status;

	int repartoprod = max, repartocons = max; /*Variables que almacenan o número de iteracións restantes a repartir entre produtores e consumidores, respectivamente*/

	pthread_attr_t attr;

	struct sched_param param;

	/*Inicialización dos atributos dos fíos*/

	pthread_attr_init(&attr); /*Inicialización da estrutura*/

	param.sched_priority = 1; /*Tódolos fíos coa mesma prioridade*/

	pthread_attr_setschedpolicy(&attr, SCHED_FIFO); /*Planificación FIFO - Os fíos organízanse nunha cola sen prioridade*/

	pthread_attr_setschedparam(&attr, &param); /*Asígnase a prioridade ó fío*/

	pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED); /*Os fíos herdarán os atributos do seu fío creador*/

	/*Inicialización de mutex e reparto das iteracións*/

	pthread_mutex_init(&the_mutex, 0); 

	for(int i = 0; i < NPROD; i++){
	
		p[i].indice = i;

		if(i != NPROD - 1){
		       
			p[i].ITER = (rand() % (repartoprod - (NPROD - (i + 1)))) + 1;

			repartoprod -= p[i].ITER;

		}

		else p[i].ITER = repartoprod; 
		
		printf("Número de iteracións do produtor %d = %d\n", i+1, p[i].ITER);
	}

	for(int j = 0; j < NCONS; j++){
	
		c[j].indice = j;

		if( j != NCONS - 1){
		
			c[j].ITER = (rand() % (repartocons - (NCONS - (j+1)))) + 1;

			repartocons -= c[j].ITER;
		
		}

		else c[j].ITER = repartocons;
	
		printf("\nNúmero de iteracións do consumidor %d = %d", j + 1, c[j].ITER);
	}

	/*Creación dos fíos*/

	for(int i = 0; i < NPROD; i++){
	
		if( (status = pthread_create(&prod[i], &attr, produtor, (void*)&p[i])) != 0){
		
			printf("pthread_create: Erro. Código: %d0\n", status);

			exit(-1);
		
		}
	
	}

	for(int j = 0; j < NCONS; j++){
	
		if( (status = pthread_create(&cons[j], &attr, consumidor, (void*)&c[j])) != 0){
		
			printf("pthread_create: Erro. Código: %d0\n", status);

			exit(-1);
		
		}
	
	}

	/*Reunión dos fíos*/

	for(int i = 0; i < NPROD; i++) pthread_join(prod[i], NULL);

	for(int j = 0; j < NCONS; j++) pthread_join(cons[j], NULL);

	/*Destrución do mutex e dos atributos do fío*/

	pthread_mutex_destroy(&the_mutex);
	
	pthread_attr_destroy(&attr);

return(EXIT_SUCCESS);

}
