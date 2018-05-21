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


//Numero maximo de threads que podem ser disparadas no merge sort
#define MAX_THREADS 4

//Prototipo da funcao mergesort que as threads executam
void *mergeSort(void * args);

//Struct onde são passados os argumentos para os threads
typedef struct {
    unsigned int ini;
    unsigned int fim;
    char maisThreads;
} limitesStruct;

//Trava para evitar crace condition
pthread_mutex_t trava;

unsigned int entrada[1000];
unsigned int aux[1000];
unsigned int numThreads;

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
	args->maisThreads = 1;

  	//Faz a primeira chamada recursiva do MergeSort, sem Threads
  	mergeSort(args);

  	
    //Le o vetor já ordenado e imprime na tela
    for(unsigned int i = 0 ; i < indice ; i++)
    {
    	printf("%u ", entrada[i]);

    }
    printf("\n");

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
    char maisThreads = limites->maisThreads;
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

		limitesStruct *dir = malloc(sizeof (limitesStruct));
		dir->ini = m+1;
		dir->fim = fim;
		
		//Se foi recebido comando de nao criar mais threads
		if(maisThreads == 0)
		{
			//Faz a recursão sem criação de novas threads
			esq->maisThreads = 0;
			dir->maisThreads = 0;

			mergeSort(esq);
			mergeSort(dir);

		}
		else //Foi recebido comando de criação de novas threads
		{
			
			//Verifica se o numero de threads existentes
			pthread_mutex_lock(&trava);
			if(numThreads >= MAX_THREADS)
			{
				pthread_mutex_unlock(&trava);

				//Se ultrapassou o número de threads máximas,
				//faz as proximas chamadas sem recursão
				esq->maisThreads = 0;
				dir->maisThreads = 0;
				mergeSort(esq);
				mergeSort(dir);
			}
			else //Faz recursao com criacao de threads
			{
				numThreads += 2;
				pthread_mutex_unlock(&trava);

				esq->maisThreads = 1;
				dir->maisThreads = 1;

				pthread_t threadEsq, threadDir;

				pthread_create(&threadEsq, NULL, mergeSort, esq);
				pthread_create(&threadDir, NULL, mergeSort, dir);

				pthread_join(threadEsq, NULL);
				pthread_join(threadDir, NULL);

				pthread_mutex_lock(&trava);
				numThreads -= 2;
				pthread_mutex_unlock(&trava);
			}
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