#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>


#define TOTAL 12 /* ordem da matriz */
#define NUM_THREADS 1

typedef struct {
    int id;
    int inicio;
    int fim;
} Tarefa;


/*
    aplicar um filtro de média 3 × 3 em uma imagem em escala de cinza. Calcular
    cada pixel de saída a partir de sua vizinhança. Os pixels das bordas deverão 
    permanecer inalterados.
*/
int aplicaFiltro(){
    return 0
}



/* 
    Lê uma matriz QUADRADA de ordem qualquer de um txt (valores separados
    por espaço/quebra de linha) e devolve um vetor alocado dinamicamente,
    linha após linha: elemento (i, j) = vetor[i * n + j].
    A ordem n é descoberta contando os números do arquivo (total = n*n).
    Retorna NULL em caso de erro. 
*/
int *le_matriz(const char *caminho, int *n_out)
{
    FILE *f = fopen(caminho, "r");
    if (!f) {
        perror("Erro ao abrir o arquivo");
        return NULL;
    }
 
    /* 1a passada: conta quantos números existem */
    long total = 0;
    int tmp;
    while (fscanf(f, "%d", &tmp) == 1)
        total++;
 
    long n = (long)(sqrt((double)total) + 0.5);
    if (total == 0 || n * n != total) {
        fprintf(stderr, "Matriz nao quadrada: %ld elementos\n", total);
        fclose(f);
        return NULL;
    }
 
    /* 2a passada: lê os valores */
    rewind(f);
    int *vetor = malloc((size_t)total * sizeof(int));
    if (!vetor) {
        fprintf(stderr, "Erro de memoria\n");
        fclose(f);
        return NULL;
    }
    for (long i = 0; i < total; i++) {
        if (fscanf(f, "%d", &vetor[i]) != 1) {
            fprintf(stderr, "Erro ao ler o elemento %ld\n", i);
            free(vetor);
            fclose(f);
            return NULL;
        }
    }
 
    fclose(f);
    *n_out = (int)n;
    return vetor;
}


void imprime_matriz(const int *m, int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%4d", m[i * n + j]);
        printf("\n");
    }
}
 

int main(int argc, char *argv[])
{
    const char *arquivo = (argc > 1) ? argv[1] : "imagemMatriz256.txt";
 
    int n;
    int *m = le_matriz(arquivo, &n);
    if (!m) return 1;

    /* imprime_matriz(m, n); */
 
    printf("Ordem da matriz: %d x %d\n", n, n);
    printf("m[0][0]         = %d\n", m[0]);
    printf("m[n-1][n-1]     = %d\n", m[(n - 1) * n + (n - 1)]);

    /* cria as threads */
    pthread_t threads[NUM_THREADS];
    Tarefa tarefas[NUM_THREADS];

    struct timespec inicio, fim;

    int tamanho = TOTAL / NUM_THREADS;

    /* Marca o início da medição */
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (int i = 0; i < NUM_THREADS; i++) {
        tarefas[i].id = i;
        tarefas[i].inicio = i * tamanho;
        tarefas[i].fim = (i + 1) * tamanho;

        pthread_create(
            &threads[i], NULL, aplicaFiltro, &tarefas[i]
        );
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Calcula o tempo decorrido em segundos */
    double tempo =
        (fim.tv_sec - inicio.tv_sec) +
        (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    printf("Tempo de execucao: %.6f segundos\n", tempo);
 
    free(m);
    return 0;
}