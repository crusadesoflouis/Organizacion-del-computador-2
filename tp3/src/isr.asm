; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones

%include "imprimir.mac"
extern screen_inicializar
extern manejo_exepciones_procesador
BITS 32


sched_tarea_offset:     dd 0x00
sched_tarea_selector:   dw 0x00

;; PIC
extern fin_intr_pic1

;;mostrar animacion
extern game_tick
extern game_atender_teclado
;; Sched
extern sched_tick
extern sched_tarea_actual
extern sched_proxima_a_ejecutar
extern syscall_handler_juego

extern screen_show_debug_info
;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;


;;
;; Estructura y variables de modo debug
;; -------------------------------------------------------------------------- ;;
extern debug_mode
extern debug_data

struc ddt
    .eax        resd 1
    .ebx        resd 1
    .ecx        resd 1
    .edx        resd 1
    .esi        resd 1
    .edi        resd 1
    .ebp        resd 1
    .esp        resd 1
    .eip        resd 1
    .cs         resw 1
    .ds         resw 1
    .es         resw 1
    .fs         resw 1
    .gs         resw 1
    .ss         resw 1
    .eflags     resd 1
    .cr0        resd 1
    .cr2        resd 1
    .cr3        resd 1
    .cr4        resd 1
    .error_code resd 1
    .stack      resd 5
endstruc





%macro ISR 1
global _isr%1

_isr%1:
	; Salvamos eax
   ; xchg bx,bx
    mov       [debug_data + ddt.eax], eax

    ; Vemos si el modo debug esta activo
    mov       eax, [debug_mode]
    cmp       eax, 0
    je        .chau_tarea

     ; Guardamos todos los registros para mostrarlos en pantalla

        ; Registros de proposito general
        mov       [debug_data + ddt.ebx], ebx
        mov       [debug_data + ddt.ecx], ecx
        mov       [debug_data + ddt.edx], edx
        mov       [debug_data + ddt.esi], esi
        mov       [debug_data + ddt.edi], edi
        mov       [debug_data + ddt.ebp], ebp
        mov       [debug_data + ddt.ds ], ds
        mov       [debug_data + ddt.es ], es
        mov       [debug_data + ddt.fs ], fs
        mov       [debug_data + ddt.gs ], gs

        ; Registros de control
        mov       eax, cr0
        mov       [debug_data + ddt.cr0], eax
        mov       eax, cr2
        mov       [debug_data + ddt.cr2], eax
        mov       eax, cr3
        mov       [debug_data + ddt.cr3], eax
        mov       eax, cr4
        mov       [debug_data + ddt.cr4], eax

        ; Registros en stack
        mov       eax, [esp]
        mov       [debug_data + ddt.error_code], eax
        mov       eax, [esp + 4]
        mov       [debug_data + ddt.eip], eax
        mov       ax, [esp + 8]
        mov       [debug_data + ddt.cs], ax
        mov       eax, [esp + 12]
        mov       [debug_data + ddt.eflags], eax
        mov       ecx, [esp + 16]
        mov       [debug_data + ddt.esp], ecx
        mov       ax, [esp + 20]
        mov       [debug_data + ddt.ss], ax

        ; Stack
        mov       eax, [ecx]
        mov       [debug_data + ddt.stack], eax
        mov       eax, [ecx + 4]
        mov       [debug_data + ddt.stack + 4], eax
        mov       eax, [ecx + 8]
        mov       [debug_data + ddt.stack + 8], eax
        mov       eax, [ecx + 12]
        mov       [debug_data + ddt.stack + 12], eax
        mov       eax, [ecx + 16]
        mov       [debug_data + ddt.stack + 16], eax
    ; Eliminamos la tarea que causo la excepcion
	.chau_tarea:
    	mov eax, %1
    	push eax
      ;xchg bx,bx
    	call manejo_exepciones_procesador
    	pop eax
    ; Vemos (de nuevo) si el modo debug esta activo
   		mov       eax, [debug_mode]
   		cmp       eax, 0
    	je        .fin
    	; Mostramos en pantalla la info de debug
        call screen_show_debug_info
        .fin:
    	jmp 0x70:0
    ; jmp $
%endmacro



;;
;; Datos
;; -------------------------------------------------------------------------- ;;
; Scheduler
global saltar_idle
saltar_idle:
    jmp 0x70:0


;;
;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR 8
ISR 9
ISR 10
ISR 11
ISR 12
ISR 13
ISR 14
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19
;la interrupcion 99 hay que eliminarla depues
global _isr32
	_isr32:
	pushad
  call fin_intr_pic1
  call sched_proxima_a_ejecutar
  push eax
  call game_tick
  ; xchg bx, bx
  pop eax
    str ebx
    cmp bx, ax
    je .fin
    mov [sched_tarea_selector], ax
    jmp far [sched_tarea_offset]
  .fin:
	popad
	iret
;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
%define key_debug_break       0x95 ; Y
%define key_a_sh_break        0xaa ; LShift
%define key_b_sh_break        0xb6 ; RShift


global _isr33

	_isr33:
	pushad
	;xchg bx,bx
	xor eax,eax
	in al,0x60
	push eax
	cmp al,key_debug_break
	je imprime_letra
	; cmp al,0x1e
	; je cambiar_tarea
	cmp al,key_a_sh_break
	je imprime_letra
	cmp al,key_b_sh_break
	je imprime_letra
	jmp fin
	imprime_letra:
	call game_atender_teclado
	fin:
	call fin_intr_pic1
	pop eax
	popad
	iret
;
; cambiar_tarea:
;   mov ax,0x7B
;   mov [sched_tarea_selector],ax
; ;  xchg bx,bx
;   jmp far [sched_tarea_offset]

;;
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;
global _isr70

	_isr70:
	pushad
  push eax
  push ecx
  call syscall_handler_juego
  pop ecx
  mov edx, eax   ; edx tiene el resultado de la syscall
  pop eax        ; eax tiene la syscall seleccionada
  cmp eax, 3
  jne .fin
  mov [esp + 28], edx  ; escribimos en la pila el eax
.fin
	popad
  jmp 0x70:0
	iret
