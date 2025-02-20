#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// Definição do número de selvagens
#define NUM_SELVEGENS 10

// Declaração dos semáforos
sem_t panelaVazia;
sem_t panelaCheia;

// Funções das threads
void *selvagem (void*);
void *cozinheiro (void*);

// Mutexes para controle de concorrência
static pthread_mutex_t mutex_porcoes;
static pthread_mutex_t mutex_impressao;

// Variável compartilhada representando o número de porções disponíveis
static int porcoes = 15;

// Função para os selvagens pegarem uma porção da panela
int pegarPorcaoDaPanela(void) {
    int retorno;

    if (porcoes <= 0) {
        sem_post (&panelaVazia); // Informa que a panela está vazia
        retorno = 0;
    } else {
        porcoes--; // Diminui uma porção
        retorno = 1;
    }

    pthread_mutex_unlock (&mutex_porcoes);

    return retorno;
}

// Função para o cozinheiro encher a panela
void colocarPorcoesNaPanela (int num) {
    porcoes += num;
    sem_post (&panelaCheia); // Indica que a panela está cheia
}

// Função da thread do cozinheiro
void *cozinheiro (void *id) {
    int id_cozinheiro = *(int *)id;
    int refeicoes = 2;
    int i;

    while (refeicoes) {
        sem_wait (&panelaVazia); // Espera a panela esvaziar
        colocarPorcoesNaPanela(15); // Reabastece a panela
        refeicoes--;

        pthread_mutex_lock (&mutex_impressao);
        printf("\nCozinheiro encheu a panela\n\n");
        pthread_mutex_unlock (&mutex_impressao);

        for (i = 0; i < NUM_SELVEGENS; i++)
            sem_post (&panelaCheia); // Libera os selvagens para comerem
    }

    return NULL;
}

// Função da thread dos selvagens
void *selvagem (void *id) {
    int id_selvagem = *(int *)id;
    int minhaPorcao;
    int refeicoes = 11;

    while (refeicoes) {
        pthread_mutex_lock (&mutex_porcoes);
        minhaPorcao = pegarPorcaoDaPanela();
        if (porcoes == 0) {
            sem_wait (&panelaCheia); // Aguarda a panela ser reabastecida
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

// Função principal do programa
int main() {
    int i, id[NUM_SELVEGENS+1];
    pthread_t tid[NUM_SELVEGENS+1];

    // Inicializa os mutexes
    pthread_mutex_init(&mutex_porcoes, NULL);
    pthread_mutex_init(&mutex_impressao, NULL);

    // Inicializa os semáforos
    sem_init(&panelaVazia, 0, 0);
    sem_init(&panelaCheia, 0, 0);

    // Criação das threads dos selvagens
    for (i = 0; i < NUM_SELVEGENS; i++) {
        id[i] = i;
        pthread_create (&tid[i], NULL, selvagem, (void *)&id[i]);
    }
    
    // Criação da thread do cozinheiro
    pthread_create (&tid[i], NULL, cozinheiro, (void *)&id[i]);

    // Espera as threads dos selvagens terminarem
    for (i = 0; i < NUM_SELVEGENS; i++)
        pthread_join(tid[i], NULL);
}
