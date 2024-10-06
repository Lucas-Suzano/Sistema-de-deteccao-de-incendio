#include "floresta.h"
#include <stdio.h>
#include <stdlib.h>    
#include <time.h>      
#include <pthread.h>
#include <unistd.h>

NoSensor floresta[TAMANHO][TAMANHO];
pthread_t sensores[TAMANHO][TAMANHO];
pthread_t thread_incendios;
pthread_t thread_central;
pthread_t thread_exibicao;  

// Função para inicializar a floresta
void inicializar_floresta() {
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            floresta[i][j].x = i;
            floresta[i][j].y = j;
            floresta[i][j].estado = '-';  // Inicialmente sem sensores
            pthread_mutex_init(&floresta[i][j].lock, NULL);
            pthread_cond_init(&floresta[i][j].cond, NULL);
        }
    }
}

// Função para exibir a matriz da floresta 
void exibir_floresta() {
    system("clear");  // Limpa o terminal
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            printf("%c ", floresta[i][j].estado);
        }
        printf("\n");
    }
}

// Função da thread de exibição 
void *thread_exibicao_func(void *arg) {
    while (1) {
        pthread_mutex_lock(&floresta[0][0].lock);  // Sincroniza acesso à matriz
        exibir_floresta();
        pthread_mutex_unlock(&floresta[0][0].lock);  // Libera o mutex
        sleep(1);  
    }
    return NULL;
}

// Função principal
int main() {
    srand(time(NULL));  // Inicializa a semente de números aleatórios

    inicializar_floresta();

    // Cria os sensores
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            floresta[i][j].estado = 'T';  
            pthread_create(&sensores[i][j], NULL, sensor_func, &floresta[i][j]);
        }
    }

    // Cria a thread para gerar incêndios
    pthread_create(&thread_incendios, NULL, gerar_incendios, NULL);

    // Cria a thread da central de controle
    pthread_create(&thread_central, NULL, central_func, NULL);

    // Cria a thread de exibição
    pthread_create(&thread_exibicao, NULL, thread_exibicao_func, NULL);

    // Aguarda as threads (nunca irão terminar)
    pthread_join(thread_incendios, NULL);
    pthread_join(thread_central, NULL);
    pthread_join(thread_exibicao, NULL);
    
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            pthread_join(sensores[i][j], NULL);
        }
    }

    return 0;
}
