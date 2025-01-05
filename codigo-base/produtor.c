/*Exercicio 1 - Produtor*/

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

	fd = open("produtos.txt", O_RDWR | O_CREAT); /*A función open recibe como argumentos a ruta do ficheiro e modo no que se abre (neste caso lectura e escritura), que se pode usar en combinación con outras opcións (por exemplo, crear o ficheiro se non existe xa)*/

	ftruncate(fd, (N+1)*sizeof(int)); /*Dáselle un tamaño ó ficheiro*/

	int *mapeo = NULL; /*Punteiro á proxección de memoria do ficheiro*/

	if( fstat(fd, &buffer) == 0 ){ 
		
		if( (mapeo = mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
		
			perror("Erro de mapeo do ficheiro en memoria"); /*Mensaxe en caso de erro*/

			exit(-1);
		
		}
	
		else{
		
			for(int i = 0; i <= N; i++){ /*Este bucle forza a que se execute primeiro o produtor, porque inicializa a memoria compartida*/
			
				*(mapeo + i) = 0; /*Inicialízase a zona de memoria compartida - ao principio non temos produtos. En cada posición de memoria (menos a última), 0 significa que non hai nada e 1 que hai un produto.*/
			
			}

			int *conta = mapeo + N; /*A variable que leva a conta do número de produtos é compartida*/

			while(1){	

				int a = 1; /*Produción dun elemento*/ 

				sleep(1);

				if(*conta == N) sleep(1); /*Se o buffer está cheo, dórmese o produtor*/
					
				else{
					sleep(1);
			
					printf("Buffer antes da inserción: ");

					for(int i = 0; i < N; i++) printf("%d ", *(mapeo + i));
					*(mapeo + *conta) = a; /*Se o buffer non está cheo, introdúcese o elemento na posición correspondente*/
					
					sleep(1);

					(*conta)++; /*Actualízase o número de elementos do buffer*/

					sleep(1);

					printf("\nBuffer despois da inserción: ");

					for(int i = 0; i < N; i++) printf("%d ", *(mapeo + i));

					printf("\nElemento producido. Hai %d elementos\n", *conta);
		

				}		
		
			}
		
			munmap(mapeo,buffer.st_size); /*Péchase a proxección en memoria do ficheiro*/

		}	
	
	}


}
