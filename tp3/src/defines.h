/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

    Definiciones globales del sistema.
*/

#ifndef __DEFINES_H__
#define __DEFINES_H__

/* Tipos basicos */
/* -------------------------------------------------------------------------- */
#define bool char
#define NULL                    0
#define TRUE                    0x00000001
#define FALSE                   0x00000000
#define true                    0x00000001
#define false                   0x00000000
#define ERROR                   1
#define CANT_PAGINAS_MAPA       4
#define MAX_CANT_PIRATAS_VIVOS  8
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;

/* Constantes basicas */
/* -------------------------------------------------------------------------- */
#define PAGE_SIZE               0x00001000
#define TASK_SIZE               4096

#define BOOTSECTOR              0x00001000 /* direccion fisica de comienzo del bootsector (copiado) */
#define KERNEL                  0x00001200 /* direccion fisica de comienzo del kernel */

#define CODIGO_BASE				0x400000   /* direccion del  codigo y pila */
// #define MAPA_BASE_FISICA		0x500000   /* direccion fisica del comienzo del mapa */
#define MAPA_BASE_VIRTUAL		0x800000   /* direccion virtual del comienzo del mapa */
/* Indices en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_COUNT 31
#define GDT_IDX_NULL_DESC           0
#define GDT_IDX_CODE_0              8
#define GDT_IDX_CODE_3              9
#define GDT_IDX_DATA_0              10
#define GDT_IDX_DATA_3              11
#define GDT_IDX_VIDEO               12
/* Offsets en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_OFF_NULL_DESC           (GDT_IDX_NULL_DESC      << 3)

/* Selectores de segmentos */
/* -------------------------------------------------------------------------- */

/* direccones del codigo de las tareas */

#define DIR_CODIGO_TAREA_A_E	0x10000
#define DIR_CODIGO_TAREA_A_M	0x11000
#define DIR_CODIGO_TAREA_B_E	0x12000
#define DIR_CODIGO_TAREA_B_M	0x13000

void* error();

typedef enum tipo_pirata_e { MINERO = 0x0, EXPLORADOR = 0x1} tipo_pirata;
typedef struct pirata_t
{
    struct jugador_t *jugador;
    uint posicion_col;
    uint posicion_fil;
    uint mision_col;
    uint mision_fil;
    tipo_pirata tipo;
    bool vivo;
    // id unica, posicion, tipo, reloj
} pirata_t;

typedef struct tupla_t{
  uint col;
  uint fil;
} tupla_t;

typedef struct jugador_t
{
    uint index; // 0 es jugador A , 1 es jugador B
    uint paginas[CANT_PAGINAS_MAPA];
    pirata_t piratas[MAX_CANT_PIRATAS_VIVOS];
    uint cant_piratas;
    uint puntos;
    tupla_t mineros_espera[8];
    // coordenadas puerto, posiciones exploradas, mineros pendientes, etc
} jugador_t;

typedef struct debug_data_t{
    uint    eax,ebx,ecx,edx,esi,edi,ebp,esp,eip;
    ushort  cs,ds,es,fs,gs,ss;
    uint    eflags,cr0,cr2,cr3,cr4,error_code;
    uint    stack[5];
} __attribute__((__packed__, aligned (8))) debug_data_t;





#define ASSERT(x) while(!(x)) {};


#endif  /* !__DEFINES_H__ */
