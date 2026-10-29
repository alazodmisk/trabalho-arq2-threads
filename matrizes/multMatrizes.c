
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

typedef struct {
    int inicio;
    int fim;
    int n;
    const int *A;
    const int *B;
    long long *C;
} Tarefa;

/* Cada thread calcula um conjunto de linhas da matriz C. */
void *multiplicaMatriz(void *arg)
{
    Tarefa *t = (Tarefa *)arg;
    int n = t->n;

    for (int i = t->inicio; i < t->fim; i++) {
        for (int j = 0; j < n; j++) {
            long long soma = 0;

            for (int k = 0; k < n; k++) {
                soma += (long long)t->A[i * n + k]
                      * t->B[k * n + j];
            }

            t->C[i * n + j] = soma;
        }
    }

    return NULL;
}

/* Le uma matriz quadrada e descobre sua ordem automaticamente. */
int *le_matriz(const char *caminho, int *n_out)
{
    FILE *f = fopen(caminho, "r");

    if (!f) {
        perror(caminho);
        return NULL;
    }

    long total = 0;
    int valor;

    while (fscanf(f, "%d", &valor) == 1)
        total++;

    long n = (long)(sqrt((double)total) + 0.5);

    rewind(f);

    int *matriz = malloc((size_t)total * sizeof(int));

    if (!matriz) {
        fprintf(stderr, "Erro de memoria.\n");
        fclose(f);
        return NULL;
    }

    for (long i = 0; i < total; i++)
        fscanf(f, "%d", &matriz[i]);

    fclose(f);

    *n_out = (int)n;
    return matriz;
}

/* Grava a matriz resultante em um arquivo TXT. */
int escreve_matriz(const char *caminho, const long long *m, int n)
{
    FILE *f = fopen(caminho, "w");

    if (!f) {
        perror(caminho);
        return 0;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (j > 0)
                fputc(' ', f);

            fprintf(f, "%lld", m[i * n + j]);
        }

        fputc('\n', f);
    }

    fclose(f);
    return 1;
}

int main(int argc, char *argv[])
{
    if (argc < 4 || argc > 5) {
        fprintf(stderr,
            "Uso: %s matrizA.txt matrizB.txt threads [saida.txt]\n",
            argv[0]);
        return 1;
    }

    const char *arquivoA = argv[1];
    const char *arquivoB = argv[2];
    const char *arquivoSaida =
        (argc == 5) ? argv[4] : "saida.txt";

    char *fimNumero;
    errno = 0;
    long qtdLong = strtol(argv[3], &fimNumero, 10);

    if (errno != 0 || *argv[3] == '\0' || *fimNumero != '\0' ||
        qtdLong > INT_MAX || qtdLong < 1) {
        fprintf(stderr, "Numero de threads invalido.\n");
        return 1;
    }

    int numThreads = (int)qtdLong;

    if (numThreads != 1 && numThreads != 2 &&
        numThreads != 4 && numThreads != 8) {
        fprintf(stderr, "Use 1, 2, 4 ou 8 threads.\n");
        return 1;
    }

    int nA, nB;
    int *A = le_matriz(arquivoA, &nA);
    int *B = le_matriz(arquivoB, &nB);

    if (!A || !B) {
        free(A);
        free(B);
        return 1;
    }

    int n = nA;

    long long *C = malloc((size_t)n * n * sizeof(long long));

    if (!C) {
        fprintf(stderr, "Erro de memoria.\n");
        free(A);
        free(B);
        return 1;
    }

    pthread_t threads[numThreads];
    Tarefa tarefas[numThreads];

    int base = n / numThreads;
    int resto = n % numThreads;
    int atual = 0;

    struct timespec inicio, fim;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (int i = 0; i < numThreads; i++) {
        int qtd = base + (i < resto ? 1 : 0);

        tarefas[i] = (Tarefa) {
            atual, atual + qtd, n, A, B, C
        };

        atual += qtd;

        pthread_create(
            &threads[i], NULL, multiplicaMatriz, &tarefas[i]
        );
    }

    for (int i = 0; i < numThreads; i++)
        pthread_join(threads[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &fim);

    double tempo =
        (fim.tv_sec - inicio.tv_sec) +
        (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    printf("Ordem das matrizes: %d x %d\n", n, n);
    printf("Numero de threads: %d\n", numThreads);
    printf("Tempo de multiplicacao: %.6f segundos\n", tempo);

    if (!escreve_matriz(arquivoSaida, C, n))
        fprintf(stderr, "Falha ao gravar o resultado.\n");
    else
        printf("Resultado salvo em: %s\n", arquivoSaida);

    free(A);
    free(B);
    free(C);

    return 0;
}
