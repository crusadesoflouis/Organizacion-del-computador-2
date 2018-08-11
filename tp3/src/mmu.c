/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"
#include "screen.h"
#include "defines.h"
#define PDE_OFFSET(direccion_virtual) direccion_virtual >> 22;
#define PTE_OFFSET(direccion_virtual) (direccion_virtual << 10)>> 22;

unsigned int proxima_pagina_libre;

/* Atributos paginas */
/* -------------------------------------------------------------------------- */

/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */

//
//

/* inicializar tabla de paginas  */

void mmu_mapear_dir_kernel(unsigned int DDB, unsigned int DTB) {
  directory_table_entry* dte = (directory_table_entry*)(DDB);
  page_table_entry* pte = (page_table_entry*)(DTB);

  dte[0].p          = 1;
  dte[0].rw         = 1;
  dte[0].us         = 0;
  dte[0].pwt        = 0;
  dte[0].pcd        = 0;
  dte[0].a          = 0;
  dte[0].ign        = 0;
  dte[0].ps         = 0;
  dte[0].ignored    = 0;
  dte[0].address    = DIRECCION_TABLA_BASE >> 12;

  for (unsigned int i = 0; i < PAGE_COUNT; i++) {
    pte[i].p          = 1;
    pte[i].rw         = 1;
    pte[i].us         = 0;
    pte[i].pwt        = 0;
    pte[i].pcd        = 0;
    pte[i].a          = 0;
    pte[i].d          = 0;
    pte[i].pat        = 0;
    pte[i].g          = 0;
    pte[i].ignored    = 0;
    pte[i].address    = i;
  }
}

void mmu_inicializar_dir_kernel(){
	mmu_mapear_dir_kernel(DIRECCION_DIRECTORIO_BASE, DIRECCION_TABLA_BASE);
}

// void mmu_prueba(){
//   uint cr3 = rcr3();
//   game_jugador_inicializar(&jugadorA);
//   uint cr3_pirata = mmu_inicializar_dir_pirata(1,2,0x16000,&jugadorA);
//   //print_hex(cr3_pirata,8,20,20,C_FG_WHITE|C_BG_RED);
//   lcr3(cr3_pirata);
//   tlbflush();
//   lcr3(cr3);
//   tlbflush();
// }


void mmu_inicializar() {
  proxima_pagina_libre = INICIO_PAGINAS_LIBRES;
}

unsigned int mmu_proxima_pagina_fisica_libre() {
  unsigned int pagina_libre = proxima_pagina_libre;
  proxima_pagina_libre += TAMANO_PAGINA;
  return pagina_libre;
}

uint mmu_inicializar_dir_pirata(uint columna, uint fila, uint dir_codigo_tarea,jugador_t * j) {
	// print_hex(0x01234567,8,30,30,C_FG_WHITE|C_BG_BLACK);
  //mmu_mapear_pagina(0x400000,0x027000,0x500000,0x7);
  //mmu_unmapear_pagina(0x3ff00,0x027000);
  unsigned int pd = mmu_proxima_pagina_fisica_libre(); // creo su directorio de pagina del pirata
  unsigned int pt = mmu_proxima_pagina_fisica_libre(); // creo su directorio de pagina del pirata
  mmu_mapear_dir_kernel(pd,pt);
  //mapeamos el codigo de la direccion 0x400000 a la direccion del mapa
  mmu_mapear_pagina(CODIGO_BASE,pd,mmu_posXposYfisica(columna,fila),0x7);
  // copiamos la direccion del codigo de la tarea a la direccion 4000000 que es la
  // direccion virtual del codigo base
  mmu_mapear_pagina(0x500000,rcr3(),mmu_posXposYfisica(columna,fila),0x7);

  mmu_copiar_pagina(dir_codigo_tarea,0x500000);

  mmu_unmapear_pagina(0x500000,rcr3());
  directory_table_entry* page_dir = (directory_table_entry*) pd;
  //poner las 4 entries
  page_dir[2].p        =   1;
  page_dir[2].rw       =   1;
  page_dir[2].us       =   1;
  page_dir[2].address  =   j->paginas[0]>>12;
  page_dir[3].p        =   1;
  page_dir[3].rw       =   1;
  page_dir[3].us       =   1;
  page_dir[3].address  =   j->paginas[1]>>12;
  page_dir[4].p        =   1;
  page_dir[4].rw       =   1;
  page_dir[4].us       =   1;
  page_dir[4].address  =   j->paginas[2]>>12;
  page_dir[5].p        =   1;
  page_dir[5].rw       =   1;
  page_dir[5].us       =   1;
  page_dir[5].address  =   j->paginas[3]>>12;

  //mapeamos el todo mapa descuebierto por el pirata
  mmu_mapear_pagina(mmu_posXposYvirtual(columna-1,fila),pd,mmu_posXposYfisica(columna-1,fila),0x7);
  mmu_mapear_pagina(mmu_posXposYvirtual(columna,fila),pd,mmu_posXposYfisica(columna,fila),0x7);
  mmu_mapear_pagina(mmu_posXposYvirtual(columna+1,fila),pd,mmu_posXposYfisica(columna+1,fila),0x7);
  mmu_mapear_pagina(mmu_posXposYvirtual(columna,fila-1),pd,mmu_posXposYfisica(columna,fila-1),0x7);
  mmu_mapear_pagina(mmu_posXposYvirtual(columna,fila+1),pd,mmu_posXposYfisica(columna,fila+1),0x7);
  mmu_mapear_pagina(mmu_posXposYvirtual(columna-1,fila-1),pd,mmu_posXposYfisica(columna-1,fila-1),0x7);
  mmu_mapear_pagina(mmu_posXposYvirtual(columna+1,fila+1),pd,mmu_posXposYfisica(columna+1,fila+1),0x7);
  mmu_mapear_pagina(mmu_posXposYvirtual(columna-1,fila+1),pd,mmu_posXposYfisica(columna-1,fila+1),0x7);
  mmu_mapear_pagina(mmu_posXposYvirtual(columna+1,fila-1),pd,mmu_posXposYfisica(columna+1,fila-1),0x7);


    return (uint) pd;

}

uint mmu_posXposYfisica(uint columna, uint fila){
	return MAPA_BASE_FISICA + (fila * PAGE_SIZE * MAPA_ANCHO) + (columna * PAGE_SIZE);
}

uint mmu_posXposYvirtual(uint columna, uint fila){
	return MAPA_BASE_VIRTUAL + (fila * PAGE_SIZE * MAPA_ANCHO) + (columna * PAGE_SIZE);
}

void mmu_copiar_pagina(uint src, uint dst){
	uint* src_ptr = (uint*) src;
	uint* dst_ptr = (uint*) dst;
	uint i;
	for(i=0; i < PAGE_COUNT; i++){
		dst_ptr[i] = src_ptr[i];
	}
}

void mmu_mapear_pagina(unsigned int direccion_virtual, unsigned int cr3, unsigned int direccion_fisica,unsigned char atri){
  directory_table_entry* dir_paginas = (directory_table_entry*) cr3;
  page_table_entry* tabla_pagina;
  unsigned int dir_i = PDE_OFFSET(direccion_virtual);
  unsigned int tabla_i = PTE_OFFSET(direccion_virtual);
  directory_table_entry* dir = &(dir_paginas[dir_i]);
  unsigned char atri_p = atri & 0x1;
  unsigned char atri_rw = (atri & 0x2) >> 1;
  unsigned char atri_us = (atri & 0x4) >> 2;
  if (dir->p == 1) {
    tabla_pagina = (page_table_entry*)((unsigned int) dir -> address << 12);
  }else{
  	tabla_pagina = (page_table_entry*)(mmu_proxima_pagina_fisica_libre());
  	unsigned int i;
  	for(i=0; i < PAGE_COUNT; i++){
  		tabla_pagina[i].p = 0;
  	}
  	dir->p = atri_p;
  	dir->rw = atri_rw;
  	dir->us = atri_us;
  	dir->pwt = 0;
  	dir->pcd = 0;
  	dir->a = 0;
  	dir->ign = 0;
  	dir->ps = 0;
  	dir->ignored = 0;
  	dir->address = (unsigned int)tabla_pagina >>12;
  }
  tabla_pagina[tabla_i].p = atri_p;
  tabla_pagina[tabla_i].rw = atri_rw;
  tabla_pagina[tabla_i].us = atri_us;
  tabla_pagina[tabla_i].pwt = 0;
  tabla_pagina[tabla_i].pcd = 0;
  tabla_pagina[tabla_i].a = 0;
  tabla_pagina[tabla_i].d = 0;
  tabla_pagina[tabla_i].pat = 0;
  tabla_pagina[tabla_i].g = 0;
  tabla_pagina[tabla_i].ignored = 0;
  tabla_pagina[tabla_i].address = (direccion_fisica >> 12);
  tlbflush();
}

void mmu_unmapear_pagina(unsigned int direccion_virtual, unsigned int cr3){
  	unsigned int dir_i = PDE_OFFSET(direccion_virtual);
  	unsigned int tabla_i = PTE_OFFSET(direccion_virtual);
  	directory_table_entry* dir = &((directory_table_entry*) cr3)[dir_i];
  	if(dir->p){
  		page_table_entry* tablas = (page_table_entry*)((unsigned int) dir -> address << 12);
  		tablas[tabla_i].p = 0;
  		tlbflush();
  	}
}

bool mmu_es_valido(uint direccion_virtual, uint cr3) {
  unsigned int dir_i = PDE_OFFSET(direccion_virtual);
  unsigned int tabla_i = PTE_OFFSET(direccion_virtual);
  directory_table_entry* dir = &((directory_table_entry*) cr3)[dir_i];
  if(dir->p){
    page_table_entry* tablas = (page_table_entry*)((unsigned int) dir -> address << 12);
    if(tablas[tabla_i].p){
      return true;
    }else{
      return false;
    }
  }else{
    return false;
  }
}
