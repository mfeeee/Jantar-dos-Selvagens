# Selvagens Comedores

Este projeto implementa o problema dos **Selvagens Comedores** usando **threads, mutexes e semáforos** em C. O problema envolve um grupo de selvagens que compartilham um único pote de comida e um cozinheiro responsável por reabastecê-lo quando necessário.

## 📌 Como funciona?

- Existem **3 selvagens** que precisam se alimentar.
- O pote inicia com **15 porções** de comida.
- Cada selvagem tenta pegar uma porção do pote:
  - Se há comida disponível, ele se serve.
  - Se o pote está vazio, ele sinaliza o cozinheiro para reabastecê-lo.
- O **cozinheiro** aguarda até ser chamado para reabastecer o pote.
- Cada selvagem faz **11 refeições**, e o cozinheiro tem comida suficiente para reabastecer o pote **duas vezes**.

## 📌 Tecnologias utilizadas

- **Linguagem:** C
- **Bibliotecas:**
  - `pthread.h`: Manipulação de threads.
  - `semaphore.h`: Controle de semáforos.
  - `stdio.h`: Entrada e saída padrão.
  - `unistd.h`: Função `sleep()` para simulação do tempo de refeição.

## 📌 Estrutura do Código

### 🔹 Variáveis Globais
```c
sem_t panelaVazia;
sem_t panelaCheia;
static pthread_mutex_t mutex_porcoes;
static pthread_mutex_t mutex_impressao;
static int porcoes = 15;
```
- `panelaVazia`: Semáforo que indica quando o pote está vazio.
- `panelaCheia`: Semáforo que controla quando o pote tem comida.
- `mutex_porcoes`: Mutex para controle seguro do acesso ao número de porções.
- `mutex_impressao`: Mutex para evitar concorrência na exibição de mensagens.
- `porcoes`: Número atual de porções no pote.

### 🔹 Função `getServingsFromPot()`
```c
int pegarPorcaoDaPanela(void) {
    int retorno;
    if (porcoes <= 0) {
        sem_post (&panelaVazia);
        retorno = 0;
    } else {
        porcoes--;
        retorno = 1;
    }
    pthread_mutex_unlock (&mutex_porcoes);
    return retorno;
}
```
- **Tenta pegar uma porção do pote**.
- Se o pote estiver vazio, **sinaliza o cozinheiro** (`sem_post (&panelaVazia)`).
- **Retorna** 1 se conseguiu pegar comida, 0 se o pote estava vazio.

### 🔹 Função `putServingsInPot()`
```c
void colocarPorcoesNaPanela (int num) {
    porcoes += num;
    sem_post (&panelaCheia);
}
```
- **Reabastece o pote com comida**.
- Sinaliza (`sem_post (&panelaCheia)`) que o pote está cheio.

### 🔹 Thread do Cozinheiro (`cook`)
```c
void *cozinheiro (void *id) {
    int id_cozinheiro = *(int *)id;
    int refeicoes = 2;
    int i;
    while (refeicoes) {
        sem_wait (&panelaVazia); 
        colocarPorcoesNaPanela(15);
        refeicoes--;
        pthread_mutex_lock (&mutex_impressao);
        printf("\nCozinheiro encheu a panela\n\n");
        pthread_mutex_unlock (&mutex_impressao);
        for (i = 0; i < NUM_SELVEGENS; i++)
            sem_post (&panelaCheia);
    }
    return NULL;
}
```
- O cozinheiro **espera ser chamado** (`sem_wait (&panelaVazia)`).
- **Reabastece o pote** com 15 porções.
- **Libera os selvagens** (`sem_post (&panelaCheia)`).

### 🔹 Thread dos Selvagens (`savage`)
```c
void *selvagem (void *id) {
    int id_selvagem = *(int *)id;
    int minhaPorcao;
    int refeicoes = 11;
    while (refeicoes) {
        pthread_mutex_lock (&mutex_porcoes);
        minhaPorcao = pegarPorcaoDaPanela();
        if (porcoes == 0) {
            sem_wait (&panelaCheia);
            minhaPorcao = pegarPorcaoDaPanela();
        }
        pthread_mutex_unlock (&mutex_porcoes);
        refeicoes--;
        pthread_mutex_lock (&mutex_impressao);
        printf("Selvagem: %i esta comendo\n", id_selvagem);
        pthread_mutex_unlock (&mutex_impressao);
        sleep(2);
        pthread_mutex_lock (&mutex_impressao);
        printf("Selvagem: %i terminou de comer\n", id_selvagem);
        pthread_mutex_unlock (&mutex_impressao);
    }
    return NULL;
}
```
- **Pega comida do pote** de forma segura.
- **Se o pote estiver vazio, espera o cozinheiro**.
- Simula o tempo de refeição com `sleep(2)`.

### 🔹 Função `main()`
```c
int main() {
    int i, id[NUM_SELVEGENS+1];
    pthread_t tid[NUM_SELVEGENS+1];
    pthread_mutex_init(&mutex_porcoes, NULL);
    pthread_mutex_init(&mutex_impressao, NULL);
    sem_init(&panelaVazia, 0, 0);
    sem_init(&panelaCheia, 0, 0);

    for (i = 0; i < NUM_SELVEGENS; i++) {
        id[i] = i;
        pthread_create (&tid[i], NULL, selvagem, (void *)&id[i]);
    }
    pthread_create (&tid[i], NULL, cozinheiro, (void *)&id[i]);

    for (i = 0; i < NUM_SELVEGENS; i++)
        pthread_join(tid[i], NULL);
}
```
- **Inicializa mutexes e semáforos**.
- **Cria threads para os selvagens e o cozinheiro**.
- **Aguarda todas as threads terminarem** (`pthread_join`).

## 📌 Como compilar e executar?

Para compilar:
```sh
gcc selvagens_comedores.c -o selvagens_comedores -lpthread
```

Para executar:
```sh
./selvagens_comedores
```

## 📌 Saída esperada
O programa imprime mensagens indicando quando os selvagens estão comendo e quando o cozinheiro reabastece o pote:
```
Selvagem 0 está comendo
Selvagem 1 está comendo
Selvagem 2 está comendo
...
Cozinheiro reabasteceu o pote
...
Selvagem 0 terminou de comer
Selvagem 1 terminou de comer
Selvagem 2 terminou de comer
```

## 📌 Conclusão
Este código ilustra o problema clássico da **sincronização de processos** usando **threads, mutexes e semáforos**. Ele mostra como coordenar múltiplas threads acessando um recurso compartilhado de maneira segura.

---
**Desenvolvido para fins acadêmicos!**

