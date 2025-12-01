#define _XOPEN_SOURCE 500 // usado para usleep
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "barrier.h"

#define NUM_THREADS 4      // numero de islands ou processadores
#define NUM_GENERATIONS 5  // quantas geracoes vamos simular

// instancia global da barreira
Barrier my_barrier;

// --- funcoes aux de tempo ---
long get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

// --- implementacao de uma Barreira (logica usada pro monitor) ---
void init_barrier(int n) {
    pthread_mutex_init(&my_barrier.mutex, NULL);
    pthread_cond_init(&my_barrier.cond, NULL);
    my_barrier.count = 0;
    my_barrier.total_threads = n;
    my_barrier.generation_count = 0;
}

void enter_barrier(int thread_id) {
    // 1. LOCK (Entrada na regiao critica)
    pthread_mutex_lock(&my_barrier.mutex);
    
    my_barrier.count++;
    int current_gen = my_barrier.generation_count;

    if (my_barrier.count == my_barrier.total_threads) {
        //ULTIMA thread a chegar
        //tarefa: zerar o contador e acordar todo mundo
        my_barrier.count = 0;
        my_barrier.generation_count++; // passa fase da barreira
        
        printf("[SISTEMA] Barreira completada! Todas %d threads chegaram. Liberando...\n", my_barrier.total_threads);
        
        // BROADCAST: acorda todas as threads dormindo na variavel condicional
        pthread_cond_broadcast(&my_barrier.cond);
    } else {
        // n é a última. -> deve dormir
        // loop while para evitar "spurious wakeups" (acordar sem querer)
        while (current_gen == my_barrier.generation_count) {
            pthread_cond_wait(&my_barrier.cond, &my_barrier.mutex);
        }
    }

    // 2. UNLOCK (saida da regiao critica)
    pthread_mutex_unlock(&my_barrier.mutex);
}

// --- a tarefa da thread (worker) ---
void* genetic_algorithm_worker(void* arg) {
    TaskInfo* task = (TaskInfo*)arg;
    
    for (int gen = 1; gen <= NUM_GENERATIONS; gen++) {
        long start_time = get_time_ms();

        printf("Thread %d: Iniciando Geracao %d (Calculando Fitness...)\n", task->id, gen);

        // simulação de custo computacional (C)
        usleep(task->execution_cost_ms * 1000); 

        long end_time = get_time_ms();
        long duration = end_time - start_time;

        // verificação de deadline (soft real time)
        if (duration > task->deadline_ms) {
            printf("Thread %d: [ALERTA] Deadline perdido na Geracao %d!\n", task->id, gen);
        }

        printf("Thread %d: Terminou Geracao %d em %ldms. Esperando na barreira...\n", task->id, gen, duration);

        // >>> PONTO DE SINCRONIZACAO <<<
        // thread só passa se todas já tiverem chegado
        enter_barrier(task->id);

        // apos a barreira, deve ocorrer a troca de dados (crossover)
        printf("Thread %d: Passou da barreira. Preparando proxima geracao.\n", task->id);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    TaskInfo task_info[NUM_THREADS];

    printf("--- Inicio da Simulacao de AG Paralelo com Barreira ---\n");

    // inicializa a barreira para esperar NUM_THREADS
    init_barrier(NUM_THREADS);

    //criacao das threads
    for (int i = 0; i < NUM_THREADS; i++) {
        task_info[i].id = i;
        //define caracteristicas de tempo real:
        //varia o custo para ver a barreira funcionando (umas rapidas, outras lentas)
        task_info[i].execution_cost_ms = 200 + (i * 150); // thread 0: 200ms, Thread 3: 650ms
        task_info[i].deadline_ms = 1000; // deadline de 1 segundo por geracao
        
        if (pthread_create(&threads[i], NULL, genetic_algorithm_worker, (void*)&task_info[i])) {
            perror("Erro ao criar thread");
            exit(1);
        }
    }

    // Join (Espera todas terminarem todas as geracoes)
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // limpa
    pthread_mutex_destroy(&my_barrier.mutex);
    pthread_cond_destroy(&my_barrier.cond);

    printf("--- Simulacao Finalizada com Sucesso ---\n");
    return 0;
}
