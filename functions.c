
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "floresta.h"

extern NoSensor floresta[TAMANHO][TAMANHO];

void *central_func(void *arg) {
    while (1) {
        for (int i = 0; i < TAMANHO; i++) {
            pthread_mutex_lock(&floresta[0][i].lock);
            pthread_cond_wait(&floresta[0][i].cond, &floresta[0][i].lock);
            printf("Central: Incêndio detectado na borda (0, %d)\n", i);
            pthread_mutex_unlock(&floresta[0][i].lock);

            pthread_mutex_lock(&floresta[TAMANHO-1][i].lock);
            pthread_cond_wait(&floresta[TAMANHO-1][i].cond, &floresta[TAMANHO-1][i].lock);
            printf("Central: Incêndio detectado na borda (%d, %d)\n", TAMANHO-1, i);
            pthread_mutex_unlock(&floresta[TAMANHO-1][i].lock);
        }
        for (int i = 0; i < TAMANHO; i++) {
            pthread_mutex_lock(&floresta[i][0].lock);
            pthread_cond_wait(&floresta[i][0].cond, &floresta[i][0].lock);
            printf("Central: Incêndio detectado na borda (%d, 0)\n", i);
            pthread_mutex_unlock(&floresta[i][0].lock);

            pthread_mutex_lock(&floresta[i][TAMANHO-1].lock);
            pthread_cond_wait(&floresta[i][TAMANHO-1].cond, &floresta[i][TAMANHO-1].lock);
            printf("Central: Incêndio detectado na borda (%d, %d)\n", i, TAMANHO-1);
            pthread_mutex_unlock(&floresta[i][TAMANHO-1].lock);
        }
    }
    return NULL;
}

// Função para gerar incêndios aleatórios
void *gerar_incendios(void *arg) {
    while (1) {
        sleep(3);  // Gera incêndios a cada 3 segundos
        int x = rand() % TAMANHO;
        int y = rand() % TAMANHO;

        pthread_mutex_lock(&floresta[x][y].lock);  // Bloqueia a célula escolhida
        if (floresta[x][y].estado == 'T') {  // Verifica se a célula tem um sensor ativo
            floresta[x][y].estado = '@';  // Incêndio ativo
            printf("Incêndio iniciado em (%d, %d)\n", x, y);  // Log do incêndio
        }
        pthread_mutex_unlock(&floresta[x][y].lock);  
    }
    return NULL;
}

void *sensor_func(void *arg) {
    NoSensor *sensor = (NoSensor *)arg;
    int x = sensor->x;
    int y = sensor->y;

    while (1) {
        pthread_mutex_lock(&sensor->lock);  

        // Se houver fogo na célula, a célula queima
        if (sensor->estado == '@') {
            printf("Sensor em (%d, %d) detectou fogo!\n", x, y);
            sensor->estado = '/';  

            // Propaga o fogo para os vizinhos
            if (x > 0 && floresta[x-1][y].estado == 'T') {
                pthread_mutex_lock(&floresta[x-1][y].lock);
                floresta[x-1][y].estado = '@';  
                pthread_mutex_unlock(&floresta[x-1][y].lock);
            }
            if (x < TAMANHO - 1 && floresta[x+1][y].estado == 'T') {
                pthread_mutex_lock(&floresta[x+1][y].lock);
                floresta[x+1][y].estado = '@';  
                pthread_mutex_unlock(&floresta[x+1][y].lock);
            }
            if (y > 0 && floresta[x][y-1].estado == 'T') {
                pthread_mutex_lock(&floresta[x][y-1].lock);
                floresta[x][y-1].estado = '@';  
                pthread_mutex_unlock(&floresta[x][y-1].lock);
            }
            if (y < TAMANHO - 1 && floresta[x][y+1].estado == 'T') {
                pthread_mutex_lock(&floresta[x][y+1].lock);
                floresta[x][y+1].estado = '@';  
                pthread_mutex_unlock(&floresta[x][y+1].lock);
            }
        }

        pthread_mutex_unlock(&sensor->lock); 
        sleep(1);  
    }
    return NULL;
}
