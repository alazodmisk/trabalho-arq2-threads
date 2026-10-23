#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

#define TOTAL 1024 /* ordem da matriz */
#define NUM_THREADS 2

typedef struct {
    int id;
    int inicio;          /* primeira linha (inclusive) */
    int fim;             /* última linha (exclusive)   */
    int n;
    const int *entrada;
    int *saida;
} Tarefa;


/*
    aplicar um filtro de média 3 × 3 em uma imagem em escala de cinza. Calcular
    cada pixel de saída a partir de sua vizinhança. Os pixels das bordas deverão permanecer
    inalterados.
*/
void *aplicaFiltro(void *arg)
{
    Tarefa *t = (Tarefa *)arg;
    int n = t->n;

    for (int i = t->inicio; i < t->fim; i++) {
        for (int j = 1; j < n - 1; j++) {
            int soma = 0;
            for (int di = -1; di <= 1; di++)
                for (int dj = -1; dj <= 1; dj++)
                    soma += t->entrada[(i + di) * n + (j + dj)];
            t->saida[i * n + j] = soma / 9;
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

int escreve_matriz(const char *caminho, const int *m, int n)
{
    FILE *f = fopen(caminho, "w");
    if (!f) {
        perror("Erro ao criar o arquivo de saida");
        return 0;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (j > 0) fputc(' ', f);
            fprintf(f, "%d", m[i * n + j]);
        }
        fputc('\n', f);
    }

    fclose(f);
    return 1;
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
    const char *arquivo = (argc > 1) ? argv[1] : "imagemMatriz1024.txt"; /*Escolha o nome da Matriz*/
 
    int n;
    int *m = le_matriz(arquivo, &n);
    if (!m) return 1;

    /* imprime_matriz(m, n); */
 
    printf("Ordem da matriz: %d x %d\n", n, n);
    printf("m[0][0]         = %d\n", m[0]);
    printf("m[n-1][n-1]     = %d\n", m[(n - 1) * n + (n - 1)]);

    if (n < 3) { fprintf(stderr, "Matriz pequena demais\n"); free(m); return 1; }

    int *saida = malloc((size_t)n * n * sizeof(int));
    if (!saida) { free(m); return 1; }
    memcpy(saida, m, (size_t)n * n * sizeof(int));   /* bordas inalteradas */

    pthread_t threads[NUM_THREADS];
    Tarefa tarefas[NUM_THREADS];

    int linhas = n - 2;                 /* linhas internas: 1 .. n-2 */
    int base   = linhas / NUM_THREADS;
    int resto  = linhas % NUM_THREADS;
    int atual  = 1;

    struct timespec inicio, fim;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (int i = 0; i < NUM_THREADS; i++) {
        int qtd = base + (i < resto ? 1 : 0);
        tarefas[i] = (Tarefa){ i, atual, atual + qtd, n, m, saida };
        atual += qtd;
        pthread_create(&threads[i], NULL, aplicaFiltro, &tarefas[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempo = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("Tempo de execucao: %.6f segundos\n", tempo);

    if (!escreve_matriz("saida.txt", saida, n)) /*Muda o nome da saída*/
        fprintf(stderr, "Falha ao gravar saida.txt\n");

    free(m);
    free(saida);
    return 0;
}