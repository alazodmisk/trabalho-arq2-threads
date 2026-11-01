#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

#define NUM_THREADS 1

typedef struct {
    int id;
    long long iteracoes;
    long long resultado;
} Tarefa;


/*
    Incrementa um contador de 0 a 100 milhões, porém dividido para cada thread possuir
    seu proprio contador local. Armazena o resultado na sua Tarefa.
*/
void *contador(void *arg) {
    Tarefa *t = (Tarefa *)arg;
    long long contador_local = 0;
  
    for (long long i = 0; i < t->iteracoes; i++) contador_local++;
    
    t->resultado = contador_local;

    return NULL;
}


int main(int argc, char *argv[])
{
    long long acumulador = 0;
  
    pthread_t threads[NUM_THREADS];
    Tarefa tarefas[NUM_THREADS];

    struct timespec inicio, fim;
    clock_gettime(CLOCK_MONOTONIC, &inicio);
  
    int iteracoes_totais = TOTAL_INCREMENTOS / NUM_THREADS;
    int iteracoes_resto = TOTAL_INCREMENTOS % NUM_THREADS;
  
    for (int i = 0; i < NUM_THREADS; i++) {
        int qtd = iteracoes_totais + (i < iteracoes_resto ? 1 : 0);
        tarefas[i] = (Tarefa) { i, qtd, 0 };
        pthread_create(&threads[i], NULL, contador, &tarefas[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) { acumulador += tarefas[i].resultado; }

    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempo = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
    for (int i = 0; i < NUM_THREADS; i++) {
        printf("Thread %d: %lld incrementos\n", tarefas[i].id, tarefas[i].resultado); }

    printf("\nvalor calculado: %lld\n", acumulador);
    printf("Tempo de execucao: %.6f segundos\n", tempo);

    return 0;
}
