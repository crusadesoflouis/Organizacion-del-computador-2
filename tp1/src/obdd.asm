section .data
true: db '1',0
false: db '0',0
section .text
extern free
extern malloc
extern dictionary_add_entry
extern dictionary_key_for_value
extern obdd_mgr_get_next_node_ID
extern is_constant
extern is_true

%define offset_obbd_node_var_id 0
%define offset_obbd_node_node_id 4
%define offset_obbd_node_ref_count 8
%define offset_obbd_node_high_obdd 12
%define offset_obbd_node_low_obdd 20
;obbd_node_size = 28

%define offset_obbd_mgr_ID 0
%define offset_obbd_mgr_greatest_node_ID 4
%define offset_obbd_mgr_greatest_var_ID 8
%define offset_obbd_mgr_true_obdd 12
%define offset_obbd_mgr_false_obdd 20
%define offset_obbd_mgr_vars_dict 28

; ; typedef struct obdd_node_t{
; ; 	uint32_t 		var_ID;
; ; 	uint32_t		node_ID;
; ; 	uint32_t		ref_count;
; ; 	struct obdd_node_t*	high_obdd;
; ; 	struct obdd_node_t*	low_obdd;
; ; } __attribute__((__packed__)) obdd_node;
;
; ; typedef struct obdd_mgr_t {
; ; 	uint32_t		ID;
; ; 	uint32_t		greatest_node_ID;
; ; 	uint32_t		greatest_var_ID;
; ; 	struct obdd_t*			true_obdd;
; ; 	struct obdd_t*			false_obdd;
; ; 	struct dictionary_t*		vars_dict;
; ; } __attribute__((__packed__)) obdd_mgr;
; %define offset_obbd_node_var_id 0
; %define offset_obbd_node_node_id 4
; %define offset_obbd_node_ref_count 8
; %define offset_obbd_node_high_obdd 12
; %define offset_obbd_node_low_obdd 20
; ;obbd_node_size = 28
;
; %define offset_obbd_mgr_ID 0
; %define offset_obbd_mgr_greatest_node_ID 4
; %define offset_obbd_mgr_greatest_var_ID 8
; %define offset_obbd_mgr_true_obdd 12
; %define offset_obbd_mgr_false_obdd 20
; %define offset_obbd_mgr_vars_dict 28




global obdd_mgr_mk_node
obdd_mgr_mk_node:

;stackframe
  push RBP ;alineada
  MOV RBP,RSP
  push R12 ;desalineada
  push R13 ;alineada
  PUSH R14 ;desa lineada
  PUSH R15 ; alineada
  PUSH RBX ; desalineada
  SUB RSP,8;alineada
;stackframe

  MOV R12,RDI  ; obdd_mgr* mgr
  MOV R13,RSI  ; char* var
  MOV R14,RDX  ; obdd_node* high
  MOV R15,RCX  ; obdd_node* low
  MOV RDI,[R12 + offset_obbd_mgr_vars_dict]
  MOV RSI,R13 ; MGR->vars_dict y  var, para call add entry
  CALL dictionary_add_entry; en EAX tengo el var_ID
  MOV EBX,EAX
  MOV RDI,28 ; 28 es el tamaño del obdd_node
  CALL malloc; en rax tengo el puntero
  MOV [RAX + offset_obbd_node_var_id],EBX
  MOV RBX,RAX; ya me copie el valor que tenia
; en RBX guardado del add entry  y lo puse
; en su posicion, y ahora me guardo en rax asi puedo
; llamar a obdd_mgr_get_next_node_ID
  MOV RDI,R12; rdi tiene el puntero a mgr
  CALL obdd_mgr_get_next_node_ID; en rax tengo el id
  MOV [RBX + offset_obbd_node_node_id],EAX
  MOV [RBX + offset_obbd_node_high_obdd],R14
  MOV [RBX + offset_obbd_node_low_obdd],R15
  CMP R14,0
  JE .low
  INC dword [R14 + offset_obbd_node_ref_count]
.low:
  CMP R15,0
  JE .salir
  INC dword [R15 + offset_obbd_node_ref_count]
.salir:
  MOV dword [RBX + offset_obbd_node_ref_count],0
  MOV RAX,RBX
;stackframe
  ADD RSP,8
  POP RBX
  POP R15
  POP R14
  POP R13
  POP R12
  POP RBP
ret




%define offset_obbd_node_var_id 0
%define offset_obbd_node_node_id 4
%define offset_obbd_node_ref_count 8
%define offset_obbd_node_high_obdd 12
%define offset_obbd_node_low_obdd 20
;obbd_node_size = 28

; RDI = node *mgr
; RDI con puntero a el nodo
global obdd_node_destroy
obdd_node_destroy:
;stackframe
  push RBP
  MOV RBP,RSP
  PUSH R12
  PUSH R13
;stackframe
  ; MOV R12,0
  ; MOV byte [R12],0
  MOV R12,RDI
;primero me guardo en R12 el puntero al nodo asi lo preservo
  LEA RSI,[R12 + offset_obbd_node_ref_count]
  MOV ESI,[RSI]
  CMP ESI,0
;Me cargo la direccion del puntero del nodo en RSI + ref count en RSI y pregunto, si es cero, salgo
  JNE .Definitivo
;si no es cero sigo ejecutando, y pregunto si el nodo HIGH es NULL
  LEA RSI,[R12 + offset_obbd_node_high_obdd]
  MOV RCX,[RSI]
;cargo en RCX el contenido del puntero a high y pregunto si es NULL, si es NULL salto a .Low sino
;hago la llamada recursiva y elimino el nodo
  CMP RCX,0
  JE .NodoLow
;decremento la posicion de memoria donde cuenta la cantidad de padres
  DEC dword [RCX + offset_obbd_node_ref_count]
;pongo en NULL la direccion high del nodo que trabajamos
  MOV qword [R12 + offset_obbd_node_high_obdd],0
;ponemos el puntero a borrar en RDI y llamos a la recursion
  MOV RDI,RCX
  CALL obdd_node_destroy
.NodoLow:
  LEA RSI,[R12 + offset_obbd_node_low_obdd]
  MOV RCX,[RSI]
  CMP RCX,0
  JE .fin
  DEC dword [RCX + offset_obbd_node_ref_count]
  MOV qword [R12 + offset_obbd_node_low_obdd],0
  MOV RDI,RCX
  CALL obdd_node_destroy
.fin:
  LEA RSI,[R12 + offset_obbd_node_var_id]
  MOV dword [RSI],0
  LEA RSI,[R12 + offset_obbd_node_node_id]
  MOV dword [RSI],0
  MOV RDI,R12
  CALL free
.Definitivo:
;stackframe
  POP R13
  POP R12
  POP RBP

ret
%define offset_obbd_mgr 0
%define offset_obbd_root 8
;obbd_size = 16
; RDI = obdd_mgr *mgr
; RSI obdd_node *root

global obdd_create
obdd_create:

;stackframe
  push RBP
  MOV RBP,RSP
  PUSH R12
  PUSH R13
;stackframe
  MOV R12,RDI
  MOV R13,RSI
  MOV RDI,16
  call malloc
;en RAX tengo el puntero
  MOV [RAX + offset_obbd_mgr],R12
;aca no va cero
  MOV [RAX + offset_obbd_root],R13
;stackframe
  POP R13
  POP R12
  POP RBP
ret

global obdd_destroy
obdd_destroy:
;stackframe
  push RBP ;alineada
  MOV RBP,RSP
  push R12 ;desalineada
  push R13 ;alineada
  PUSH R14 ;desa lineada
  SUB RSP,8;alineada
;stackframe
  CMP RDI,0
  JE .Termine
  MOV R14,RDI
  LEA R12,[RDI + offset_obbd_mgr]
  LEA R13,[RDI + offset_obbd_root]
  ;obdd_root no seteado
  ;R12 puntero a mgr
  ;R13 puntero a root
  MOV R9,[R13]
  CMP R9,0
;R9 tiene la direccion de root->root_obdd
  JE .Salir
  MOV RDI,R9
  call obdd_node_destroy
  .Salir:
  MOV qword [R13],0
  MOV qword [R12],0
  MOV RDI,R14
  call free
.Termine:
;stackframe
  ADD RSP,8
  POP R14
  POP R13
  POP R12
  POP RBP
ret




; %define offset_obbd_aply_local_left_var_ID 0
; ;ocupa 4 bytes
; %define offset_obbd_aply_local_right_var_ID 4
; ;ocupa 4 bytes
%define offset_obbd_aply_local_left_var 0
;ocupa 8 bytes, es un puntero a char
%define offset_obbd_aply_local_right_var 8
;8 bytes otro puntero
%define offset_obbd_aply_local_is_left_constant 16
;4 bytes
%define offset_obbd_aply_local_is_right_constant 20
;4bytes size total 24 bytes




; RDI *funcion
; RSI obdd_mgr* mgr
; RDX obdd_node* left_node
; RCX obdd_node* right_node


global obdd_node_apply
obdd_node_apply:
;stackframe
  push RBP ;alineada
  MOV RBP,RSP
  push R12 ;desalineada
  push R13 ;alineada
  PUSH R14 ;desalineada
  PUSH R15 ;alineada
  PUSH RBX ;desalineada
  SUB RSP,24 ;alineada

;stackframe
  MOV R12,RDI ;FUNCION
  MOV R13,RSI ;obdd_mgr*
  MOV R14,RDX ;left_node
  MOV R15,RCX ;right_node
;busco key para left_var_ID
; Y estooooo
  MOV RDI,[R13 + offset_obbd_mgr_vars_dict]
  MOV ESI,[R14 + offset_obbd_node_var_id]
  CALL dictionary_key_for_value
  MOV [RSP + offset_obbd_aply_local_left_var],RAX
;hasta aca debugueado y esta ok
;busco key para right_var_ID
  MOV RDI,[R13 + offset_obbd_mgr_vars_dict]
  MOV ESI,[R15 + offset_obbd_node_var_id]
  CALL dictionary_key_for_value
  MOV [RSP + offset_obbd_aply_local_right_var],RAX
  ;hasta acá no testeado pero deberia ser lo mismo
  ; ver algo raro con ceros en el medio en la pila
  ; MOV RAX,[RSP + offset_obbd_aply_local_left_var]
  ; MOV RBX,[RSP + offset_obbd_aply_local_right_var]
  ;parece que anda bien por ahora esto
  MOV RDI,R13
  MOV RSI,R14
  CALL is_constant ;pregunto si el nodo left es constante
  MOV [RSP + offset_obbd_aply_local_is_left_constant],EAX
  MOV RDI,R13
  MOV RSI,R15
  CALL is_constant ;pregunto si el nodo left es constante
  MOV [RSP + offset_obbd_aply_local_is_right_constant],EAX
  ;pareciera que hasta aca esta todo bien
  XOR RAX,RAX
  MOV EAX,[RSP + offset_obbd_aply_local_is_left_constant]
  CMP EAX,0
  JE .Caso1
  MOV EAX,[RSP + offset_obbd_aply_local_is_right_constant]
  CMP EAX,0
  JE .Caso1; si es igual a cero, entonces me quedo en el if
  ;aca tengo otros un if con la llamada rara
  MOV RDI,R13
  MOV RSI,R14 ; IS TRUE LEFT NODE?
  CALL is_true
  MOV EBX,EAX
  MOV RDI,R13
  MOV RSI,R15  ; IS TRUE RIGHT NODE?
  CALL is_true
  MOV EDI,EBX; EDI tiene la respuesta si left
  MOV ESI,EAX ;ESI la de right
  CALL R12; hago la llamada rara en rax tengo la respuesta
  CMP EAX,0
  JE .else ; ver esto
  MOV RDI,R13
  MOV RSI,true
  MOV RDX,0
  MOV RCX,0
  CALL obdd_mgr_mk_node
  JMP .exit
.else:
  MOV RDI,R13
  MOV RSI,false
  MOV RDX,0
  MOV RCX,0
  CALL obdd_mgr_mk_node
  JMP .exit
.Caso1:
  MOV EAX,[RSP + offset_obbd_aply_local_is_left_constant]
  CMP EAX,0; is left_constant?
  JE .Caso2
  MOV RDI,R12
  MOV RSI,R13
  MOV RDX,R14
  MOV RCX,[R15 + offset_obbd_node_high_obdd]
  CALL obdd_node_apply
  MOV RBX,RAX
  MOV RDI,R12
  MOV RSI,R13
  MOV RDX,R14
  MOV RCX,[R15 + offset_obbd_node_low_obdd]
  CALL obdd_node_apply
  MOV RDI,R13
  MOV RSI,[RSP + offset_obbd_aply_local_right_var]
  MOV RDX,RBX
  MOV RCX,RAX
  CALL obdd_mgr_mk_node
  JMP .exit
.Caso2:
  MOV EAX,[RSP + offset_obbd_aply_local_is_right_constant]
  CMP EAX,0
  JE .Caso3
  MOV RDI,R12
  MOV RSI,R13
  MOV RDX,[R14 + offset_obbd_node_high_obdd]
  MOV RCX,R15
  CALL obdd_node_apply
  MOV RBX,RAX
  MOV RDI,R12
  MOV RSI,R13
  MOV RDX,[R14 + offset_obbd_node_low_obdd]
  MOV RCX,R15
  CALL obdd_node_apply
  MOV RDI,R13
  MOV RSI,[RSP + offset_obbd_aply_local_left_var]
  MOV RDX,RBX
  MOV RCX,RAX
  CALL obdd_mgr_mk_node
  JMP .exit
.Caso3:
  MOV R9D,[R14 + offset_obbd_node_var_id]
  MOV R10D,[R15 + offset_obbd_node_var_id]
  CMP R9D,R10D
  JNE .Caso4
  MOV RDI,R12
  MOV RSI,R13
  MOV RDX,[R14 + offset_obbd_node_high_obdd]
  MOV RCX,[R15 + offset_obbd_node_high_obdd]
  CALL obdd_node_apply
  MOV RBX,RAX
  MOV RDI,R12
  MOV RSI,R13
  MOV RDX,[R14 + offset_obbd_node_low_obdd]
  MOV RCX,[R15 + offset_obbd_node_low_obdd]
  CALL obdd_node_apply
  MOV RDI,R13
  MOV RSI,[RSP + offset_obbd_aply_local_left_var]
  MOV RDX,RBX
  MOV RCX,RAX
  CALL obdd_mgr_mk_node
  JMP .exit
.Caso4:
  JNL .Caso5
  MOV RDI,R12
  MOV RSI,R13
  MOV RDX,[R14 + offset_obbd_node_high_obdd]
  MOV RCX,R15
  CALL obdd_node_apply
  MOV RBX,RAX
  MOV RDI,R12
  MOV RSI,R13
  MOV RDX,[R14 + offset_obbd_node_low_obdd]
  MOV RCX,R15
  CALL obdd_node_apply
  MOV RDI,R13
  MOV RSI,[RSP + offset_obbd_aply_local_left_var]
  MOV RDX,RBX
  MOV RCX,RAX
  CALL obdd_mgr_mk_node
  JMP .exit
.Caso5:
  MOV RDI,R12
  MOV RSI,R13
  MOV RDX,R14
  MOV RCX,[R15 + offset_obbd_node_high_obdd]
  CALL obdd_node_apply
  MOV RBX,RAX
  MOV RDI,R12
  MOV RSI,R13
  MOV RDX,R14
  MOV RCX,[R15 + offset_obbd_node_low_obdd]
  CALL obdd_node_apply
  MOV RDI,R13
  MOV RSI,[RSP + offset_obbd_aply_local_right_var]
  MOV RDX,RBX
  MOV RCX,RAX
  CALL obdd_mgr_mk_node
.exit:
;stackframe
  ADD RSP,24
  POP RBX
  POP R15
  POP R14
  POP R13
  POP R12
  POP RBP

ret








global is_tautology
is_tautology:
;stackframe
  push RBP ;alineada
  MOV RBP,RSP
  push R12 ;desalineada
  push R13 ;alineada
  PUSH R14 ;desalineada
  PUSH RBX ;aliana
;stackframe
  MOV R12,RDI
  MOV R13,RSI
;obdd_mgr en R12
;obdd_node en R13
  call is_constant
  MOV RBX,RAX
  CMP AL,0
;en RAX tengo el valor de true o false si es constante o no
  JNE .EsTrue?
;Si no es true, tengo que hacer la llamada recursiva
  MOV RDI,R12
;en RDI pongo mgr
;Pongo en RSI la direccion de memoria a la que apunta R13 + high obdd, osea node->high-obdd
  MOV RSI,[R13 + offset_obbd_node_high_obdd]
  CALL is_tautology
;en RAX tengo el resultado de is_tautology node high
  CMP AL,0
  JE .Salir
;si es 0, osea que no es tautologia, salgo y dejo el valor en RAX que indica fals
;sino, sigo y pregunto por la derecha
  MOV RDI,R12
  MOV RSI,[R13 + offset_obbd_node_low_obdd]
  CALL is_tautology
;en RAX tengo el valor de si el nodo de la derecha es una tautologia o no
;si es tautologia o no, queda en el registro RAX y jumpeo a salir y devuelvo
;el valor que sea segun tautologia en RAX
  JMP .Salir
.EsTrue?:
  MOV RDI,R12
  MOV RSI,R13
  call is_true
  JMP .Salir
;en RAX tengo si es true o no y salgo directamente
.Salir:
;stackframe
  POP RBX
  POP R14
  POP R13
  POP R12
  POP RBP
ret

global is_sat
is_sat:
;stackframe
  push RBP ;alineada
  MOV RBP,RSP
  push R12 ;desalineada
  push R13 ;alineada
  PUSH RBX ;aliana
  PUSH R14 ;desalineada
;stackframe
  MOV R12,RDI
  MOV R13,RSI
;RDI puntero a obdd_mgr mgr
;RSI puntero a obdd_node root
  call is_constant
  MOV RBX,RAX
  CMP AL,0
  JNE .EsTrue?
;si es verdadero que es constanto llamo a .EsTrue?, sino, tengo que mirar los hijos
;como no es verdadero, muevo a RDI el puntero al mgr y a RSI el puntero a el nodo high obbd
  MOV RDI,R12
  MOV RSI,[R13 + offset_obbd_node_high_obdd]
;hago la llamada recursiva
  CALL is_sat
  CMP AL,0
  JNE .Salir
;si no es 1, osea que no es verdadera, pregunto al otro hijo
MOV RDI,R12
MOV RSI,[R13 + offset_obbd_node_low_obdd]
CALL is_sat
JMP .Salir
  .EsTrue?:
;pregunto si el nodo que es constante es true o false, si es true, o falso y devuelvo
;el valor en RAX, pero como el valor ya vino en RAX no tenemos que moverlo.
  MOV RDI,R12
  MOV RSI,R13
  call is_true
  JMP .Salir
.Salir:
;stackframe
  POP RBX
  POP R14
  POP R13
  POP R12
  POP RBP

ret




global str_len
str_len:
;RDI = *char
;stackframe
  push RBP
  MOV RBP, RSP
;
  XOR RAX,RAX
.ciclo:
  INC RAX
  INC RDI
  MOV CL,[RDI]
  CMP CL,0
  JNE .ciclo
;stackframe
  POP RBP
ret

global str_copy
str_copy:
;RDI = *char
;stackframe
  push RBP
  push R12
  PUSH R13
  MOV RBP,RSP
;stackframe
  MOV R12,RDI
;me guardo el puntero en R12
  call str_len
;en RAX tengo el tamaño del string
  MOV RDI,RAX
  MOV R13,RAX
  INC R13
  INC RDI
;muevo a RDI para llamar a malloc y pedir la memoria que necesito
;y me de el puntero a esta memoria en RAX y en R13 me guardo el tamaño
  call malloc
  MOV R9,RAX
;guardo en R9 la posicion de rax asi la incremento mientras voy copiando
.ciclo:
  MOV  SIL,[R12]
  MOV  [R9],SIL
  INC R12
  INC R9
  DEC R13
  CMP R13,0
  JNE .ciclo
;stackframe
  POP R13
  POP R12
  POP RBP
ret

global str_cmp
str_cmp:
;RDI = *char A
;RSI = *char B
;stackframe
  PUSH RBP
  MOV RBP, RSP

;stackframe
  .ciclo:
  MOV DL, [RDI]
  MOV CL, [RSI]
  CMP DL,0
  JE .igual
  CMP DL, CL
  JNE .seguir
  INC RDI
  INC RSI
  JMP .ciclo
  .igual:
  CMP CL,0
  JNE .devuelveNegativo
  MOV RAX,0
  jmp .terminar
  .seguir:
  CMP DL, CL
  JL .devuelvePositivo
  JMP .devuelveNegativo
  .devuelvePositivo:
  MOV RAX, 1
  JMP .terminar
  .devuelveNegativo:
  MOV RAX, -1
  .terminar:
;stackframe
  POP RBP
ret
