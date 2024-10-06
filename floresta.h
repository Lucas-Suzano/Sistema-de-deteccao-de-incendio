#ifndef FLORESTA_H
#define FLORESTA_H

#include <pthread.h>

#define TAMANHO 30

typedef struct {
    int x, y;               
    char estado;            // ('-', 'T', '@', '/')
    pthread_mutex_t lock;   // Mutex para garantir exclusão mútua
    pthread_cond_t cond;    // Variável de condição para comunicação
} NoSensor;

extern NoSensor floresta[TAMANHO][TAMANHO];

void inicializar_floresta();
void exibir_floresta();
void *sensor_func(void *arg);         
void *gerar_incendios(void *arg);     
void *central_func(void *arg);        

#endif
