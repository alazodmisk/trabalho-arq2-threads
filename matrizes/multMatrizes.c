#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#define NUM_THREADS 8

typedef struct {
    int inicio;          /* primeira linha (inclusive) */
    int fim;             /* última linha (exclusive)   */
    int n;
    const int *A;
    const int *B;
    long long *C;
} Tarefa;


/*
    Percorre cada elemento por linha de A e coluna de B, multiplicando-os e somando ate chegar no
    fim de ambos, guardando a soma na mesma posicao porem em C. Com mais threads cada uma se 
    responsabiliza por uma fatia das linhas da matriz A.
*/
void *multiplicaMatriz(void *arg)
{
    Tarefa *t = (Tarefa *)arg;
    int n = t->n;

    for (int i = t->inicio; i < t->fim; i++) {
        for (int j = 0; j < n; j++) {
            long long soma = 0;
            for (int k = 0; k < n; k++) {
                soma += (long long)t->A[i * n + k] * t->B[k * n + j];
            }

            t->C[i * n + j] = soma;
        }
    }
    return NULL;
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
        fprintf(stderr, "Matriz %s nao quadrada: %ld elementos\n", caminho, total);
        fclose(f);
        return NULL;
    }

    /* 2a passada: lê os valores */
    rewind(f);
    int *vetor = malloc((size_t)total * sizeof(int));
    if (!vetor) {
        fprintf(stderr, "Erro de memoria.\n");
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


int escreve_matriz(const char *caminho, const long long *m, int n)
{
    FILE *f = fopen(caminho, "w");
    if (!f) {
        perror("Erro ao criar o arquivo de saida");
        return 0;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (j > 0) fputc(' ', f);
            fprintf(f, "%lld", m[i * n + j]);
        }
        fputc('\n', f);
    }

    fclose(f);
    return 1;
}

int main(int argc, char *argv[])
{
    if (argc < 3 || argc > 4) {
        fprintf(stderr,
            "Uso: %s matrizA.txt matrizB.txt [saida.txt]\n", argv[0]);
        return 1;
    }

    const char *arquivoA = argv[1];
    const char *arquivoB = argv[2];
    const char *arquivoSaida = (argc == 5) ? argv[4] : "saida.txt";

    int nA, nB;
    
    int *A = le_matriz(arquivoA, &nA);
    if (!A) { fprintf(stderr, "Erro na leitura da matriz A\n"); free(A); return 1; }
    int *B = le_matriz(arquivoB, &nB);
    if (!B) { fprintf(stderr, "Erro na leitura da matriz A\n"); free(A); free(B); return 1; }
    
    if (nA != nB) { fprintf(stderr, "Matriz de tamanhos diferentes\n"); free(A); free(B); return 1; }

    long long *C = malloc((size_t)n * nA * sizeof(long long));
    if (!C) { fprintf(stderr, "Erro de memoria.\n"); free(A); free(B); return 1; }

    
    pthread_t threads[NUM_THREADS];
    Tarefa tarefas[NUM_THREADS];

    int base = nA / NUM_THREADS;
    int resto = nA % NUM_THREADS;
    int atual = 0;

    struct timespec inicio, fim;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (int i = 0; i < NUM_THREADS; i++) {
        int qtd = base + (i < resto ? 1 : 0);
        tarefas[i] = (Tarefa) { atual, atual + qtd, n, A, B, C };
        atual += qtd;
        pthread_create(&threads[i], NULL, multiplicaMatriz, &tarefas[i]);
    }
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }
        
    clock_gettime(CLOCK_MONOTONIC, &fim);

    double tempo = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("Tempo de execucao: %.6f segundos\n", tempo);
    printf("Ordem das matrizes: %d x %d\n", nA, nA);

    if (!escreve_matriz(arquivoSaida, C, n)) fprintf(stderr, "Falha ao gravar o resultado.\n");
    free(A);
    free(B);
    free(C);
    return 0;
}
