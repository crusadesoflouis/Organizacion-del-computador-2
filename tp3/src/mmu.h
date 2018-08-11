/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#ifndef __MMU_H__
#define __MMU_H__

#include "defines.h"
#include "game.h"


#define MAPA_BASE_FISICA  0x500000
#define MAPA_BASE_VIRTUAL 0x800000

#define DIRECCION_DIRECTORIO_BASE   0x27000
#define DIRECCION_TABLA_BASE        0x28000
#define INICIO_PAGINAS_LIBRES       0x100000
#define TAMANO_PAGINA               0x1000

#define PAGE_COUNT 1024



typedef struct str_directory_table_entry {
    unsigned char   p:1;
    unsigned char   rw:1;
    unsigned char   us:1;
    unsigned char   pwt:1;
    unsigned char   pcd:1;
    unsigned char   a:1;
    unsigned char   ign:1;
    unsigned char   ps:1;
    unsigned char   ignored:4;
    unsigned int    address:20;
} __attribute__((__packed__)) directory_table_entry;

typedef struct str_page_table_entry {
    unsigned char   p:1;
    unsigned char   rw:1;
    unsigned char   us:1;
    unsigned char   pwt:1;
    unsigned char   pcd:1;
    unsigned char   a:1;
    unsigned char   d:1;
    unsigned char   pat:1;
    unsigned char   g:1;
    unsigned char   ignored:3;
    unsigned int    address:20;
} __attribute__((__packed__)) page_table_entry;

void mmu_mapear_dir_kernel(unsigned int DDB, unsigned int DTB);
void mmu_inicializar_dir_kernel();
void mmu_inicializar();
unsigned int mmu_proxima_pagina_fisica_libre();
uint mmu_inicializar_dir_pirata(uint columna, uint fila, uint dir_codigo_tarea, jugador_t *j);
void mmu_mapear_pagina(unsigned int direccion_virtual, unsigned int cr3, unsigned int direccion_fisica,unsigned char atri);
void mmu_unmapear_pagina(unsigned int direccion_virtual, unsigned int cr3);
uint mmu_posXposYfisica(uint columna, uint fila);
uint mmu_posXposYvirtual(uint columna, uint fila);
void mmu_copiar_pagina(uint src, uint dst);
bool mmu_es_valido(uint, uint);

#endif	/* !__MMU_H__ */
