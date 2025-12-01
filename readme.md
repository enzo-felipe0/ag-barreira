# Simulação de Algoritmo Genético Paralelo com Barreira de Sincronização

Este projeto implementa uma solução em **Linguagem C** para sincronizar múltiplas threads em um ambiente paralelo simulado, utilizando a biblioteca `pthread`. O objetivo é modelar o comportamento de um **Algoritmo Genético (AG)** onde várias "ilhas" (threads) avaliam populações independentes e precisam sincronizar seus estados ao final de cada geração.

## 📋 Descrição do Problema

Em algoritmos genéticos paralelos, diversas sub-populações evoluem simultaneamente. Para manter a consistência do processo evolutivo (migração de indivíduos, elitismo global), é crucial que nenhuma sub-população inicie a **Geração N+1** antes que todas tenham concluído a **Geração N**.

Este projeto resolve esse problema implementando uma **Barreira de Sincronização** personalizada baseada no conceito de **Monitores** (Mutex + Variáveis de Condição), atendendo aos requisitos de sistemas de Tempo Real.

## 🚀 Funcionalidades

*   **Sincronização Robusta:** Implementação manual de uma barreira usando `pthread_mutex` e `pthread_cond`, sem depender de `pthread_barrier_wait` (para fins didáticos e de controle).
*   **Simulação de Carga:** Cada thread possui um custo computacional (`execution_cost_ms`) simulado, variando entre as threads para demonstrar a espera na barreira.
*   **Restrições de Tempo Real:** Monitoramento de *Deadlines* (prazos) por geração, alertando caso uma thread demore mais do que o permitido (`deadline_ms`).
*   **Escalabilidade:** Fácil ajuste do número de threads (ilhas) e gerações via constantes.

## 🛠️ Estrutura do Projeto

O projeto está organizado da seguinte forma:

```
projeto_ag_barreira/
├── Makefile # Script de automação para compilação e limpeza
├── README.md # Documentação do projeto
└── src/
├── main.c # Lógica principal, criação das threads e simulação do AG
└── barrier.h # Definições das estruturas da barreira e tarefas
```


## 💻 Pré-requisitos (Linux)

*   **Compilador GCC:** Suporte a C99 ou superior.
*   **Ambiente Linux/Unix:** Devido ao uso da biblioteca `pthread` e `unistd.h`. (No Windows, recomenda-se usar WSL).
*   **Make:** Para utilizar a automação de build.

## 💻 Pré-requisitos (Windows)

*  **MSYS2**

## Instalando no Windows:

* Dentro do **MSYS2 MSYS**, rodar o comando abaixo para atualizar o sistema:

```
pacman -Syu
```
* Abrir o **MSYS2 MinGW64**
* Instalar o ambiente **MinGW-w64 com suporte a pthread** 

```
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-libwinpthread-git
```

* Clonar o repositório

```
git clone https://github.com/enzo-felipe0/ag-barreira.git
cd ag-barreira
```
## Adaptação no Makefile

Antes de compilar e executar é extremamente importante adaptar o Makefile para o seu sistema operacional.

* Linux: 
```
-pthread
```

* Windows (MinGW):
```
-lpthread
```

## ⚙️ Compilação e Execução

Utilize o `Makefile` incluído para gerenciar o projeto facilmente.

1.  **Compilar o projeto:**
    ```
    make all
    ou
    mingw32-make all
    ```
    Isso gerará um executável chamado `main`.

2.  **Executar a simulação:**
    ```
    make run
    ou
    mingw32-make run
    ```
    Ou manualmente: `./main`

3.  **Limpar arquivos compilados:**
    ```
    make clean
    ou
    mingw32-make clean
    ```

## 📊 Entendendo a Saída (Log)

Ao executar, você verá logs indicando o progresso de cada thread. Exemplo de fluxo esperado:

1.  Threads iniciam a **Geração 1** com custos diferentes.
2.  Threads rápidas terminam primeiro e **bloqueiam** na barreira (status "Esperando na barreira...").
3.  O sistema aguarda a thread mais lenta terminar.
4.  Quando a última thread chega, a barreira "abre" (log: `[SISTEMA] Barreira completada!`).
5.  Todas as threads são liberadas simultaneamente para iniciar a **Geração 2**.

Exemplo visual do terminal:

```
Thread 0: Terminou Geração 1 em 200ms. Esperando na barreira...
Thread 1: Terminou Geração 1 em 350ms. Esperando na barreira...
...
Thread 3: Terminou Geração 1 em 650ms. Esperando na barreira...
[SISTEMA] Barreira completada! Todas 4 threads chegaram. Liberando...
Thread 3: Passou da barreira. Preparando próxima geração.
Thread 0: Passou da barreira. Preparando próxima geração.
```


## 🧩 Detalhes Técnicos da Implementação

### A Barreira (Monitor)
Utilizamos um **Monitor** composto por:
*   **Mutex:** Garante exclusão mútua ao incrementar o contador de threads que chegaram.
*   **Variável de Condição (`pthread_cond_t`):** Permite que threads "durmam" eficientemente (sem *busy wait*) até serem notificadas via `pthread_cond_broadcast`.
*   **Controle de Fase:** Uma variável `generation_count` evita o problema de *spurious wakeups*, garantindo que threads só avancem quando a fase da barreira realmente mudar.

### Tempo Real
Cada tarefa (thread) é definida pela struct `TaskInfo`:
*   **C (Custo):** Simulado via `usleep()`.
*   **D (Deadline):** Verificado após cada execução. Se `tempo_gasto > deadline`, um alerta é emitido no console, simulando uma falha de *Soft Real-Time*.

---
**Autores:** Amanda Caroline, Enzo Felipe, Gustavo Antonio e Leonardo Abreu
**Data:** Novembro 2025