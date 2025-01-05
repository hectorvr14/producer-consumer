/*Exercicio 1 - Consumidor*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define N 10

int main(int argc, char **argv){

	struct stat buffer; /*Buffer do ficheiro*/

	int fd; /*File descriptor*/

	fd = open("produtos.txt", O_RDWR | O_CREAT); /*A función open recibe como argumentos a ruta do ficheiro e o modo no que se abre (neste caso lectura e escritura), que se pode usar en combinación con outras opcións (por exemplo, crear o ficheiro se non existe xa)*/

	ftruncate(fd, (N+1)*sizeof(int)); /*Dáselle un tamaño ó ficheiro*/

	int *mapeo = NULL; /*Punteiro á proxección de memoria do ficheiro*/

	if( fstat(fd, &buffer) == 0 ){
	
		if( (mapeo = mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
		
			perror("Erro de mapeo do ficheiro en memoria"); /*Mensaxe en caso de erro*/

			exit(-1);
		
		}
	
		else{
		
			int *conta = mapeo + N; /*A variable que leva a conta do número de produtos é compartida*/

			while(1){
			
				sleep(0.75);

				if(*conta == 0) sleep(1.25); /*Dórmese o proceso mentres non haxa produtos*/
				else{

					sleep(0.5);

					printf("Buffer antes da extracción: ");

					for(int i = 0; i < N; i++) printf("%d ", *(mapeo + i));	
					*(mapeo + (*conta - 1)) = 0; /*Consúmese o último elemento do buffer*/
					
					sleep(1.35);

					(*conta)--; /*Actualízase o número de elementos*/

					sleep(1);

					printf("\nBuffer despois da extracción: ");

					for(int i = 0; i < N; i++) printf("%d ", *(mapeo + i));	
					printf("\nElemento consumido. Hai %d elementos\n", *conta);
		
				}
					
			
			}
		
			munmap(mapeo, buffer.st_size); /*Elimínase a zona de memoria compartida*/
		
		}

		close(fd); /*Péchase o ficheiro*/
	
	}

}
