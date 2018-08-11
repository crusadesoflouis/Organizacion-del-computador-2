/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/
#ifndef __SCHED_H__

#include "defines.h"
#include "tss.h"

//#include "game.h"
// #include "gdt.h"
#define MAX_TASKS 16
#define INDICE_TAREA_IDLE 16

typedef struct task_t {
  bool      present;
  uint      contex_index;  // es el indice al que apunta el vector dela tss
  pirata_t  pirate;
} __attribute__((__packed__)) task_t;

typedef struct scheduler_t {
  task_t tasks[MAX_TASKS];
  uint current_task;
  uint addres_task_idle;
} __attribute__((__packed__)) scheduler_t;

scheduler_t scheduler;

void sched_initialize();
uint sched_proxima_a_ejecutar();
uint dame_posicion_libre(bool);
uint sched_tick (uint );
void sched_agregar_tarea(jugador_t*, pirata_t);
void sched_sacar_task(uint);
uint sched_indice_libre(bool);
bool sched_hay_lugar(bool);
bool sched_es_vacio();
void sched_eliminar_tarea();




// funciones de la cola

bool mineros_en_espera(jugador_t* j );
void mineros_encolar(jugador_t*,uint col, uint fil);
tupla_t mineros_desencolar(jugador_t*);
#endif	/* !__SCHED_H__ */
