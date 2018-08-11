/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "mmu.h"
#include "gdt.h"
#include "screen.h"
tss tss_inicial;
tss tss_idle;
tss tss_pirata;





#define GDT_IDX_TSS_INIT 13
#define GDT_IDX_TSS_IDLE 14

#define TAREA_IDLE 0x16000
#define TAREA_PIRATA 0x10000

#define PILA_NIVEL_3 0x401000
#define BASE_PILA_KERNEL 0x00027000
#define CR3_KERNEL 0x00027000

#define DESPLAZAMIENTO_GDT 15

void tss_inicializar() {
	unsigned int base = (unsigned int) &tss_inicial;
    tss_inicializar_gdt_entry(GDT_IDX_TSS_INIT, base, 0x0);
}

void tss_inicializar_idle() {
	unsigned int base = (unsigned int) &tss_idle;
    tss_inicializar_gdt_entry(GDT_IDX_TSS_IDLE, base, 0x0);

    tss_idle.ptl = 0x0000;
    tss_idle.unused0 = 0x0000;
    tss_idle.esp0 = BASE_PILA_KERNEL;
    tss_idle.ss0 = GDT_IDX_DATA_0 << 3;
    tss_idle.unused1 = 0x0000;
    tss_idle.esp1 = 0x00000000;
    tss_idle.ss1 = 0x0000;
    tss_idle.unused2 = 0x0000;
    tss_idle.esp2 = 0x00000000;
    tss_idle.ss2 = 0x0000;
    tss_idle.unused3 = 0x0000;
    tss_idle.cr3 = CR3_KERNEL;
    tss_idle.eip = TAREA_IDLE;
    tss_idle.eflags = 0x00000202;
    tss_idle.eax = 0x00000000;
    tss_idle.ecx = 0x00000000;
    tss_idle.edx = 0x00000000;
    tss_idle.ebx = 0x00000000;
    tss_idle.esp = BASE_PILA_KERNEL;
    tss_idle.ebp = BASE_PILA_KERNEL;
    tss_idle.esi = 0x00000000;
    tss_idle.edi = 0x00000000;
    tss_idle.es = GDT_IDX_DATA_0 << 3;
    tss_idle.unused4 = 0x0000;
    tss_idle.cs = GDT_IDX_CODE_0 << 3;
    tss_idle.unused5 = 0x0000;
    tss_idle.ss = GDT_IDX_DATA_0 << 3;
    tss_idle.unused6 = 0x0000;
    tss_idle.ds = GDT_IDX_DATA_0 << 3;
    tss_idle.unused7 = 0x0000;
    tss_idle.fs = GDT_IDX_DATA_0 << 3;
    tss_idle.unused8 = 0x0000;
    tss_idle.gs = GDT_IDX_DATA_0 << 3;
    tss_idle.unused9 = 0x0000;
    tss_idle.ldt = 0x0000;
    tss_idle.unused10 = 0x0000;
    tss_idle.dtrap = 0x0000;
    tss_idle.iomap = 0xFFFF;
}




void tss_inicializar_pirata(jugador_t *j, pirata_t p, uint index,uint mision_col,uint mision_fil) {
		uint address_task;
		if(j->index == 0){
				if(p.tipo == 0){
						address_task = 0x11000;
				}else{
						address_task = 0x10000;
				}
		}else{
			if(p.tipo == 0){
					address_task = 0x13000;
			}else{
					address_task = 0x12000;
			}
		}

	tss_inicializar_gdt_entry(index + DESPLAZAMIENTO_GDT, (uint)&(tss_global[index]), 0x0);
	uint cr3_nuevo =  mmu_inicializar_dir_pirata(p.posicion_col,p.posicion_fil,address_task,j);
	tss_global[index].ptl = 0x0000;
    tss_global[index].unused0 = 0x0000;
    tss_global[index].esp0 = mmu_proxima_pagina_fisica_libre() + 0x1000;
    tss_global[index].ss0 = (GDT_IDX_DATA_0 << 3);
    tss_global[index].unused1 = 0x0000;
    tss_global[index].esp1 = 0x00000000;
    tss_global[index].ss1 = 0x0000;
    tss_global[index].unused2 = 0x0000;
    tss_global[index].esp2 = 0x00000000;
    tss_global[index].ss2 = 0x0000;
    tss_global[index].unused3 = 0x0000;
    tss_global[index].cr3 = cr3_nuevo;
    tss_global[index].eip = 0x400000;
    tss_global[index].eflags = 0x00000202;
    tss_global[index].eax = 0x00000000;
    tss_global[index].ecx = 0x00000000;
    tss_global[index].edx = 0x00000000;
    tss_global[index].ebx = 0x00000000;
    tss_global[index].esp = PILA_NIVEL_3 - 0xc;
    tss_global[index].ebp = PILA_NIVEL_3 - 0xc;
    tss_global[index].esi = 0x00000000;
    tss_global[index].edi = 0x00000000;
    tss_global[index].es = (GDT_IDX_DATA_3 << 3 ) | 0x3;
    tss_global[index].unused4 = 0x0000;
    tss_global[index].cs = (GDT_IDX_CODE_3 << 3 ) | 0x3;
    tss_global[index].unused5 = 0x0000;
    tss_global[index].ss = (GDT_IDX_DATA_3 << 3) | 0x3;
    tss_global[index].unused6 = 0x0000;
    tss_global[index].ds = (GDT_IDX_DATA_3 << 3) | 0x3;
    tss_global[index].unused7 = 0x0000;
    tss_global[index].fs = (GDT_IDX_DATA_3 << 3) | 0x3;
    tss_global[index].unused8 = 0x0000;
    tss_global[index].gs = (GDT_IDX_DATA_3 << 3) | 0x3;
    tss_global[index].unused9 = 0x0000;
    tss_global[index].ldt = 0x0000;
    tss_global[index].unused10 = 0x0000;
    tss_global[index].dtrap = 0x0000;
    tss_global[index].iomap = 0xFFFF;

    mmu_mapear_pagina (0x402000,rcr3(),mmu_posXposYfisica(p.posicion_col,p.posicion_fil),0x7);
		// breakpoint();
    uint * pila = (uint *) (0x403000 - 4);
	*pila = mision_fil;
    pila = (uint *) (0x403000 - 8);
	*pila = mision_col;
    pila = (uint *) (0x403000 - 12);
    *pila = 0;
    mmu_unmapear_pagina(0x402000,rcr3());

    // uint cr3 = rcr3();
    // lcr3(tss_global[index].cr3);
    // breakpoint();
    // lcr3(cr3);
}

void tss_inicializar_gdt_entry(uint index, uint base, uint dpl_atributos) {
    gdt[index].limit_0_15 = 0x0067;
    gdt[index].base_0_15 = base & 0xFFFF;
    gdt[index].base_23_16 = (base >> 16) & 0xFF;
    gdt[index].type = 0x9;
    gdt[index].s = 0x0;
    gdt[index].dpl = dpl_atributos;
    gdt[index].p = 0x1;
    gdt[index].limit_16_19 = 0x0;
    gdt[index].avl = 0x0;
    gdt[index].l = 0x0;
    gdt[index].db = 0x1;
    gdt[index].g = 0x0;
    gdt[index].base_31_24 = (base >> 24);
}

void tss_eliminar_gdt(uint index){
	gdt[index].limit_0_15 = 0x0000;
	gdt[index].base_0_15 = 0x00;
	gdt[index].base_23_16 = 0x0;
	gdt[index].type = 0x0;
	gdt[index].s = 0x0;
	gdt[index].dpl = 0x0;
	gdt[index].p = 0x0;
	gdt[index].limit_16_19 = 0x0;
	gdt[index].avl = 0x0;
	gdt[index].l = 0x0;
	gdt[index].db = 0x0;
	gdt[index].g = 0x0;
	gdt[index].base_31_24 = 0x0;
}
