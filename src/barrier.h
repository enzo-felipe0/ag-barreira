#ifndef BARRIER_H
#define BARRIER_H

#include <pthread.h>

// estrutura que representa o monitor de barreira
typedef struct {
    pthread_mutex_t mutex;      // garante acesso exclusivo ao contador
    pthread_cond_t cond;        // "lugar" onde as threads "dormem" esperando a barreira abrir
    int count;                  // quantas threads ja chegaram na barreira
    int total_threads;          // quantas threads sao esperadas
    int generation_count;       // contador para controlar a "fase" da barreira 
} Barrier;

// estrutura para simular caracteristicas de tempo real
typedef struct {
    int id;
    long execution_cost_ms;     // (C) tempo estimado de computacao
    long deadline_ms;           // (D) tempo limite para terminar
} TaskInfo;

#endif
