/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/
#include "screen.h"
#include "sched.h"
#include "i386.h"

void sched_initialize(){
  for (uint i = 0; i < MAX_TASKS; i++) {
    scheduler.tasks[i].present = false;
    scheduler.tasks[i].contex_index = 0;
    //scheduler.tasks[i].pirate = NULL;
  }
  scheduler.current_task = 16;
  scheduler.addres_task_idle = 0x70;
}

bool sched_es_indice_valido(uint indice) {
  return 0 <= indice && indice < MAX_TASKS;
}

void sched_agregar_tarea(jugador_t* j, pirata_t p) {
  task_t tarea;
  //tarea_pirata.present = 0;
  //tarea_pirata.contex_index = indice;
  tarea.pirate = p;

  uint indice = sched_indice_libre(j->index); // 0
  if(sched_es_indice_valido(indice)) {
    // explorador o minero
    tarea.present = 1;
    tarea.contex_index = indice; // 0
    if(sched_es_vacio()) {
      //breakpoint();
      scheduler.current_task = indice; //0
    }

    scheduler.tasks[indice] = tarea;

    tss_inicializar_pirata(j,p,indice,p.mision_col,p.mision_fil); // esto no lo revisamos


    if (j->index== 0) {
      screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,p.posicion_fil+2,p.posicion_col+1);
      screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,p.posicion_fil+2,p.posicion_col);
      screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,p.posicion_fil+2,p.posicion_col-1);
      screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,p.posicion_fil+1,p.posicion_col+1);
      screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,p.posicion_fil+1,p.posicion_col);
      screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,p.posicion_fil+1,p.posicion_col-1);
      screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,p.posicion_fil,p.posicion_col+1);
      screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,p.posicion_fil,p.posicion_col);
      screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,p.posicion_fil,p.posicion_col-1);
      // if (MINERO == p.tipo) {
      //   screen_pintar('M',C_FG_WHITE|C_BG_BLUE,p.posicion_fil+1,p.posicion_col);
      // }else{
      //   screen_pintar('E',C_FG_WHITE|C_BG_BLUE,p.posicion_fil+1,p.posicion_col);
      // }
    }
    else{
      screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,p.posicion_fil+1,p.posicion_col-2);
      screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,p.posicion_fil+1,p.posicion_col-1);
      screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,p.posicion_fil+1,p.posicion_col);
      screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,p.posicion_fil,p.posicion_col-2);
      screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,p.posicion_fil,p.posicion_col-1);
      screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,p.posicion_fil,p.posicion_col);
      screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,p.posicion_fil-1,p.posicion_col-2);
      screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,p.posicion_fil-1,p.posicion_col-1);
      screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,p.posicion_fil-1,p.posicion_col);
      // if (MINERO == p.tipo) {
      //   screen_pintar('M',C_FG_WHITE|C_BG_RED,p.posicion_fil,p.posicion_col-1);
      // }else{
      //   screen_pintar('E',C_FG_WHITE|C_BG_RED,p.posicion_fil,p.posicion_col-1);
      // }
    }


  } else {
    // agregar minero a la cola
  }
}

bool sched_es_vacio() {
  for (uint i = 0; i < MAX_TASKS; i++) {
    if(scheduler.tasks[i].present == 1){
      return false;
    }
  }
  return true;
}

uint sched_indice_libre(bool j){
  for(uint i = j; i < 16; i = i + 2) {
    if(scheduler.tasks[i].present == 0){
      return i;
    }
  }
  return 16;
}

bool sched_hay_lugar(bool j) {
  for(uint i = j; i < 16; i = i + 2) {
    if(scheduler.tasks[i].present == 0){
      return true;
    }
  }
  return false;
}

uint sched_proxima_de_jugador(uint indice_jugador) { // indice_jugador = B = 1 // B
  indice_jugador = (indice_jugador + 1) % 2; // B
  uint siguiente = scheduler.current_task + 1 >= MAX_TASKS ? 0 : scheduler.current_task + 1; // 1
  for (int i = siguiente; i != scheduler.current_task; i++) {

     // print_hex(i,2,35,35,C_FG_WHITE|C_BG_RED);
     //
     // breakpoint();
    if(i == MAX_TASKS) {
      i = -1;
      continue;
    }
    if(i % 2 == indice_jugador && scheduler.tasks[i].present == true) {
      // print_hex(88,2,40,40,C_FG_WHITE|C_BG_BLUE);
      return i;
    }
  }
  // screen_pintar((unsigned char)scheduler.current_task,C_FG_WHITE|C_BG_BLACK,35,45);
  // print_hex(90,2,40,40,C_FG_WHITE|C_BG_BLUE);
  return scheduler.current_task; // 0
}

//caso bordes
//[]: devuelve 16 = idle
//[a]: devuelve a
//[b]: devuelve b
//[b,a]:a devuelve b
//[b,a]:b devuelve a

// uint sched_proxima_jugadorA(uint indice){
//
// for (uint  i = indice; i < 16; i+2) {
//   if (scheduler.tasks[i].presente == 1) {
//     return i;
//   }
// }
//
// for (uint  i = 0; i < indice-2; i+2) {
//   if (scheduler.tasks[i].presente == 1) {
//     return i;
//   }
// }
// return 16;
// }
//
//
// uint sched_proxima_jugadorB(uint indice){
//
// for (uint  i = indice; i < 16; i+2) {
//   if (scheduler.tasks[i].presente == 1) {
//     return i;
//   }
// }
//
// for (uint  i = 1; i < indice-2; i+2) {
//   if (scheduler.tasks[i].presente == 1) {
//     return i;
//   }
// }
// return 16;
// }

// uint jugador = scheduler.current_task %2;
// if (jugador == 0 ) {
//   indice_a_ejecutar = sched_proxima_jugadorA(scheduler.current_task + 1);
//   if (indice_a_ejecutar == 16) {
//     indice_a_ejecutar = sched_proxima_jugadorB(scheduler.current_task);
//   }
// }
// else{




uint sched_proxima_a_ejecutar() {
if(!sched_es_vacio()){
  if(!debug_screen){
    uint indice_a_ejecutar = sched_proxima_de_jugador(scheduler.current_task % 2);
    if(indice_a_ejecutar == scheduler.current_task){
      indice_a_ejecutar =  sched_proxima_de_jugador((scheduler.current_task+1) % 2);
    }
    scheduler.current_task = indice_a_ejecutar;
    if(indice_a_ejecutar != 16){
      return ((15 + indice_a_ejecutar) << 3); // 0
    }else{
      return (14 << 3);
    }
  }else{
    return (14 << 3);
  }
}else
  return (14 << 3);
}



void sched_eliminar_tarea(){
uint tarea_actual = scheduler.current_task;
scheduler.tasks[tarea_actual].present = 0;
screen_matar_tarea_pirata(tarea_actual);
tss_eliminar_gdt(tarea_actual + 15);
scheduler.current_task = 16;
}


bool mineros_en_espera(jugador_t* j ){
	for (uint i = 0; i < 8; i++) {
		if (j->mineros_espera[i].col != 0) {
			return true;
		}
	}
	return false;
}


void mineros_encolar(jugador_t* j,uint col, uint fil){
	uint i = 0;
	while (i < 8) {
		if (j->mineros_espera[i].col != 0 ) {
			i++;
		}
	}
	j->mineros_espera[i].col = col;
	j->mineros_espera[i].fil = fil;
}

tupla_t mineros_desencolar(jugador_t* j){
	tupla_t tupla;
	uint i = 0;
	tupla.col = j->mineros_espera[0].col;
	tupla.fil = j->mineros_espera[0].fil;
	while (i < 7) {
		j->mineros_espera[i].col = j->mineros_espera[i+1].col;
		j->mineros_espera[i].fil = j->mineros_espera[i+1].fil;
		i++;
		}
	j->mineros_espera[7].col = 0;
	j->mineros_espera[7].fil = 0;
return tupla;
}
