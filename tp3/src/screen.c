/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "screen.h"


extern jugador_t jugadorA, jugadorB;


static ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO;

const char reloj[] = "|/-\\";
#define reloj_size 4
#define DBG_OFF_X             25
#define DBG_OFF_Y              6
#define DBG_WIDTH             30
#define DBG_HEIGHT            38

debug_data_t debug_data;
extern bool debug_screen = FALSE;
extern bool debug_mode;


ca p_copia[VIDEO_FILS][VIDEO_COLS];

void screen_actualizar_reloj_global()
{
    static uint reloj_global = 0;

    reloj_global = (reloj_global + 1) % reloj_size;

    screen_pintar(reloj[reloj_global], C_BW, 49, 79);
}


void screen_actualizar_reloj_pirata(uint id_pirata){

  static uint reloj_JugadorA0 = 0;
  static uint reloj_JugadorA1 = 0;
  static uint reloj_JugadorA2 = 0;
  static uint reloj_JugadorA3 = 0;
  static uint reloj_JugadorA4 = 0;
  static uint reloj_JugadorA5 = 0;
  static uint reloj_JugadorA6 = 0;
  static uint reloj_JugadorA7 = 0;
  static uint reloj_JugadorB0 = 0;
  static uint reloj_JugadorB1 = 0;
  static uint reloj_JugadorB2 = 0;
  static uint reloj_JugadorB3 = 0;
  static uint reloj_JugadorB4 = 0;
  static uint reloj_JugadorB5 = 0;
  static uint reloj_JugadorB6 = 0;
  static uint reloj_JugadorB7 = 0;
// screen_pintar((char)id_pirata, C_BW, 48, 60);
  if (id_pirata == 0) {
    reloj_JugadorA0 = (reloj_JugadorA0 + 1) % reloj_size;
    screen_pintar(reloj[reloj_JugadorA0], C_BW, 48, 60);
  }
  if (id_pirata == 1) {
    reloj_JugadorB0 = (reloj_JugadorB0 + 1) % reloj_size;
    screen_pintar(reloj[reloj_JugadorB0], C_BW, 48, 4);
  }
  if (id_pirata == 2) {
    reloj_JugadorA1 = (reloj_JugadorA1 + 1) % reloj_size;
    screen_pintar(reloj[reloj_JugadorA1], C_BW, 48, 62);
  }
  if (id_pirata == 3) {
    reloj_JugadorB1 = (reloj_JugadorB1 + 1) % reloj_size;
    screen_pintar(reloj[reloj_JugadorB1], C_BW, 48, 6);
  }
  if (id_pirata == 4) {
    reloj_JugadorA2 = (reloj_JugadorA2 + 1) % reloj_size;
    screen_pintar(reloj[reloj_JugadorA2], C_BW, 48, 64);
  }
  if (id_pirata == 5) {
    reloj_JugadorB2 = (reloj_JugadorB2 + 1) % reloj_size;
    screen_pintar(reloj[reloj_JugadorB2], C_BW, 48, 8);
  }
  if (id_pirata == 6) {
    reloj_JugadorA3 = (reloj_JugadorA3 + 1) % reloj_size;
    screen_pintar(reloj[reloj_JugadorA3], C_BW, 48, 66);
  }
  if (id_pirata == 7) {
    reloj_JugadorB3 = (reloj_JugadorB3 + 1) % reloj_size;
    screen_pintar(reloj[reloj_JugadorB3], C_BW, 48, 10);
  }
  if (id_pirata == 8) {
    reloj_JugadorA4 = (reloj_JugadorA4 + 1) % reloj_size;
    screen_pintar(reloj[reloj_JugadorA4], C_BW, 48, 68);
  }
  if (id_pirata == 9) {
    reloj_JugadorB4 = (reloj_JugadorB4 + 1) % reloj_size;
    screen_pintar(reloj[reloj_JugadorB4], C_BW, 48, 12);
  }
  if (id_pirata == 10) {
    reloj_JugadorA5 = (reloj_JugadorA5 + 1) % reloj_size;
    screen_pintar(reloj[reloj_JugadorA5], C_BW, 48, 70);
  }
  if (id_pirata == 11) {
    reloj_JugadorB5 = (reloj_JugadorB5 + 1) % reloj_size;
    screen_pintar(reloj[reloj_JugadorB5], C_BW, 48, 14);
  }
  if (id_pirata == 12) {
    reloj_JugadorA6 = (reloj_JugadorA6 + 1) % reloj_size;
    screen_pintar(reloj[reloj_JugadorA6], C_BW, 48, 72);
  }
  if (id_pirata == 13) {
    reloj_JugadorB6 = (reloj_JugadorB6 + 1) % reloj_size;
    screen_pintar(reloj[reloj_JugadorB6], C_BW, 48, 16);
  }
  if (id_pirata == 14) {
    reloj_JugadorA7 = (reloj_JugadorA7 + 1) % reloj_size;
    screen_pintar(reloj[reloj_JugadorA7], C_BW, 48, 74);
  }
  if (id_pirata == 15) {
    reloj_JugadorB7 = (reloj_JugadorB7 + 1) % reloj_size;
    screen_pintar(reloj[reloj_JugadorB7], C_BW, 48, 18);
  }

}





void screen_matar_tarea_pirata(uint id_pirata){
  if (id_pirata == 0) {
    // reloj_JugadorA0 = 0;
    screen_pintar('X', C_FG_BLUE|C_BG_BLACK, 48, 60);
  }
  if (id_pirata == 1) {
    // reloj_JugadorB0 = 0;
    screen_pintar('X', C_FG_RED|C_BG_BLACK, 48, 4);
  }
  if (id_pirata == 2) {
    // reloj_JugadorA1 = 0;
    screen_pintar('X', C_FG_BLUE|C_BG_BLACK, 48, 62);
  }
  if (id_pirata == 3) {
    // reloj_JugadorB1 = 0;
    screen_pintar('X', C_FG_RED|C_BG_BLACK, 48, 6);
  }
  if (id_pirata == 4) {
    // reloj_JugadorA2 = 0;
    screen_pintar('X', C_FG_BLUE|C_BG_BLACK, 48, 64);
  }
  if (id_pirata == 5) {
    // reloj_JugadorB2 = 0;
    screen_pintar('X', C_FG_RED|C_BG_BLACK, 48, 8);
  }
  if (id_pirata == 6) {
    // reloj_JugadorA3 = 0;
    screen_pintar('X', C_FG_BLUE|C_BG_BLACK, 48, 66);
  }
  if (id_pirata == 7) {
    // reloj_JugadorB3 = 0;
    screen_pintar('X', C_FG_RED|C_BG_BLACK, 48, 10);
  }
  if (id_pirata == 8) {
    // reloj_JugadorA4 = 0;
    screen_pintar('X', C_FG_BLUE|C_BG_BLACK, 48, 68);
  }
  if (id_pirata == 9) {
    // reloj_JugadorB4 = 0;
    screen_pintar('X', C_FG_RED|C_BG_BLACK, 48, 12);
  }
  if (id_pirata == 10) {
    // reloj_JugadorA5 = 0;
    screen_pintar('X', C_FG_BLUE|C_BG_BLACK, 48, 70);
  }
  if (id_pirata == 11) {
    // reloj_JugadorB5 = 0;
    screen_pintar('X', C_FG_RED|C_BG_BLACK, 48, 14);
  }
  if (id_pirata == 12) {
    // reloj_JugadorA6 = 0;
    screen_pintar('X', C_FG_BLUE|C_BG_BLACK, 48, 72);
  }
  if (id_pirata == 13) {
    // reloj_JugadorB6 = 0;
    screen_pintar('X', C_FG_RED|C_BG_BLACK, 48, 16);
  }
  if (id_pirata == 14) {
    // reloj_JugadorA7 = 0;
    screen_pintar('X', C_FG_BLUE|C_BG_BLACK, 48, 74);
  }
  if (id_pirata == 15) {
    // reloj_JugadorB7 = 0;
    screen_pintar('X', C_FG_RED|C_BG_BLACK, 48, 18);
  }

}




void screen_pintar(uchar c, uchar color, uint fila, uint columna)
{
    p[fila][columna].c = c;
    p[fila][columna].a = color;
}

uchar screen_valor_actual(uint fila, uint columna)
{
    return p[fila][columna].c;
}

void print(const char * text, uint x, uint y, unsigned short attr) {
    int i;
    for (i = 0; text[i] != 0; i++)
     {
        screen_pintar(text[i], attr, y, x);

        x++;
        if (x == VIDEO_COLS) {
            x = 0;
            y++;
        }
    }
}

void print_hex(uint numero, int size, uint x, uint y, unsigned short attr) {
    int i;
    char hexa[8];
    char letras[16] = "0123456789ABCDEF";
    hexa[0] = letras[ ( numero & 0x0000000F ) >> 0  ];
    hexa[1] = letras[ ( numero & 0x000000F0 ) >> 4  ];
    hexa[2] = letras[ ( numero & 0x00000F00 ) >> 8  ];
    hexa[3] = letras[ ( numero & 0x0000F000 ) >> 12 ];
    hexa[4] = letras[ ( numero & 0x000F0000 ) >> 16 ];
    hexa[5] = letras[ ( numero & 0x00F00000 ) >> 20 ];
    hexa[6] = letras[ ( numero & 0x0F000000 ) >> 24 ];
    hexa[7] = letras[ ( numero & 0xF0000000 ) >> 28 ];
    for(i = 0; i < size; i++) {
        p[y][x + size - i - 1].c = hexa[i];
        p[y][x + size - i - 1].a = attr;
    }
}

void print_dec(uint numero, int size, uint x, uint y, unsigned short attr) {
    int i;
    char letras[16] = "0123456789";

    for(i = 0; i < size; i++) {
        int resto  = numero % 10;
        numero = numero / 10;
        p[y][x + size - i - 1].c = letras[resto];
        p[y][x + size - i - 1].a = attr;
    }
}

void screen_inicializar(){
  for (int i = 0; i < 50; i++) {
    for (int j = 0; j < 80; j++) {
      // screen_pintar('c',C_FG_RED|C_BG_CYAN,i,j);
      screen_pintar(' ',C_FG_BLACK|C_BG_BLACK,i,j);
    }
  }
}

void screen_pintar_puntajes(jugador_t* j) {

if (j->index == 0) {
  print_dec(j->puntos, 3,34,47,C_FG_WHITE|C_BG_RED);
}else{
  print_dec(j->puntos, 3,41,47,C_FG_WHITE|C_BG_BLUE);
}

  // // j->puntos = a b c
  // unsigned char a = j->puntos / 100;
  // unsigned char b = j->puntos % 100;
  // unsigned char c = b % 10;
  // b = b / 10;
  //
  // if(j->index == 0) {
  //   // jugador 1
  //   screen_pintar(a,C_FG_WHITE|C_BG_RED,47,34);
  //   screen_pintar(b,C_FG_WHITE|C_BG_RED,47,35);
  //   screen_pintar(c,C_FG_WHITE|C_BG_RED,47,36);
  // } else {
  //  // jugador 2
  //   screen_pintar(a,C_FG_WHITE|C_BG_BLUE,47,41);
  //   screen_pintar(b,C_FG_WHITE|C_BG_BLUE,47,42);
  //   screen_pintar(c,C_FG_WHITE|C_BG_BLUE,47,43);
  // }
}

void enviar_mensaje() {
  screen_pintar('H',C_FG_WHITE|C_BG_RED,20,20);
  screen_pintar('O',C_FG_WHITE|C_BG_RED,20,21);
  screen_pintar('L',C_FG_WHITE|C_BG_RED,20,22);
  screen_pintar('A',C_FG_WHITE|C_BG_RED,20,23);
}

void pintar_pantalla() {
  for (int i = 1; i < 45; i++) {
    for (int j = 0; j < 80; j++) {
      screen_pintar(' ',C_BG_LIGHT_GREY,i,j);
    }
  }
  for (int i = 45; i < 50; i++) {
    for (int j = 32; j < 39; j++) {
      screen_pintar(' ',C_BG_RED,i,j);
    }
  }
  for (int i = 45; i < 50; i++) {
    for (int j = 39; j < 46; j++) {
      screen_pintar(' ',C_BG_BLUE,i,j);
    }
  }

  screen_pintar('1',C_FG_WHITE|C_BG_BLACK,46,4);
  screen_pintar('2',C_FG_WHITE|C_BG_BLACK,46,6);
  screen_pintar('3',C_FG_WHITE|C_BG_BLACK,46,8);
  screen_pintar('4',C_FG_WHITE|C_BG_BLACK,46,10);
  screen_pintar('5',C_FG_WHITE|C_BG_BLACK,46,12);
  screen_pintar('6',C_FG_WHITE|C_BG_BLACK,46,14);
  screen_pintar('7',C_FG_WHITE|C_BG_BLACK,46,16);
  screen_pintar('8',C_FG_WHITE|C_BG_BLACK,46,18);

  screen_pintar('X',C_FG_RED|C_BG_BLACK,48,4);
  screen_pintar('X',C_FG_RED|C_BG_BLACK,48,6);
  screen_pintar('X',C_FG_RED|C_BG_BLACK,48,8);
  screen_pintar('X',C_FG_RED|C_BG_BLACK,48,10);
  screen_pintar('X',C_FG_RED|C_BG_BLACK,48,12);
  screen_pintar('X',C_FG_RED|C_BG_BLACK,48,14);
  screen_pintar('X',C_FG_RED|C_BG_BLACK,48,16);
  screen_pintar('X',C_FG_RED|C_BG_BLACK,48,18);



  screen_pintar('1',C_FG_WHITE|C_BG_BLACK,46,60);
  screen_pintar('2',C_FG_WHITE|C_BG_BLACK,46,62);
  screen_pintar('3',C_FG_WHITE|C_BG_BLACK,46,64);
  screen_pintar('4',C_FG_WHITE|C_BG_BLACK,46,66);
  screen_pintar('5',C_FG_WHITE|C_BG_BLACK,46,68);
  screen_pintar('6',C_FG_WHITE|C_BG_BLACK,46,70);
  screen_pintar('7',C_FG_WHITE|C_BG_BLACK,46,72);
  screen_pintar('8',C_FG_WHITE|C_BG_BLACK,46,74);

  screen_pintar('X',C_FG_BLUE|C_BG_BLACK,48,60);
  screen_pintar('X',C_FG_BLUE|C_BG_BLACK,48,62);
  screen_pintar('X',C_FG_BLUE|C_BG_BLACK,48,64);
  screen_pintar('X',C_FG_BLUE|C_BG_BLACK,48,66);
  screen_pintar('X',C_FG_BLUE|C_BG_BLACK,48,68);
  screen_pintar('X',C_FG_BLUE|C_BG_BLACK,48,70);
  screen_pintar('X',C_FG_BLUE|C_BG_BLACK,48,72);
  screen_pintar('X',C_FG_BLUE|C_BG_BLACK,48,74);



  screen_pintar('0',C_FG_WHITE|C_BG_RED,47,34);
  screen_pintar('0',C_FG_WHITE|C_BG_RED,47,35);
  screen_pintar('0',C_FG_WHITE|C_BG_RED,47,36);

  screen_pintar('0',C_FG_WHITE|C_BG_BLUE,47,41);
  screen_pintar('0',C_FG_WHITE|C_BG_BLUE,47,42);
  screen_pintar('0',C_FG_WHITE|C_BG_BLUE,47,43);


  screen_pintar('-',C_FG_WHITE|C_BG_BLACK,49,79);
  screen_pintar(')',C_FG_WHITE|C_BG_BLACK,49,77);
  screen_pintar('-',C_FG_WHITE|C_BG_BLACK,49,76);
  screen_pintar('(',C_FG_WHITE|C_BG_BLACK,49,75);



}


void screen_guardar_copia() {
    int i;
    int j;
    for (i = 0; i < VIDEO_FILS; i++) {
        for (j = 0; j < VIDEO_COLS; j++) {
            p_copia[i][j] = p[i][j];
        }
    }
}

void screen_leer_copia() {
    int i;
    int j;
    for (i = 0; i < VIDEO_FILS; i++) {
        for (j = 0; j < VIDEO_COLS; j++) {
            p[i][j] = p_copia[i][j];
        }
    }
}

void screen_show_debug_mode() {
    if (debug_mode) {
        print("Modo DEBUG activado", 30, 0, C_BG_BLACK | C_FG_LIGHT_GREY);
    } else {
        print("                   ", 30, 0, C_BG_BLACK | C_FG_LIGHT_GREY);
    }
}


void screen_show_debug_info() {
    screen_guardar_copia();

    // Fondo
    screen_pintar_rect(' ', C_BG_RED, DBG_OFF_Y, DBG_OFF_X, 4, DBG_WIDTH);
    screen_pintar_rect(' ', C_BG_LIGHT_GREY, DBG_OFF_Y + 4, DBG_OFF_X, DBG_HEIGHT - 4, DBG_WIDTH);

    // Bordes
    screen_pintar_linea_v(' ', C_BG_BLACK, DBG_OFF_Y, DBG_OFF_X, DBG_HEIGHT);
    screen_pintar_linea_v(' ', C_BG_BLACK, DBG_OFF_Y, DBG_OFF_X + DBG_WIDTH - 1, DBG_HEIGHT);
    screen_pintar_linea_h(' ', C_BG_BLACK, DBG_OFF_Y, DBG_OFF_X, DBG_WIDTH);
    screen_pintar_linea_h(' ', C_BG_BLACK, DBG_OFF_Y + DBG_HEIGHT - 1, DBG_OFF_X, DBG_WIDTH);

    // Labels
    print("Excepcion de tipo", DBG_OFF_X + 2, DBG_OFF_Y + 2, C_BG_RED | C_FG_LIGHT_GREY);

    print("eax",        DBG_OFF_X +  2, DBG_OFF_Y +  5, C_BG_LIGHT_GREY | C_FG_RED);
    print("ebx",        DBG_OFF_X +  2, DBG_OFF_Y +  7, C_BG_LIGHT_GREY | C_FG_RED);
    print("ecx",        DBG_OFF_X +  2, DBG_OFF_Y +  9, C_BG_LIGHT_GREY | C_FG_RED);
    print("edx",        DBG_OFF_X +  2, DBG_OFF_Y + 11, C_BG_LIGHT_GREY | C_FG_RED);
    print("esi",        DBG_OFF_X +  2, DBG_OFF_Y + 13, C_BG_LIGHT_GREY | C_FG_RED);
    print("edi",        DBG_OFF_X +  2, DBG_OFF_Y + 15, C_BG_LIGHT_GREY | C_FG_RED);
    print("ebp",        DBG_OFF_X +  2, DBG_OFF_Y + 17, C_BG_LIGHT_GREY | C_FG_RED);
    print("esp",        DBG_OFF_X +  2, DBG_OFF_Y + 19, C_BG_LIGHT_GREY | C_FG_RED);
    print("eip",        DBG_OFF_X +  2, DBG_OFF_Y + 21, C_BG_LIGHT_GREY | C_FG_RED);
    print(" cs",        DBG_OFF_X +  2, DBG_OFF_Y + 23, C_BG_LIGHT_GREY | C_FG_RED);
    print(" ds",        DBG_OFF_X +  2, DBG_OFF_Y + 25, C_BG_LIGHT_GREY | C_FG_RED);
    print(" es",        DBG_OFF_X +  2, DBG_OFF_Y + 27, C_BG_LIGHT_GREY | C_FG_RED);
    print(" fs",        DBG_OFF_X +  2, DBG_OFF_Y + 29, C_BG_LIGHT_GREY | C_FG_RED);
    print(" gs",        DBG_OFF_X +  2, DBG_OFF_Y + 31, C_BG_LIGHT_GREY | C_FG_RED);
    print(" ss",        DBG_OFF_X +  2, DBG_OFF_Y + 33, C_BG_LIGHT_GREY | C_FG_RED);
    print("eflags",     DBG_OFF_X +  2, DBG_OFF_Y + 35, C_BG_LIGHT_GREY | C_FG_RED);

    print("cr0",        DBG_OFF_X + 16, DBG_OFF_Y +  5, C_BG_LIGHT_GREY | C_FG_RED);
    print("cr2",        DBG_OFF_X + 16, DBG_OFF_Y +  7, C_BG_LIGHT_GREY | C_FG_RED);
    print("cr3",        DBG_OFF_X + 16, DBG_OFF_Y +  9, C_BG_LIGHT_GREY | C_FG_RED);
    print("cr4",        DBG_OFF_X + 16, DBG_OFF_Y + 11, C_BG_LIGHT_GREY | C_FG_RED);
    print("error code", DBG_OFF_X + 16, DBG_OFF_Y + 14, C_BG_LIGHT_GREY | C_FG_RED);
    print("stack",      DBG_OFF_X + 16, DBG_OFF_Y + 22, C_BG_LIGHT_GREY | C_FG_RED);

    // Contenido

    print_hex(debug_data.eax,        8, DBG_OFF_X +  6, DBG_OFF_Y +  5, C_BG_LIGHT_GREY | C_FG_BLACK);  // eax
    print_hex(debug_data.ebx,        8, DBG_OFF_X +  6, DBG_OFF_Y +  7, C_BG_LIGHT_GREY | C_FG_BLACK);  // ebx
    print_hex(debug_data.ecx,        8, DBG_OFF_X +  6, DBG_OFF_Y +  9, C_BG_LIGHT_GREY | C_FG_BLACK);  // ecx
    print_hex(debug_data.edx,        8, DBG_OFF_X +  6, DBG_OFF_Y + 11, C_BG_LIGHT_GREY | C_FG_BLACK);  // edx
    print_hex(debug_data.esi,        8, DBG_OFF_X +  6, DBG_OFF_Y + 13, C_BG_LIGHT_GREY | C_FG_BLACK);  // esi
    print_hex(debug_data.edi,        8, DBG_OFF_X +  6, DBG_OFF_Y + 15, C_BG_LIGHT_GREY | C_FG_BLACK);  // edi
    print_hex(debug_data.ebp,        8, DBG_OFF_X +  6, DBG_OFF_Y + 17, C_BG_LIGHT_GREY | C_FG_BLACK);  // ebp
    print_hex(debug_data.esp,        8, DBG_OFF_X +  6, DBG_OFF_Y + 19, C_BG_LIGHT_GREY | C_FG_BLACK);  // esp
    print_hex(debug_data.eip,        8, DBG_OFF_X +  6, DBG_OFF_Y + 21, C_BG_LIGHT_GREY | C_FG_BLACK);  // eip
    print_hex(debug_data.cs,         4, DBG_OFF_X +  6, DBG_OFF_Y + 23, C_BG_LIGHT_GREY | C_FG_BLACK);  // cs
    print_hex(debug_data.ds,         4, DBG_OFF_X +  6, DBG_OFF_Y + 25, C_BG_LIGHT_GREY | C_FG_BLACK);  // ds
    print_hex(debug_data.es,         4, DBG_OFF_X +  6, DBG_OFF_Y + 27, C_BG_LIGHT_GREY | C_FG_BLACK);  // es
    print_hex(debug_data.fs,         4, DBG_OFF_X +  6, DBG_OFF_Y + 29, C_BG_LIGHT_GREY | C_FG_BLACK);  // fs
    print_hex(debug_data.gs,         4, DBG_OFF_X +  6, DBG_OFF_Y + 31, C_BG_LIGHT_GREY | C_FG_BLACK);  // gs
    print_hex(debug_data.ss,         4, DBG_OFF_X +  6, DBG_OFF_Y + 33, C_BG_LIGHT_GREY | C_FG_BLACK);  // ss
    print_hex(debug_data.eflags,     8, DBG_OFF_X +  9, DBG_OFF_Y + 35, C_BG_LIGHT_GREY | C_FG_BLACK);  // eflags

    print_hex(debug_data.cr0,        8, DBG_OFF_X + 20, DBG_OFF_Y +  5, C_BG_LIGHT_GREY | C_FG_BLACK);  // cr0
    print_hex(debug_data.cr2,        8, DBG_OFF_X + 20, DBG_OFF_Y +  7, C_BG_LIGHT_GREY | C_FG_BLACK);  // cr2
    print_hex(debug_data.cr3,        8, DBG_OFF_X + 20, DBG_OFF_Y +  9, C_BG_LIGHT_GREY | C_FG_BLACK);  // cr3
    print_hex(debug_data.cr4,        8, DBG_OFF_X + 20, DBG_OFF_Y + 11, C_BG_LIGHT_GREY | C_FG_BLACK);  // cr4
    print_hex(debug_data.error_code, 8, DBG_OFF_X + 16, DBG_OFF_Y + 16, C_BG_LIGHT_GREY | C_FG_BLACK);  // error code

    print_hex(debug_data.stack[0],   8, DBG_OFF_X + 16, DBG_OFF_Y + 24, C_BG_LIGHT_GREY | C_FG_BLACK);  // stack0
    print_hex(debug_data.stack[1],   8, DBG_OFF_X + 16, DBG_OFF_Y + 25, C_BG_LIGHT_GREY | C_FG_BLACK);  // stack1
    print_hex(debug_data.stack[2],   8, DBG_OFF_X + 16, DBG_OFF_Y + 26, C_BG_LIGHT_GREY | C_FG_BLACK);  // stack2
    print_hex(debug_data.stack[3],   8, DBG_OFF_X + 16, DBG_OFF_Y + 27, C_BG_LIGHT_GREY | C_FG_BLACK);  // stack3
    print_hex(debug_data.stack[4],   8, DBG_OFF_X + 16, DBG_OFF_Y + 28, C_BG_LIGHT_GREY | C_FG_BLACK);  // stack4

    debug_screen = TRUE;
}

void screen_hide_debug_info() {
    screen_leer_copia();
    debug_screen = FALSE;
}


void screen_pintar_rect(uchar letra, uchar color, int fila, int columna, int alto, int ancho)
{
    int f, c;
    for (f = fila; f < fila + alto; f++)
    {
        for (c = columna; c < columna+ancho; c++)
            screen_pintar(letra, color, f, c);
    }
}

void screen_pintar_linea_h(uchar c, uchar color, int fila, int columna, int ancho)
{
    screen_pintar_rect(c, color, fila, columna, 1, ancho);
}

void screen_pintar_linea_v(uchar c, uchar color, int fila, int columna, int alto)
{
    screen_pintar_rect(c, color, fila, columna, alto, 1);
}
