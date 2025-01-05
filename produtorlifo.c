/*Práctica 5 - Produtor LIFO con paso de mensaxes*/

#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>

/*Definición de constantes*/

#define MAX_BUFFER 6 /*Tamaño do buffer*/
#define MAX_DATOS 100 /*Número de datos que se van producir e consumir*/

/*Declaración de variables globais*/

mqd_t almacen1; /*Cola de entrada de mensaxes para o produtor*/
mqd_t almacen2; /*Cola de entrada de mensaxes para o consumidor*/

void produtor(){

	char message[50]; /*Cadea na que se almacenan as mensaxes recibidas e se constrúen as mensaxes a enviar*/
	unsigned int prio = 0; /*Prioridade da mensaxe recibida*/

	struct mq_attr attr; /*Atributos da cola*/

	do{ /*Monitorízase a cola de mensaxes - Non se comezarán a producir os elementos ata que a cola estea chea coas mensaxes do consumidor*/
	
		mq_getattr(almacen1, &attr);
	
	}while(attr.mq_curmsgs != MAX_BUFFER);

	for(int i = 0; i < MAX_DATOS; i++){ /*Comézanse a producir os elementos*/

		sleep(rand() % 3);
		mq_receive(almacen1, message, sizeof(message), &prio); /*Extráese a mensaxe recibida da cola*/
		mq_getattr(almacen1, &attr); /*Obtéñense as características da cola de mensaxes despois da extracción*/
		if(message != NULL) printf("\n%s. Hai %ld mensaxes na cola\n", message, attr.mq_curmsgs); /*Imprímese a mensaxe recibida e o número de mensaxes restantes*/
		sprintf(message, "Elemento %d producido", i+1); /*Constrúese a mensaxe a enviar, xunto co elemento producido*/
		mq_send(almacen2, message, strlen(message), i+1);/*Envíase a mensaxe con prioridade crecente - así, a cola de mensaxes do consumidor presentará un comportamento LIFO, pois o último en chegar é o que ten máis prioridade e, polo tanto, será o primeiro que se extraia*/
		sleep(rand() % 2);

	}

	/*Iteracións restantes para os 6 últimos elementos. O bucle anterior non basta porque nel recíbense as 6 primeiras mensaxes baleiras e 94 confirmacións de elementos consumidos, quedando pendentes outras 6, que se teñen en conta aquí*/
	for(int i = 0; i < MAX_BUFFER; i++){
		sleep(rand() % 5);
		mq_receive(almacen1, message, sizeof(message), &prio);
		mq_getattr(almacen1, &attr);
		if(message != NULL) printf("\n%s. Hai %ld mensaxes na cola\n", message, attr.mq_curmsgs); 
	}

}

void main(){

	struct mq_attr attr; /*Atributos da cola de mensaxes*/

	/*Asignación de atributos*/
	attr.mq_maxmsg = MAX_BUFFER;
	attr.mq_msgsize = 50*sizeof(char);

	/*Bórranse os buffers de entrada por se xa existían*/
	mq_unlink("/ALMACEN1");
	mq_unlink("/ALMACEN2");

	/*Ábrense os buffers*/
	almacen1 = mq_open("/ALMACEN1", O_CREAT|O_RDONLY, 0777, &attr); /*Permisos de lectura - Extracción de mensaxe*/
	almacen2 = mq_open("/ALMACEN2", O_CREAT|O_WRONLY, 0777, &attr); /*Permisos de escritura - Envío de mensaxe*/

	if((almacen1 == -1) || (almacen2 == -1)){
	
		perror("mq_open");
		exit(EXIT_FAILURE);
	
	}

	produtor();

	/*Péchanse os buffers*/
	mq_close(almacen1);
	mq_close(almacen2);

	exit(EXIT_SUCCESS);

}
