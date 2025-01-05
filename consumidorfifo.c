/*Práctica 5 - Consumidor con paso de mensaxes, versión FIFO*/

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

void consumidor(){

	char message[50]; /*Cadea na que se almacenan as mensaxes recibidas e se constrúen as mensaxes a enviar*/
	unsigned int prio = 0; /*Prioridade da mensaxe recibida*/

	struct mq_attr attr; /*Atributos da cola*/

	memset(message, 0, sizeof(message)); /*Mensaxe "baleira"*/

	for(int j = 0; j < MAX_BUFFER; j++){ /*Énchese a cola de mensaxes do produtor con mensaxes baleiras*/
	
		mq_send(almacen1, message, strlen(message), 1); 

	}

	for(int i = 0; i < MAX_DATOS; i++){ /*Bucle para a consumición dos elementos*/ 
		sleep(rand() % 2);
		mq_receive(almacen2, message, sizeof(message), &prio); /*Extráese da cola a mensaxe máis antiga (estamos no caso FIFO - First In First Out)*/
		mq_getattr(almacen2, &attr); /*Obtéñense as caracterísitcas da cola despois da extracción da mensaxe*/
		if(message != NULL) printf("\n%s. Hai %ld mensaxes na cola\n", message, attr.mq_curmsgs); /*Imprímese a mensaxe recibida e o número de mensaxes restantes na cola*/
		sprintf(message, "Elemento %d consumido", i+1); /*Constrúese a mensaxe de confirmación de que se consumiu o elemento*/
		mq_send(almacen1, message, strlen(message), 1); /*Envíase a mensaxe de confirmación, prioridade 1 sempre - así, extraerase primeiro a máis antiga*/
		sleep(rand() % 5);
	}
	
}

void main(){

	almacen1 = mq_open("/ALMACEN1", O_WRONLY, 0777, 0); /*Ábrese a cola do produtor (xa creada) con permisos de escritura, pois vánselle enviar as mensaxes*/
	almacen2 = mq_open("/ALMACEN2", O_RDONLY, 0777, 0); /*Ábrese a cola do consumidor (xa creada) con permisos de lectura, pois vanse extraer mensaxes dela*/

	consumidor(); /*Execútase o procedemento do consumidor*/

	/*Péchanse os buffers*/
	mq_close(almacen1);
	mq_close(almacen2);

	exit(EXIT_SUCCESS);

}
