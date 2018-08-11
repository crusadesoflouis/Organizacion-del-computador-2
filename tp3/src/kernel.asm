; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================


%include "imprimir.mac"
%define GDT_IDX_CODE_0              0x40
%define GDT_IDX_DATA_0              0x50
%define GDT_IDX_VIDEO               0x60
extern GDT_DESC
extern IDT_DESC
extern screen_inicializar
extern idt_inicializar
extern pintar_pantalla
extern mmu_inicializar
extern mmu_inicializar_dir_kernel
extern mmu_prueba
extern resetear_pic
extern habilitar_pic
extern tss_inicializar
extern tss_inicializar_idle
extern game_inicializar
extern sched_initialize
extern mmu_inicializar_dir_pirata
;; Saltear seccion de datos
jmp start

;;
;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
iniciando_mr_msg db     'Iniciando kernel (Modo Real)...'
iniciando_mr_len equ    $ - iniciando_mr_msg

iniciando_mp_msg db     'Iniciando kernel (Modo Protegido)...'
iniciando_mp_len equ    $ - iniciando_mp_msg

;;
;; Seccion de código.
;; -------------------------------------------------------------------------- ;;

;; Punto de entrada del kernel.
BITS 16
start:
    ; Deshabilitar interrupciones
    cli

    ; Cambiar modo de video a 80 X 50
    mov ax, 0003h
    int 10h ; set mode 03h
    xor bx, bx
    mov ax, 1112h
    int 10h ; load 8x8 font

    ; Imprimir mensaje de bienvenida
    imprimir_texto_mr iniciando_mr_msg, iniciando_mr_len, 0x07, 0, 0


    ; Habilitar A20
    call habilitar_A20

    ; Cargar la GDT
    ; call inicializar_gdt
    ; %define offset_address_gdt 2
    lgdt [GDT_DESC]

    ; Setear el bit PE del registro CR0
    xor eax,eax
    mov eax,cr0
    or eax,1
    mov cr0,eax

    ; Saltar a modo protegido
    jmp GDT_IDX_CODE_0:mp
    mp:
    BITS 32
    ; Establecer selectores de segmentosç

    mov ax, GDT_IDX_DATA_0
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov ss, ax
    mov ax, GDT_IDX_VIDEO
    mov fs, ax
    ; Establecer la base de la pila
  ;  mov eax, 0x1f400
    mov ebp, 0x27000
    mov esp, 0x27000
    ; Imprimir mensaje de bienvenida
    imprimir_texto_mp iniciando_mp_msg, iniciando_mp_len, 0x07, 2, 0
    ; Inicializar el juego

    ; Inicializar pantalla
    call screen_inicializar
    call pintar_pantalla
    ; Inicializar el manejador de memoria
    call mmu_inicializar
    ; Inicializar el directorio de paginas
    call mmu_inicializar_dir_kernel
    ; Cargar directorio de paginas
    mov eax,0x27000;
    mov cr3,eax;
    ; Habilitar paginacion
    mov eax,0x80000000;
    mov ecx,cr0;
    or ecx,eax
    mov cr0,ecx
    ;call mmu_prueba
    ; Inicializar tss
    call tss_inicializar
    ; Inicializar tss de la tarea Idle
    call tss_inicializar_idle
    ; Inicializar el scheduler
    call sched_initialize
    ;llamar funcion de prueba
    call game_inicializar
    ; Inicializar la IDT
    call idt_inicializar

    ; Cargar IDT
    lidt [IDT_DESC]
    ; Configurar controlador de interrupciones
    call resetear_pic
    call habilitar_pic
    ; Cargar tarea inicial
    mov ax, 0x68
    ltr ax
    ; Habilitar interrupciones
    sti

    ; Saltar a la primera tarea: Idle
  ;  jmp 0x78:0
    jmp 0x70:0


    ; Ciclar infinitamente (por si algo sale mal...)
    mov eax, 0xFFFF
    mov ebx, 0xFFFF
    mov ecx, 0xFFFF
    mov edx, 0xFFFF
    jmp $
    jmp $

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
