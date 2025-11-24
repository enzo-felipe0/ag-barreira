#define _XOPEN_SOURCE 500 // Necessário para usleep
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "barrier.h"

#define NUM_THREADS 4      // Número de "Ilhas" ou processadores
#define NUM_GENERATIONS 5  // Quantas gerações vamos simular

// Instância global da barreira
Barrier my_barrier;

// --- Funções Auxiliares de Tempo ---
long get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

// --- Implementação da Barreira (Lógica do Monitor) ---
void init_barrier(int n) {
    pthread_mutex_init(&my_barrier.mutex, NULL);
    pthread_cond_init(&my_barrier.cond, NULL);
    my_barrier.count = 0;
    my_barrier.total_threads = n;
    my_barrier.generation_count = 0;
}

void enter_barrier(int thread_id) {
    // 1. LOCK (Entrada na região crítica)
    pthread_mutex_lock(&my_barrier.mutex);
    
    my_barrier.count++;
    int current_gen = my_barrier.generation_count;

    if (my_barrier.count == my_barrier.total_threads) {
        // Eu sou a ÚLTIMA thread a chegar!
        // A tarefa é: Zerar o contador e acordar todo mundo.
        my_barrier.count = 0;
        my_barrier.generation_count++; // Avança fase da barreira
        
        printf("[SISTEMA] Barreira completada! Todas %d threads chegaram. Liberando...\n", my_barrier.total_threads);
        
        // BROADCAST: Acorda todas as threads dormindo na variável condicional
        pthread_cond_broadcast(&my_barrier.cond);
    } else {
        // Não sou a última. Devo dormir.
        // Loop while para evitar "spurious wakeups" (acordar sem querer)
        while (current_gen == my_barrier.generation_count) {
            pthread_cond_wait(&my_barrier.cond, &my_barrier.mutex);
        }
    }

    // 2. UNLOCK (Saída da região crítica)
    pthread_mutex_unlock(&my_barrier.mutex);
}

// --- A Tarefa da Thread (Worker) ---
void* genetic_algorithm_worker(void* arg) {
    TaskInfo* task = (TaskInfo*)arg;
    
    for (int gen = 1; gen <= NUM_GENERATIONS; gen++) {
        long start_time = get_time_ms();

        printf("Thread %d: Iniciando Geracao %d (Calculando Fitness...)\n", task->id, gen);

        // Simulação de Custo Computacional (C)
        // Em um AG real, aqui estaria o loop de avaliação da população
        usleep(task->execution_cost_ms * 1000); 

        long end_time = get_time_ms();
        long duration = end_time - start_time;

        // Verificação de Deadline (Soft Real-Time)
        if (duration > task->deadline_ms) {
            printf("Thread %d: [ALERTA] Deadline perdido na Geracao %d!\n", task->id, gen);
        }

        printf("Thread %d: Terminou Geracao %d em %ldms. Esperando na barreira...\n", task->id, gen, duration);

        // >>> PONTO DE SINCRONIZAÇÃO <<<
        // Nenhuma thread passa daqui até que todas cheguem
        enter_barrier(task->id);

        // Após a barreira, teoricamente ocorreria a troca de dados (migração/crossover)
        printf("Thread %d: Passou da barreira. Preparando proxima geracao.\n", task->id);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    TaskInfo task_info[NUM_THREADS];

    printf("--- Inicio da Simulacao de AG Paralelo com Barreira ---\n");

    // Inicializa a barreira para esperar NUM_THREADS
    init_barrier(NUM_THREADS);

    // Criação das Threads
    for (int i = 0; i < NUM_THREADS; i++) {
        task_info[i].id = i;
        // Definindo características de tempo real:
        // Vamos variar o custo para ver a barreira funcionando (umas rápidas, outras lentas)
        task_info[i].execution_cost_ms = 200 + (i * 150); // Thread 0: 200ms, Thread 3: 650ms
        task_info[i].deadline_ms = 1000; // Deadline de 1 segundo por geração
        
        if (pthread_create(&threads[i], NULL, genetic_algorithm_worker, (void*)&task_info[i])) {
            perror("Erro ao criar thread");
            exit(1);
        }
    }

    // Join (Espera todas terminarem todas as gerações)
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Limpeza
    pthread_mutex_destroy(&my_barrier.mutex);
    pthread_cond_destroy(&my_barrier.cond);

    printf("--- Simulacao Finalizada com Sucesso ---\n");
    return 0;
}
