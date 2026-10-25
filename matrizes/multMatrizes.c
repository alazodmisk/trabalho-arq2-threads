#include <stdio.h>
#include <time.h>
///#include <pthread.h>

#define TOTAL 12
#define NUM_THREADS 1

typedef struct {
    int id;
    int inicio;
    int fim;
} Tarefa;

void elemento(FILE *arquivo) {

    char num[5];
    while (fgets(num, sizeof(num), arquivo) != NULL) {
        printf("%s", num);
    }
}














void *multiplica_linha(void *arg) {
    Tarefa *tarefa = (Tarefa *)arg;

    for (int i = tarefa->inicio; i < tarefa->fim; i++) {
        printf("Thread %d processa a linha %d\n", tarefa->id, i);
    }
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    Tarefa tarefas[NUM_THREADS];

    int tamanho = TOTAL / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        tarefas[i].id = i;
        tarefas[i].inicio = i * tamanho;
        tarefas[i].fim = (i + 1) * tamanho;

        pthread_create(
            &threads[i], NULL, multiplica_linha, &tarefas[i]
        );
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}


