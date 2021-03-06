/* Ordenacao multithread
 *
 * Le da entrada padrao varios numeros inteiros positivos de tamanhao 
 * e ordena-os utilizando uma implementacao multithread de mergesort
 *
 * Victor Ferrao Santolim
 * RA 187888
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


//Numero maximo de niveis que sao disparados com novos threads na recursao
#define MAX_THREADS_NIVEL 2

//Prototipo da funcao mergesort que as threads executam
void *mergeSort(void * args);

//Struct onde são passados os argumentos para os threads
typedef struct {
    unsigned int ini;
    unsigned int fim;
    unsigned int nivel;
} limitesStruct;

unsigned int entrada[100000];
unsigned int aux[100000];

int main() {

	unsigned int indice = 0;
	unsigned int numLido; //Leitura atual

    //Pega os números da entrada e coloca na memória
	do
	{
       scanf("%u", &numLido); //Pega um numero da entrada

       entrada[indice] = numLido;
       indice++;

	}while(getchar() != 10); //Enquanto nao pega um \n


	limitesStruct *args = malloc(sizeof (limitesStruct));
	args->ini = 0;
	args->fim = indice-1;
	args->nivel = 1;

  	//Faz a primeira chamada recursiva do MergeSort, sem Threads
  	mergeSort(args);

    //Le o vetor já ordenado e imprime na tela
    for(unsigned int i = 0 ; i < indice-1 ; i++)
    {
    	printf("%u ", entrada[i]);

    }
    printf("%u\n" , entrada[indice-1]);

    return 0;

}

//Funcao que eh executada nos threads
//Calcula recursivamente o mergesort criando novas threads
void *mergeSort(void * args)
{
	//Coloca as informacoes passadas pela struct
	//apontada pelo parametro recebido em variaveis locais
	limitesStruct * limites = args;
	unsigned int ini = limites->ini;
    unsigned int fim = limites->fim;
    unsigned int nivel = limites->nivel;
   	free(limites); //Libera a memória

   	//Condicao de continuidade da recursao
	if(ini < fim)
	{
		
		//Elemento mediano da parte do vetor recebida
		unsigned int m = (ini+fim)/2;

		//Define os limites para as próximas chamadas recursivas
		limitesStruct *esq = malloc(sizeof (limitesStruct));
		esq->ini = ini;
		esq->fim = m;
		esq->nivel = nivel+1;

		limitesStruct *dir = malloc(sizeof (limitesStruct));
		dir->ini = m+1;
		dir->fim = fim;
		dir->nivel = nivel+1;
		
		//Se o nvel maior do que o maximo, nao cria threads
		if(nivel >= MAX_THREADS_NIVEL)
		{
			//Faz a recursão sem criação de novas threads
			mergeSort(esq);
			mergeSort(dir);

		}
		else //Nivel mais baixo que o maximo, cria threads
		{
			
			pthread_t threadEsq, threadDir;

			//Chama recursivamente o mergesort com criacao de novas threads
			pthread_create(&threadEsq, NULL, mergeSort, esq);
			pthread_create(&threadDir, NULL, mergeSort, dir);

			//Espera elas retornarem, para poder comecar a intercacalacao
			pthread_join(threadEsq, NULL);
			pthread_join(threadDir, NULL);
		}


		//Realiza o merge das duas metades
		//Compara elemento a elemento dos dois subvetores ordenados
		//Colocando sempre o de menor valor antes
		unsigned int ie, id , i;
		for(ie = ini , id = m+1 , i = ini ; ie <= m && id <= fim ; i++)
	    {
	    	if(entrada[ie] <= entrada[id])
	      	{
	       		aux[i] = entrada[ie];
	        	ie++;
	      	}
	      	else
	      	{
	        	aux[i] = entrada[id];
	        	id++;
	      	}
	    }

	    while(ie <= m)
	    {
	      	aux[i] = entrada[ie];
	      	i++;
	      	ie++;
	    }

	    while(id <= fim)
	    {
	      	aux[i] = entrada[id];
	      	i++;
	      	id++;
	    }

	    for(i = ini ; i <= fim ; i++)
	    {
	      	entrada[i] = aux[i];
	    }
	}
	
	return NULL;
}