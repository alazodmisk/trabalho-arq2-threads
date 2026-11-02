# Avaliação de Desempenho e Programação Paralela com Threads

Trabalho da disciplina de Arquitetura de Computadores II, Departamento de Informática da Universidade Estadual de Maringá (UEM).

**Autores:** Guilherme Henrique Viana Pichitelli Vitor e Pedro Henrique Bertoni de Souza

O projeto avalia o desempenho da programação paralela com a biblioteca **POSIX Threads (Pthreads)** em C. Foram implementados quatro algoritmos, executados com **1, 2, 4 e 8 threads**:

1. Filtro de média 3x3 em imagens em escala de cinza
2. Multiplicação de matrizes quadradas
3. Contador com mutex
4. Contador sem mutex (com contadores locais por thread)

Para cada configuração, o tempo é a média de **5 execuções**. Também são calculados o desvio padrão amostral, o speedup e a eficiência paralela:

```
Speedup = Tempo sequencial / Tempo paralelo
EP      = Speedup / Quantidade de threads
```

---

## Estrutura do repositório

| Pasta | Conteúdo |
|---|---|
| `imgMat/` | Filtro de média 3x3 (`filterSingle.c` e variantes), imagens, matrizes em `.txt` e scripts Python de conversão (`imgToMat.py` e `matToImg.py`) |
| `matrizes/` | Multiplicação de matrizes com threads |
| `counter/` | Contadores com e sem mutex |

---

## Ambiente de testes

| Item | Especificação |
|---|---|
| Processador | Intel Core i5-12500H (12 núcleos físicos, 16 threads lógicas, arquitetura híbrida) |
| Memória | 24 GB DDR4, 3200 MHz |
| Sistema | Windows 11 com WSL2 (Linux) |
| Compilador | GCC, com `-lm -pthread` |

Durante as medições, as únicas aplicações ativas eram o WSL2, o VSCode e uma aba do Microsoft Edge com a planilha de registro dos dados.

O tempo é medido com `clock_gettime(CLOCK_MONOTONIC)`, imediatamente antes da criação das threads e logo após o último `pthread_join`. Leitura e escrita de arquivos **não** entram na medição.

---

## Como executar

### Compilação (qualquer programa em C)

```bash
gcc arquivo.c -o arquivo -lm -pthread
./arquivo
```

Para o filtro de imagem, por exemplo:

```bash
gcc filterSingle.c -o filterSingle -lm -pthread
./filterSingle
```

O número de threads (`NUM_THREADS`) e os nomes dos arquivos de entrada e saída são definidos no código-fonte.

### Ferramentas em Python

Na raiz do projeto, ative o ambiente virtual:

```bash
source .venv/bin/activate
```

1. **`imgToMat.py`** converte uma imagem `.png` quadrada em escala de cinza para uma matriz `.txt` (usa Pillow e NumPy). Coloque a imagem no mesmo diretório do script e informe o nome do arquivo no código.
2. **Programa em C** lê a matriz, aplica o filtro e grava o resultado em `saida.txt`.
3. **`matToImg.py`** converte a matriz filtrada de volta para `.png`.

---

## Descrição dos algoritmos

### 1. Filtro de média 3x3

- A imagem é uma matriz quadrada de ordem `n`, guardada em um vetor 1D (o pixel `(i, j)` fica em `vetor[i * n + j]`).
- A função `le_matriz` lê o arquivo em duas passadas: conta os valores, obtém a ordem pela raiz quadrada e então preenche o vetor. Assim, qualquer ordem (256, 512, 1024...) funciona sem alterar o código.
- Cada pixel interno recebe a média inteira (truncada) dos 9 valores da vizinhança 3x3. As bordas permanecem inalteradas.
- As `n - 2` linhas internas são divididas em faixas contíguas. As linhas que sobram são distribuídas uma a uma entre as primeiras threads, de modo que a diferença de carga entre duas threads é de no máximo uma linha.
- Cada thread recebe uma struct `Tarefa` com seu intervalo de linhas `[inicio, fim)`, a ordem `n` e os ponteiros de entrada e saída.

### 2. Multiplicação de matrizes

Aceita apenas matrizes quadradas de mesma ordem. Os testes usaram ordens 256, 512 e 1024.

### 3. Contador com mutex

Todas as threads incrementam a **mesma variável** `acumulador` até 100.000.000. Como `(*acumulador)++` na CPU é uma sequência de leitura, soma e escrita, duas threads podem se intercalar e perder incrementos (condição de corrida). O `pthread_mutex_lock` e o `pthread_mutex_unlock` em volta do incremento garantem exclusão mútua, e o resultado final é sempre exatamente 100.000.000. O custo é que o lock é disputado a cada incremento.

### 4. Contador sem mutex

Cada thread acumula em uma variável local, sem disputa, e o total é somado ao final. Isso elimina a sincronização no laço principal.

---

## Resultados

Todos os tempos estão em segundos (média de 5 execuções). DP é o desvio padrão amostral.

### Filtro de média 3x3

| Experimento | Média (s) | DP (s) | Speedup | EP |
|---|---|---|---|---|
| img 256 / 1 thread | 0,000964 | 0,000023 | N/A | N/A |
| img 256 / 2 threads | 0,000631 | 0,000034 | 1,53 | 0,765 |
| img 256 / 4 threads | 0,000529 | 0,000093 | 1,82 | 0,455 |
| img 256 / 8 threads | 0,000525 | 0,000069 | 1,84 | 0,230 |
| img 512 / 1 thread | 0,003659 | 0,000104 | N/A | N/A |
| img 512 / 2 threads | 0,002021 | 0,000072 | 1,81 | 0,905 |
| img 512 / 4 threads | 0,001571 | 0,000280 | 2,32 | 0,580 |
| img 512 / 8 threads | 0,001258 | 0,000187 | 2,90 | 0,362 |
| img 1024 / 1 thread | 0,013323 | 0,000583 | N/A | N/A |
| img 1024 / 2 threads | 0,006741 | 0,000446 | 1,98 | 0,990 |
| img 1024 / 4 threads | 0,003866 | 0,000502 | 3,45 | 0,862 |
| img 1024 / 8 threads | 0,002976 | 0,000256 | 4,47 | 0,559 |

### Multiplicação de matrizes

| Experimento | Média (s) | DP (s) | Speedup | EP |
|---|---|---|---|---|
| matriz 256 / 1 thread | 0,028588 | 0,001741 | N/A | N/A |
| matriz 256 / 2 threads | 0,015143 | 0,000573 | 1,89 | 0,944 |
| matriz 256 / 4 threads | 0,008714 | 0,001355 | 3,28 | 0,820 |
| matriz 256 / 8 threads | 0,006634 | 0,001148 | 4,31 | 0,539 |
| matriz 512 / 1 thread | 0,284795 | 0,007226 | N/A | N/A |
| matriz 512 / 2 threads | 0,141025 | 0,011738 | 2,02 | 1,010 |
| matriz 512 / 4 threads | 0,076613 | 0,002266 | 3,72 | 0,929 |
| matriz 512 / 8 threads | 0,051789 | 0,004643 | 5,50 | 0,687 |
| matriz 1024 / 1 thread | 2,926945 | 0,048958 | N/A | N/A |
| matriz 1024 / 2 threads | 1,481238 | 0,029534 | 1,98 | 0,988 |
| matriz 1024 / 4 threads | 0,805438 | 0,031259 | 3,63 | 0,908 |
| matriz 1024 / 8 threads | 0,586256 | 0,017237 | 4,99 | 0,624 |

### Contador com mutex

| Experimento | Média (s) | DP (s) | Speedup | EP |
|---|---|---|---|---|
| contadorMutex 1t | 1,251113 | 0,021407 | N/A | N/A |
| contadorMutex 2t | 4,823704 | 0,177409 | 0,26 | 0,130 |
| contadorMutex 4t | 4,254707 | 0,117072 | 0,29 | 0,072 |
| contadorMutex 8t | 6,670753 | 0,096369 | 0,19 | 0,024 |

### Contador sem mutex

| Experimento | Média (s) | DP (s) | Speedup | EP |
|---|---|---|---|---|
| contador 1 thread | 0,047759 | 0,000537 | N/A | N/A |
| contador 2 threads | 0,025945 | 0,001121 | 1,84 | 0,920 |
| contador 4 threads | 0,015183 | 0,001886 | 3,15 | 0,787 |
| contador 8 threads | 0,010182 | 0,000623 | 4,69 | 0,586 |

---

## Principais observações

- **Mais threads reduzem o tempo, mas com retorno decrescente.** A eficiência paralela cai à medida que threads são adicionadas, em todos os algoritmos que escalam.
- **O tamanho do problema importa.** Em matrizes maiores, o custo fixo de criação e gerenciamento das threads é diluído, e a escalabilidade com 4 e 8 threads melhora. No filtro de 256x256, o tempo total é tão pequeno (menos de 1 ms) que 4 e 8 threads praticamente empatam.
- **Speedup superlinear em 512 / 2 threads (EP = 1,010)** está dentro do ruído de medição, dado o desvio padrão da configuração.
- **O mutex a cada incremento é um gargalo.** Com 2 threads, o contador com mutex fica cerca de 4 vezes mais lento que a versão sequencial, porque o custo de sincronização supera o trabalho útil (um incremento). Com 4 threads o tempo foi ligeiramente menor que com 2. Os autores atribuem isso ao comportamento do escalonador e à disputa pelo lock, mas a causa exata não foi isolada.
- **Sem mutex, o contador escala bem** (speedup de 4,69 com 8 threads), pois não há disputa pela variável compartilhada.

---

## Relatório

O relatório completo, com a explicação detalhada de cada código, está no formato PDF do repositório (`relatorio_threads.pdf`)