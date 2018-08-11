/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/



// print_hex(eax,2,35,35,C_FG_WHITE|C_BG_BLUE);
  // breakpoint();

#include "game.h"
#include "mmu.h"
#include "tss.h"
#include "screen.h"
#include "i386.h"
#include <stdarg.h>

char* mensajes_del_procesador[19]={"Divide-by-zero (0)","reserved (1)","NMI Interrupt (2)","Breakpoint (3)","Overflow (4)",
																	"BOUND Range Exceeded (5)","Invalid Opcode (6)","Device Not Available (7)","Double Fault (8)",
																	"Coprocessor Segment Overrun (9)","Invalid TSS (10)","Segment not present (11)","Stack-Segment Fault (12)",
																	"General Protection Fault (13)","Page Fault (14)","reserved (15)","x87 FPU Foating-Point Error (16)",
																	"Alignment Check (17)","Machine Check (18)","SIMD Floating-Point Exception (19)"};


#define POS_INIT_A_X                      1
#define POS_INIT_A_Y                      1
#define POS_INIT_B_X         MAPA_ANCHO - 2
#define POS_INIT_B_Y          MAPA_ALTO - 2

#define CANT_POSICIONES_VISTAS            9
#define MAX_SIN_CAMBIOS                 999

#define BOTINES_CANTIDAD 8

uint botines[BOTINES_CANTIDAD][3] = { // TRIPLAS DE LA FORMA (X, Y, MONEDAS)
                                        {30,  3, 50}, {31, 38, 50}, {15, 21, 100}, {45, 21, 100} ,
                                        {49,  3, 50}, {48, 38, 50}, {64, 21, 100}, {34, 21, 100}
                                    };


jugador_t jugadorA;
jugador_t jugadorB;


extern bool debug_mode = false;

void* error()
{
	__asm__ ("int3");
	return 0;
}

uint game_xy2lineal (uint x, uint y) {
	return (y * MAPA_ANCHO + x);
}

uint game_posicion_valida(int x, int y) {
	return (x >= 0 && y >= 0 && x < MAPA_ANCHO && y < MAPA_ALTO);
}


uint game_dir2xy(direccion dir, int *x, int *y)
{
	switch (dir)
	{
		case IZQ: *x = -1; *y =  0; break;
		case DER: *x =  1; *y =  0; break;
		case ABA: *x =  0; *y =  1; break;
		case ARR: *x =  0; *y = -1; break;
    	default: return -1;
	}

	return 0;
}

void game_actualizar_valor_botin(uint x, uint y) {
  int i;
  for (i = 0; i < BOTINES_CANTIDAD; i++)
  {
    if (botines[i][0] == x && botines[i][1] == y) {
      botines[i][2]--;
      break;
    }
  }
}

uint game_valor_tesoro(uint x, uint y)
{
	int i;
	for (i = 0; i < BOTINES_CANTIDAD; i++)
	{
		if (botines[i][0] == x && botines[i][1] == y)
			return botines[i][2];
	}
	return 0;
}

// dada una posicion (x,y) guarda las posiciones de alrededor en dos arreglos, uno para las x y otro para las y
void game_calcular_posiciones_vistas(int *vistas_x, int *vistas_y, int x, int y)
{
	int next = 0;
	int i, j;
	for (i = -1; i <= 1; i++)
	{
		for (j = -1; j <= 1; j++)
		{
			vistas_x[next] = x + j;
			vistas_y[next] = y + i;
			next++;
		}
	}
}


void game_inicializar(){
  game_jugador_inicializar(&jugadorA);
  game_jugador_inicializar(&jugadorB);
}


void game_jugador_inicializar(jugador_t *j)
{
	static int index = 0;
  j->index = index++;
  j->paginas[0] = mmu_proxima_pagina_fisica_libre();
  j->paginas[1] = mmu_proxima_pagina_fisica_libre();
  j->paginas[2] = mmu_proxima_pagina_fisica_libre();
  j->paginas[3] = mmu_proxima_pagina_fisica_libre();
  for (uint i = 0; i < MAX_CANT_PIRATAS_VIVOS; i++) {
      j->piratas[i].vivo = false;
    }
  j->cant_piratas = 0;
  j->puntos = 0;
	for (uint i = 0; i < 8; i++) {
		j->mineros_espera[i].col = 0;
		j->mineros_espera[i].fil = 0;
	}


}

void game_pirata_inicializar(pirata_t *pirata, jugador_t *j, tipo_pirata tipo,uint mision_col,uint mision_fil){
  pirata->jugador = j;
  if (j->index == 0) {
    // pirata->posicion_col = 1;
    pirata->posicion_col = 1;
    pirata->posicion_fil = 1;
  }
  else{
    pirata->posicion_col = 79;
    pirata->posicion_fil = 43;
  }
  pirata->tipo = tipo;
  pirata->vivo = true;
  pirata->mision_col = mision_col;
  pirata->mision_fil = mision_fil;
}

void game_tick(uint id_pirata)
{
	screen_actualizar_reloj_global();
	screen_actualizar_reloj_pirata(scheduler.current_task);

}



void game_pirata_relanzar(pirata_t *pirata, jugador_t *j, uint tipo)
{
}

pirata_t* game_jugador_erigir_pirata(jugador_t *j, uint tipo)
{
    // ~ completar ~

	return NULL;
}




void game_jugador_lanzar_pirata(jugador_t *j, tipo_pirata tipo, uint col, uint fil) {
  if((tipo == EXPLORADOR && sched_hay_lugar(j->index)) || (tipo == MINERO)) {
    pirata_t pirata;
    game_pirata_inicializar(&pirata, j, tipo, col, fil);
    // print_hex(pirata.tipo,2,35,35,C_FG_WHITE|C_BG_BLUE);
    sched_agregar_tarea(j, pirata);
  }
}

//void game_jugador_lanzar_pirata(jugador_t *j, uint tipo){
//  pirata_t pirata;
//  game_pirata_inicializar(&pirata,j,0,tipo);
//  uint indice = dame_posicion_libre(j -> index);
//  tss_inicializar_pirata(j,&pirata,indice);
//  agregar_tarea(indice,&pirata);
//  //uint hola = rcr3();
////  indice = scheduler.tasks[0].contex_index;
//  //lcr3(tss_global[indice].cr3);
//  //lcr3(hola);
//}

void game_pirata_habilitar_posicion(jugador_t *j, pirata_t *pirata, int x, int y)
{
}


void game_explorar_posicion(jugador_t *jugador, int c, int f)
{
}


uint game_syscall_pirata_mover(uint id, direccion dir)
{
    // ~ completar
    return 0;
}


uint game_syscall_pirata_posicion(uint id, int idx)
{
    // ~ completar ~
    return 0;
}

uint game_syscall_manejar(uint syscall, uint param1)
{
    // ~ completar ~
    return 0;
}

void game_pirata_exploto(uint id)
{
}

pirata_t* game_pirata_en_posicion(uint x, uint y)
{
	return NULL;
}


void game_jugador_anotar_punto(jugador_t *j) {
  j->puntos++;
}



void game_terminar_si_es_hora()
{
}


#define KB_w        0x11 // 0x91
#define KB_s        0x1f // 0x9f
#define KB_a        0x1e // 0x9e
#define KB_d        0x20 // 0xa0
#define KB_e        0x12 // 0x92
#define KB_q        0x10 // 0x90
#define KB_i        0x17 // 0x97
#define KB_k        0x25 // 0xa5
#define KB_j        0x24 // 0xa4
#define KB_l        0x26 // 0xa6
#define KB_shiftL   0x2a // 0xaa
#define KB_shiftR   0x36 // 0xb6
#define KB_shiftR_break 0xb6
#define KB_shiftL_break 0xaa
#define KB_y_break 0x95


void game_atender_teclado(unsigned char tecla)
{

	if(debug_screen == 1){
    if(KB_y_break == tecla){
      screen_hide_debug_info();
    }
  }else{
    if(KB_shiftL_break == tecla){
    screen_pintar('S',C_FG_LIGHT_RED,0,78);
    screen_pintar('L',C_FG_LIGHT_RED,0,79);
     game_jugador_lanzar_pirata(&jugadorA,EXPLORADOR,0,0);
		//game_jugador_lanzar_minero(&jugadorA,5,2);
  }else{
    if(KB_shiftR_break == tecla){
      screen_pintar('S',C_FG_LIGHT_RED,0,78);
      screen_pintar('R',C_FG_LIGHT_RED,0,79);
      game_jugador_lanzar_pirata(&jugadorB,EXPLORADOR,0,0);
    }else{
      if(KB_y_break == tecla){
        screen_pintar('Y',C_FG_LIGHT_RED,0,78);
        screen_pintar('Y',C_FG_LIGHT_RED,0,79);
        if(debug_mode == true){
          debug_mode = false;
        }else{
          debug_mode = true;
        }
        screen_show_debug_mode();
      }
    }
  }
  }
}





uint syscall_handler_juego(uint ecx,uint eax){

  if (eax == 1) {

    game_syscall_mover(ecx);
  }
  if (eax == 2) {
    game_syscall_cavar();
  }
  if (eax == 3) {
    return game_syscall_posicion(ecx);
  }
  //falta agregar una rama de matar tarea
  //matar_tarea(holis);
  return -1;
}


uint game_syscall_posicion(int dir){
uint indice = 0;
uint col = 0;
uint fil = 0;
uint jugador = 0;
indice = scheduler.current_task;
if (dir == -1) {
	col = scheduler.tasks[indice].pirate.posicion_col;
	fil = scheduler.tasks[indice].pirate.posicion_fil;
}
else{
jugador = scheduler.tasks[indice].pirate.jugador->index;

	if (jugador == 0) {
		col = scheduler.tasks[dir*2].pirate.posicion_col;
	 	fil = scheduler.tasks[dir*2].pirate.posicion_fil;
	}
	else{
		col = scheduler.tasks[(dir*2)+1].pirate.posicion_col;
		fil = scheduler.tasks[(dir*2)+1].pirate.posicion_fil;
	}
}
  return ((fil<<8) | col);
}





void game_syscall_cavar() {
  uint indice_pirata_actual = scheduler.current_task;
  pirata_t *pirata_actual = &scheduler.tasks[indice_pirata_actual].pirate;
  uint botin = game_valor_tesoro(pirata_actual->posicion_col, pirata_actual->posicion_fil);
  if(botin == 0) {
		// breakpoint();
		sched_eliminar_tarea();
		saltar_idle();
  } else {
    game_jugador_anotar_punto(pirata_actual->jugador);
    screen_pintar_puntajes(pirata_actual->jugador);
    game_actualizar_valor_botin(pirata_actual->posicion_col, pirata_actual->posicion_fil);
  }
}

//analizar todo el codigo ya que no hay
void game_syscall_mover(direccion dir) {
  //game_posicion_valida
  //del scheduler nos vamos a traer la tarea actual
  // breakpoint();
  uint indice_pirata_actual = scheduler.current_task;
  pirata_t *pirata_actual = &scheduler.tasks[indice_pirata_actual].pirate;

  uint nueva_posicion_col = pirata_actual->posicion_col;
  uint nueva_posicion_fil = pirata_actual->posicion_fil;

  uint nueva_ady_col_1 = 0;
  uint nueva_ady_fil_1 = 0;
  uint nueva_ady_col_2 = 0;
  uint nueva_ady_fil_2 = 0;
	uint nueva_ady_col_3 = 0;
	uint nueva_ady_fil_3 = 0;



  if(dir == DER){
    nueva_posicion_col++;
    nueva_ady_col_1 = pirata_actual->posicion_col + 2;
    nueva_ady_fil_1 = pirata_actual->posicion_fil - 1;
    nueva_ady_col_2 = pirata_actual->posicion_col + 2;
    nueva_ady_fil_2 = pirata_actual->posicion_fil;
    nueva_ady_col_3 = pirata_actual->posicion_col + 2;
    nueva_ady_fil_3 = pirata_actual->posicion_fil + 1;
  }

  if(dir == IZQ){
    nueva_posicion_col--;
    nueva_ady_col_1 = pirata_actual->posicion_col - 2;
    nueva_ady_fil_1 = pirata_actual->posicion_fil + 1;
    nueva_ady_col_2 = pirata_actual->posicion_col - 2;
    nueva_ady_fil_2 = pirata_actual->posicion_fil;
    nueva_ady_col_3 = pirata_actual->posicion_col - 2;
    nueva_ady_fil_3 = pirata_actual->posicion_fil - 1;
  }

  if(dir == ARR){
    nueva_posicion_fil--;
    nueva_ady_col_1 = pirata_actual->posicion_col - 1;
    nueva_ady_fil_1 = pirata_actual->posicion_fil - 2;
    nueva_ady_col_2 = pirata_actual->posicion_col;
    nueva_ady_fil_2 = pirata_actual->posicion_fil - 2;
    nueva_ady_col_3 = pirata_actual->posicion_col + 1;
    nueva_ady_fil_3 = pirata_actual->posicion_fil - 2;

  }
  if(dir == ABA){
    nueva_posicion_fil++;
    nueva_ady_col_1 = pirata_actual->posicion_col - 1;
    nueva_ady_fil_1 = pirata_actual->posicion_fil + 2;
    nueva_ady_col_2 = pirata_actual->posicion_col;
    nueva_ady_fil_2 = pirata_actual->posicion_fil + 2;
    nueva_ady_col_3 = pirata_actual->posicion_col + 1;
    nueva_ady_fil_3 = pirata_actual->posicion_fil + 2;
  }

  uint nueva_col = nueva_posicion_col;
  uint nueva_fil = nueva_posicion_fil;


  if(game_posicion_valida(nueva_posicion_col, nueva_posicion_fil)) {

		if ((scheduler.current_task % 2) == 0 ) {
			screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,pirata_actual->posicion_fil+1,pirata_actual->posicion_col);
		}else{
			screen_pintar(' ',C_FG_WHITE|C_BG_CYAN,pirata_actual->posicion_fil+1,pirata_actual->posicion_col);
		}

		//es minero o explorador?
    if(pirata_actual->tipo == EXPLORADOR) {
          // breakpoint();
      pirata_actual->posicion_col = nueva_posicion_col;
      pirata_actual->posicion_fil = nueva_posicion_fil;
			uint nueva_virtual = mmu_posXposYvirtual(nueva_col,nueva_fil);
      uint nueva_fisica = mmu_posXposYfisica(nueva_col,nueva_fil);
      mmu_mapear_pagina(nueva_virtual,rcr3(),nueva_fisica,0x7);
      mmu_copiar_pagina(0x400000,nueva_virtual);
			mmu_mapear_pagina(CODIGO_BASE,rcr3(),mmu_posXposYfisica(nueva_col,nueva_fil),0x7);
			//primer adyacente
      if (game_posicion_valida(nueva_ady_col_1,nueva_ady_fil_1)) {
        uint direccion_virtual_adyacente_1 = mmu_posXposYvirtual(nueva_ady_col_1,nueva_ady_fil_1);
        uint direccion_fisica_adyacente_1 = mmu_posXposYfisica(nueva_ady_col_1,nueva_ady_fil_1);
        mmu_mapear_pagina(direccion_virtual_adyacente_1,rcr3(),direccion_fisica_adyacente_1,0x7);
				if ((scheduler.current_task % 2) == 0 ) {
					screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,nueva_ady_fil_1+1,nueva_ady_col_1);
				}else{
					screen_pintar(' ',C_FG_WHITE|C_BG_CYAN,nueva_ady_fil_1+1,nueva_ady_col_1);
				}
				if(game_valor_tesoro(nueva_ady_col_1,nueva_ady_fil_1) != 0){
					game_jugador_lanzar_minero(pirata_actual->jugador,nueva_ady_col_1,nueva_ady_fil_1);
				}
      }
			//segundo adyacente
      if (game_posicion_valida(nueva_ady_col_2,nueva_ady_fil_2)) {
        uint direccion_virtual_adyacente_2 = mmu_posXposYvirtual(nueva_ady_col_2,nueva_ady_fil_2);
        uint direccion_fisica_adyacente_2 = mmu_posXposYfisica(nueva_ady_col_2,nueva_ady_fil_2);
        mmu_mapear_pagina(direccion_virtual_adyacente_2,rcr3(),direccion_fisica_adyacente_2,0x7);
				if ((scheduler.current_task % 2) == 0 ) {
					screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,nueva_ady_fil_2+1,nueva_ady_col_2);
				}else{
					screen_pintar(' ',C_FG_WHITE|C_BG_CYAN,nueva_ady_fil_2+1,nueva_ady_col_2);
				}
				if(game_valor_tesoro(nueva_ady_col_2,nueva_ady_fil_2) != 0){
					game_jugador_lanzar_minero(pirata_actual->jugador,nueva_ady_col_2,nueva_ady_fil_2);
				}

      }
			//tercero adyacente
			if (game_posicion_valida(nueva_ady_col_3,nueva_ady_fil_3)) {
				uint direccion_virtual_adyacente_3 = mmu_posXposYvirtual(nueva_ady_col_3,nueva_ady_fil_3);
				uint direccion_fisica_adyacente_3 = mmu_posXposYfisica(nueva_ady_col_3,nueva_ady_fil_3);
				mmu_mapear_pagina(direccion_virtual_adyacente_3,rcr3(),direccion_fisica_adyacente_3,0x7);
				if ((scheduler.current_task % 2) == 0 ) {
					screen_pintar(' ',C_FG_WHITE|C_BG_GREEN,nueva_ady_fil_3+1,nueva_ady_col_3);
				}else{
					screen_pintar(' ',C_FG_WHITE|C_BG_CYAN,nueva_ady_fil_3+1,nueva_ady_col_3);
				}
				if(game_valor_tesoro(nueva_ady_col_3,nueva_ady_fil_3) != 0){
					game_jugador_lanzar_minero(pirata_actual->jugador,nueva_ady_col_3,nueva_ady_fil_3);
				}

			}

      if(game_valor_tesoro(nueva_col,nueva_fil) != 0){
        game_jugador_lanzar_minero(pirata_actual->jugador,nueva_col,nueva_fil);
      }
      // print_hex(pirata_actual->jugador->index ,2,35,35,C_FG_WHITE|C_BG_BLUE);
      if (pirata_actual->jugador->index == 0 ) {
        screen_pintar('E',C_FG_WHITE|C_BG_BLUE,nueva_fil+1,nueva_col);
      }
      else{
        screen_pintar('E',C_FG_WHITE|C_BG_RED,nueva_fil+1,nueva_col);
      }

    }else{ // MINERO
      //breakpoint();
      // chekear que se mueva a una posicion mapeada (valida)
      bool cheko = mmu_es_valido(mmu_posXposYvirtual(nueva_col,nueva_fil), rcr3());
			// print_hex(mmu_posXposYvirtual(nueva_fil,nueva_col),20,33,33,C_FG_WHITE|C_BG_BLUE);
			// print_hex(cheko,1,34,34,C_FG_WHITE|C_BG_BLUE);
      if(cheko){

				if (pirata_actual->jugador->index == 0 ) {
					screen_pintar('M',C_FG_WHITE|C_BG_BLUE,nueva_fil+1,nueva_col);
				}
				else{
					screen_pintar('M',C_FG_WHITE|C_BG_RED,nueva_fil+1,nueva_col);
				}

        pirata_actual->posicion_col = nueva_posicion_col;
        pirata_actual->posicion_fil = nueva_posicion_fil;
        uint nueva_virtual = mmu_posXposYvirtual(nueva_posicion_col,nueva_posicion_fil);
        uint nueva_fisica = mmu_posXposYfisica(nueva_posicion_col,nueva_posicion_fil);
        mmu_mapear_pagina(nueva_virtual,rcr3(),nueva_fisica,0x7);
        mmu_copiar_pagina(0x400000,nueva_virtual);
				mmu_mapear_pagina(CODIGO_BASE,rcr3(),mmu_posXposYfisica(nueva_posicion_col,nueva_posicion_fil),0x7);
				// breakpoint();
			}else{
				// print_hex(nueva_posicion_col,20,35,35,C_FG_WHITE|C_BG_BLUE);
				// print_hex(nueva_posicion_fil,20,36,36,C_FG_WHITE|C_BG_BLUE);
				// breakpoint();
				screen_matar_tarea_pirata(scheduler.current_task);
        sched_eliminar_tarea();
				saltar_idle();
      }
    }
  }
}

void game_jugador_lanzar_minero(jugador_t* j, uint col, uint fil){
  game_jugador_lanzar_pirata(j, MINERO,col,fil);
}




void manejo_exepciones_procesador(unsigned int interruption) {
	print (mensajes_del_procesador[interruption],40,40,C_FG_WHITE | C_BG_BLACK);
 // breakpoint();
  //breakpoint();
	// mostrar la pantalla de debug y despues restablecerla
	// breakpoint();

		// breakpoint();
	sched_eliminar_tarea();

}
