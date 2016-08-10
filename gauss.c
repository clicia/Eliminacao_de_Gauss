/**
  * "Soma de vetor usando sinais" para a disciplina sistemas operacionais
  * Universidade Federal da Bahia (UFBA)
  * Departamento de Ciência da Computacao (DCC)
  * by: Clícia dos Santos Pinto
  */
    
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>

float **le_matriz( int linha, int coluna){   
    float **mat = (float **) malloc(sizeof(float *)*linha);
    
    if(mat == NULL){
        printf("ERRO DE ALOCAÇÃO DA MATRIZ\n");
        return;
    }

    int i=0,j=0;
    for(i=0; i<linha; i++)    
        mat[i] = (float *) malloc(sizeof(float)*coluna);
        if(mat[i] == NULL){
            printf("ERRO DE ALOCAÇÃO DA MATRIZ \n");
            return;
        }
       for(i=0; i<linha; i++){
           for(j=0; j<coluna; j++){
               scanf("%f",&mat[i][j]);
           }
       }
    return mat;
}

float *le_vetor(int size){
    float *vet = NULL;
    int i;
    vet = (float *) malloc(sizeof(float)*size);
    if(vet == NULL){
        printf("ERRO NA ALOCAÇÃO DO VETOR \n");
        return;
    }

    for(i=0; i<size; i++){
       scanf("%f",&vet[i]);
    }

    return vet;
}

void free_matrix(int linha, float **mat){
    int i=0;
    for(i=0;i<linha;i++)    
        free(mat[i]);
    free(mat);
}

void free_vector(float *vet){
    free(vet);
}


int main(int argc[], char *argv[]){
	int dimensao;
	float **matriz;
    float *vetorIndependente;
    
    int i, j, t, l, status_t, tam, *vet;
	int linha = 0;
	pid_t pid = 1;


    printf("Digite o dimensao da matriz seguido dos elementos e do vetor independente:");
    scanf("%d", &dimensao);

    int TAM_BUFFER = dimensao+3;	
	
	matriz = le_matriz(dimensao, dimensao);

    vetorIndependente = le_vetor(dimensao);

    //criação dos filhos
	
	int REF = 1;
	int inicio = 0;
	
	while(REF != dimensao){
		int fd[2];
		float buffer[TAM_BUFFER];
		if(pipe(fd) == -1) printf("Erro na funcao que cria o descritor de arquivo!\n");
		
		for(i=0; i<(dimensao-REF); i++){
			if(pid!=0){
				sleep(10);
				pid = fork();
				inicio = i+REF;		// inicio vai determinar a linha que será usada como referencia
			}
		}

		if(pid == 0){
		float constante = (matriz[inicio][REF-1]) / (matriz[REF-1][REF-1]);
		float *vetor_auxiliar;
		vetor_auxiliar =  calloc(dimensao+2,sizeof(float));
		
		int w = REF-1;
		for(i=(REF-1); i<dimensao; i++){
				vetor_auxiliar[w] = matriz[inicio][i] - ((matriz[REF-1][i])*constante);
				w++;
		}
		vetor_auxiliar[w] = vetorIndependente[inicio] - ((vetorIndependente[REF-1])*constante);
		vetor_auxiliar[w+1] = inicio;
		
		close(fd[0]);
		write(fd[1], vetor_auxiliar, (TAM_BUFFER)*sizeof(float));
		exit(0);
		sleep(5);
		}
		

		// FIM DA EXECUÇÃO DOS FILHOS


		if (pid != 0){
			close(fd[1]);
			
			for (l=0; l<dimensao-REF; l++){
				read(fd[0], buffer, sizeof(buffer));
				
				linha = (buffer[dimensao+1]);
				int y = 0;
				for(i=0; i<dimensao+1; i++){
					matriz[linha][i] = buffer[y];
					y++;
				}
				vetorIndependente[linha] = buffer[dimensao];
			}

		}
		REF += 1;
	}
	sleep(5);

	// REALIZANDO A ELIMINAÇÃO

	float *vetor_resultados = NULL;
    
    vetor_resultados = (float *) calloc(dimensao, sizeof(float));
	float soma = 0;

	for (i = dimensao; i>0; i=i-1){
		soma = vetorIndependente[i-1];
	
		for (j=i; j<dimensao; j++){
			soma += (matriz[i-1][j]*vetor_resultados[j]) * (-1);
		}
	
		vetor_resultados[i-1] = soma / matriz[i-1][i-1];
	}

	// REALIZANDO A ELIMINAÇÃO

	printf("\n");
	
	//Imprimindo o vetor resultado
	for(i=0; i<dimensao; i++){
        printf("%f ",vetor_resultados[i]);
	}
	
	free_matrix(dimensao, matriz);
	free_vector(vetorIndependente);
	

	return 0;

}