/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "colors.h"
#include "game.h"
#include "syscall.h"
#include "i386.h"
void task(int x, int y) {
  		// print_hex(x,8,40,40,C_FG_WHITE|C_BG_BLUE);
      // print_hex(y,8,41,41,C_FG_WHITE|C_BG_BLUE);
    /* Tarea */
    int restantex = MAPA_ANCHO-3;
    int restantey = MAPA_ALTO-3;
    //breakpoint();
    // breakpoint();

     while(1)
    {
        int i;
        for (i = 0; i < restantex; i++)
            syscall_mover(DER);

        restantex = restantex - 3;

        for (i = 0; i < restantey; i++)
            syscall_mover(ABA);

        restantey = restantey - 3;

        for (i = 0; i < restantex; i++)
            syscall_mover(IZQ);

        restantex = restantex - 3;

        for (i = 0; i < restantey; i++)
            syscall_mover(ARR);

        restantey = restantey - 3;
    }
    while(1) { __asm __volatile("mov $2, %%eax":::"eax"); }
}
