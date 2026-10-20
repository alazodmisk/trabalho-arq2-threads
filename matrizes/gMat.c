#include <stdio.h>
#include <stdlib.h>
void gerar_matriz(int n, const char *nome) {
FILE *arquivo = fopen(nome, "w");
if (arquivo == NULL) {
printf("Erro ao criar arquivo.\n");
return;
}
for (int i = 0; i < n; i++) {
for (int j = 0; j < n; j++) {
fprintf(arquivo, "%d ", rand() % 100);
}
fprintf(arquivo, "\n");
}
fclose(arquivo);
}
int main() {
int tamanhos[] = {256, 512, 1024};
srand(12345);
for (int i = 0; i < 3; i++) {
int n = tamanhos[i];
char nomeA[50], nomeB[50];
sprintf(nomeA, "A_%d.txt", n);
sprintf(nomeB, "B_%d.txt", n);
gerar_matriz(n, nomeA);
gerar_matriz(n, nomeB);
printf("Matrizes %d x %d geradas.\n", n, n);
}
return 0;
}