#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

#define NUM_THREADS 2
#define TOTAL_INCREASES 100000000

typedef struct {
    int id;
    long long iteracoes; /*quantidade de icrementos*/
    long long int *acumulador; /*Variável para salvar o que foi feito*/
    pthread_mutex_t *mutex;
} Tarefa;


/* 
    incrementa um contador de 0 a 100 milhões.
    todas as threads incrementam a mesma variável contador.
    Cada incremento é protegido por um mutex para evitar atualizações perdidas.
*/
void *contador(void *arg) {
    Tarefa *t = (Tarefa *)arg;

    for(long long int i = 0; i < t->iteracoes; i++){
        pthread_mutex_lock(t->mutex);
        (*t->acumulador)++;
        pthread_mutex_unlock(t->mutex);
    }
    
    return NULL;
}


int main(int argc, char *argv[])
{
    long long acumulador = 0;
    pthread_mutex_t mutex; 

    pthread_mutex_init(&mutex, NULL);
    pthread_t threads[NUM_THREADS];
    Tarefa tarefas[NUM_THREADS];

    struct timespec inicio, fim;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    int iteracoes_totais = TOTAL_INCREASES / NUM_THREADS;
    int iteracoes_resto = TOTAL_INCREASES % NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; i++) {
        int qtd = iteracoes_totais + (i < iteracoes_resto ? 1 : 0);
        tarefas[i] = (Tarefa){i, qtd, &acumulador, &mutex};
        pthread_create(&threads[i], NULL, contador, &tarefas[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mutex); 

    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempo = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("\nvalor calculado: %lld\n", acumulador);
    printf("Tempo de execucao: %.6f segundos\n", tempo);

    return 0;
}
