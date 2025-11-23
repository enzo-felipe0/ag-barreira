#ifndef BARRIER_H
#define BARRIER_H

#include <pthread.h>

// Estrutura que representa nosso Monitor de Barreira
typedef struct {
    pthread_mutex_t mutex;      // Garante acesso exclusivo ao contador
    pthread_cond_t cond;        // Onde as threads "dormem" esperando a barreira abrir
    int count;                  // Quantas threads já chegaram na barreira
    int total_threads;          // Quantas threads são esperadas no total
    int generation_count;       // Contador para controlar a "fase" da barreira (evita wakeups espúrios)
} Barrier;

// Estrutura para simular características de Tempo Real
typedef struct {
    int id;
    long execution_cost_ms;     // (C) Tempo estimado de computação
    long deadline_ms;           // (D) Tempo limite para terminar
} TaskInfo;

#endif
