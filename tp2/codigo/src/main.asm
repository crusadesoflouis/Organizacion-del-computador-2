; Organización del computador 2 - FCEyN, UBA
; Trabajo práctico 2
; Juan Carlos Humpiri - LU 842/14 - jhumpiri1599@gmail.com
; Luis Alberto Arroyo - LU 913/13 - luis.arroyo.90@gmail.com
; Luis Enrique García Gómez - LU 675/13 - garcia_luis_94@hotmail.com
; TUTOR: MARIANO CERRUTTI

%define OFFSET_FLUID_SOLVER_N 0
%define OFFSET_FLUID_SOLVER_U 16
%define OFFSET_FLUID_SOLVER_V 24

%define MULTIPLICATION_FACTOR_POSITIVE 0x3f000000
%define MULTIPLICATION_FACTOR_NEGATIVE 0xbf000000

%define FLIP_SIGN_SINGLE_FLOAT_MASK 0x80000000

%define SOLVER_PROJECT_ONE_AS_FLOAT 0x3f800000
%define SOLVER_PROJECT_FOUR_AS_FLOAT 0x40800000

;offsets
%define offset_fluid_solver_N 0
%define offset_fluid_solver_dt 4
%define offset_fluid_solver_diff 8
%define offset_fluid_solver_visc 12
%define offset_fluid_solver_u 16
%define offset_fluid_solver_v 24
%define offset_fluid_solver_u_prev 32
%define offset_fluid_solver_v_prev 40
%define offset_fluid_solver_dens 48
%define offset_fluid_solver_dens_prev 56

section .rodata
ZeroFiveMask: dq 0.5, 0.5, 0.5, 0.5 ;mascara para la constante en solver_project
limpia_Arriba: dq 0xffffffff00000000
limpia_Abajo:  dq 0x00000000ffffffff

section .text
global solver_project
; rdi = fluid_solver* solver
; rsi = float* p
; rdx = float* div
solver_project:
    ; stack frame ========================
    push rbp                ; alineada
    mov rbp, rsp
    push rbx                ; desalineada
    push r12                ; alineada
    push r13                ; desalineada
    push r14                ; alineada
    push r15                ; desalineada
    sub rsp, 8              ; alineada

    mov rbx, rdi                                ; rbx = solver
    mov r12, rsi                                ; r12 = p
    mov r13, rdx                                ; r13 = div
    mov r14, [ rbx + OFFSET_FLUID_SOLVER_V ]    ; r14 = solver->v
    mov r15, [ rbx + OFFSET_FLUID_SOLVER_U ]    ; r15 = solver->u

    ; inicialización para primer loop ==================================

    xor r8, r8

    mov eax, MULTIPLICATION_FACTOR_NEGATIVE             ; eax = -MF
    movd xmm0, eax                                      ; xmm0 = 0 | 0 | 0 | -MF
    shufps xmm0, xmm0, 0x00                             ; xmm0 = -MF | -MF | -MF | -MF

    pshufd xmm1, [ rbx + OFFSET_FLUID_SOLVER_N ], 0x00  ; xmm1 = N | N | N | N

    movd r8d, xmm1                                      ; r8 = N

    cvtdq2ps xmm1, xmm1                                 ; xmm1 = N | N | N | N
    divps xmm0, xmm1                                    ; xmm0 = -MF/N | -MF/N | -MF/N | -MF/N

    mov r9, r8                                          ; r9 = N
    sub r9, 3                                           ; r9 = N - 3
    mov r10, r8                                         ; r10 = N
    add r10, 2                                          ; r10 = N + 2

    mov rdi, 1                                          ; rdi = i = 1
    mov rsi, 0                                          ; rsi = j = 0

    ; estado de registros (64 bits)
    ; r8    = N                         xmm0 = -MF/N | -MF/N | -MF/N | -MF/N
    ; r9    = N - 3
    ; r10   = N + 2
    ; r11   = LIBRE
    ; rax   = LIBRE
    ; rcx   = LIBRE
    ; rdx   = LIBRE
    ; rdi   = i
    ; rsi   = j
    ; rbx   = solver
    ; r12   = p
    ; r13   = div
    ; r14   = solver->v
    ; r15   = solver->u

    .solver_project_first_loop_i:
        cmp rdi, r9                             ; ¿ i == N - 3 ?
        jg .solver_project_first_loop_i_exit
        .solver_project_first_loop_j:
            cmp rsi, r8                         ; ¿ j == N ?
            jge .solver_project_first_loop_j_exit

            ; calculo de posición (i, j) (actual)
            mov rax, r8                         ; rax = N
            mul esi                             ; edx:eax = N * j
            shl rdx, 32
            mov eax, eax
            add rax, rdx                        ; rax = N * j
            mov rdx, rsi                        ; rdx = j
            shl rdx, 1                          ; rdx = 2 * j
            add rax, rdx                        ; rax = N * j + 2 * j
            add rax, rdi                        ; rax = i + N * j + 2 * j = (i, j)

            ; lectura de posición v[i, j]
            movdqu xmm1, [ r14 + rax * 4 ]      ; xmm1 = a3 | a2 | a1 | a0 = v[i, j]

            ; calculo de posición (i-1, j+1)
            add rax, r10                        ; rax = i + N * j + 2 * j + N + 2
            sub rax, 1                          ; rax = i + N * j + 2 * j + N + 1 = (i-1, j+1)

            ; lectura de posición u[i-1, j+1]
            movdqu xmm2, [ r15 + rax * 4 ]      ; xmm2 = b3 | b2 | b1 | b0 = u[i-1, j+1]

            ; calculo de posición (i+1, j+1)
            add rax, 2                          ; rax = i + N * j + 2 * j + N + 3 = (i+1, j+1)

            ; lectura de posición u[i+1, j+1]
            movdqu xmm3, [ r15 + rax * 4 ]      ; xmm3 = c3 | c2 | c1 | c0 = u[i+1, j+1]

            ; calculo de posición (i, j+2)
            sub rax, 1                          ; rax = i + N * j + 2 * j + N + 2
            add rax, r10                        ; rax = i + N * j + 2 * j + 2 * N + 4 = (i, j+2)

            ; lectura de posición v[i, j+2]
            movdqu xmm4, [ r14 + rax * 4 ]       ; xmm4 = d3 | d2 | d1 | d0 = v[i, j+2]

            ; calculo de elementos sobre u
            movdqu xmm5, xmm2                   ; xmm5 = b3 | b2 | b1 | b0
            psrldq xmm2, 8                      ; xmm2 = b1 | b0 | 0 | 0
            subps xmm2, xmm5                    ; xmm2 = b1 - b3 | b0 - b2 | 0 - b1 | 0 - b0

            movdqu xmm5, xmm3                   ; xmm5 = c3 | c2 | c1 | c0
            psrldq xmm3, 8                      ; xmm3 = c1 | c0 | 0 | 0
            subps xmm3, xmm5                    ; xmm3 = c1 - c3 | c0 - c2 | 0 - c1 | 0 - c0

            shufps xmm2, xmm3, 0x44             ; xmm2 = b1 - b3 | b0 - b2 | c1 - c3 | c0 - c2

            ; calculo de elementos sobre v
            subps xmm4, xmm1                    ; xmm4 = d3 - a3 | d2 - a2 | d1 - a1 | d0 - a0

            ; calculo sobre los elementos calculados previamente de u y v
            addps xmm2, xmm4                    ; xmm2 = b1 - b3 + d3 - a3 | b0 - b2 + d2 - a2 | c1 - c3 + d1 - a1 | c0 - c2 + d0 - a0
            mulps xmm2, xmm0                    ; xmm2 = -MF/N*(b1 - b3 + d3 - a3) | -MF/N*(b0 - b2 + d2 - a2) | -MF/N*(c1 - c3 + d1 - a1) | -MF/N*(c0 - c2 + d0 - a0)

            ; calculo de posición (i, j+1)
            sub rax, r10                        ; rax = i + N * j + 2 * j + N + 2

            ; guardado de datos calculados sobre div[i, j+1]
            movdqu [ r13 + rax * 4 ], xmm2

            ; guardado de dato nulo sobre p[i, j+1]
            pxor xmm2, xmm2
            movdqu [ r12 + rax * 4 ], xmm2

            add rsi, 1                          ; j = j + 1
        jmp .solver_project_first_loop_j
        .solver_project_first_loop_j_exit:
            xor rsi, rsi                        ; j = 0
            add rdi, 4                          ; i = i + 4
    jmp .solver_project_first_loop_i
    .solver_project_first_loop_i_exit:

    mov rdi, rbx
    xor rsi, rsi
    mov rdx, r13
    call solver_set_bnd

    mov rdi, rbx
    xor rsi, rsi
    mov rdx, r12
    call solver_set_bnd

    mov rdi, rbx
    xor rsi, rsi
    mov rdx, r12
    mov rcx, r13
    pxor xmm0, xmm0
    mov eax, SOLVER_PROJECT_ONE_AS_FLOAT
    movd xmm0, eax
    pxor xmm1, xmm1
    mov eax, SOLVER_PROJECT_FOUR_AS_FLOAT
    movd xmm1, eax
    call solver_lin_solve
    ; inicialización para segundo loop ==================================

    xor r8, r8

    mov eax, MULTIPLICATION_FACTOR_POSITIVE             ; eax = MF
    movd xmm0, eax                                      ; xmm0 = 0 | 0 | 0 | MF
    shufps xmm0, xmm0, 0x00                             ; xmm0 = MF | MF | MF | MF

    pshufd xmm1, [ rbx + OFFSET_FLUID_SOLVER_N ], 0x00  ; xmm1 = N | N | N | N

    movd r8d, xmm1                                      ; r8 = N

    cvtdq2ps xmm1, xmm1                                 ; xmm1 = N | N | N | N
    mulps xmm0, xmm1                                    ; xmm0 = MF*N | MF*N | MF*N | MF*N

    mov r9, r8                                          ; r9 = N
    sub r9, 3                                           ; r9 = N - 3
    mov r10, r8                                         ; r10 = N
    add r10, 2                                          ; r10 = N + 2

    mov rdi, 1                                          ; rdi = i = 1
    mov rsi, 0                                          ; rsi = j = 0

    ; estado de registros (64 bits)
    ; r8    = N                         xmm0 = MF*N | MF*N | MF*N | MF*N
    ; r9    = N - 3
    ; r10   = N + 2
    ; r11   = LIBRE
    ; rax   = LIBRE
    ; rcx   = LIBRE
    ; rdx   = LIBRE
    ; rdi   = i
    ; rsi   = j
    ; rbx   = solver
    ; r12   = p
    ; r13   = div
    ; r14   = solver->v
    ; r15   = solver->u

    .solver_project_second_loop_i:
        cmp rdi, r9                             ; ¿ i == N - 3 ?
        jg .solver_project_second_loop_i_exit
        .solver_project_second_loop_j:
            cmp rsi, r8                         ; ¿ j == N ?
            jge .solver_project_second_loop_j_exit

            ; calculo de posición (i, j) (actual)
            mov rax, r8                         ; rax = N
            mul esi                             ; edx:eax = N * j
            shl rdx, 32
            mov eax, eax
            add rax, rdx                        ; rax = N * j
            mov rdx, rsi                        ; rdx = j
            shl rdx, 1                          ; rdx = 2 * j
            add rax, rdx                        ; rax = N * j + 2 * j
            add rax, rdi                        ; rax = i + N * j + 2 * j = (i, j)

            ; lectura de posición p[i, j]
            movdqu xmm1, [ r12 + rax * 4 ]      ; xmm1 = a3 | a2 | a1 | a0 = p[i, j]

            ; calculo de posición (i-1, j+1)
            add rax, r10                        ; rax = i + N * j + 2 * j + N + 2
            sub rax, 1                          ; rax = i + N * j + 2 * j + N + 1 = (i-1, j+1)

            ; lectura de posición p[i-1, j+1]
            movdqu xmm2, [ r12 + rax * 4 ]      ; xmm2 = b3 | b2 | b1 | b0 = p[i-1, j+1]

            ; calculo de posición (i+1, j+1)
            add rax, 2                          ; rax = i + N * j + 2 * j + N + 3 = (i+1, j+1)

            ; lectura de posición p[i+1, j+1]
            movdqu xmm3, [ r12 + rax * 4 ]      ; xmm3 = c3 | c2 | c1 | c0 = p[i+1, j+1]

            ; calculo de posición (i, j+2)
            sub rax, 1                          ; rax = i + N * j + 2 * j + N + 2
            add rax, r10                        ; rax = i + N * j + 2 * j + 2 * N + 4 = (i, j+2)

            ; lectura de posición p[i, j+2]
            movdqu xmm4, [ r12 + rax * 4 ]       ; xmm4 = d3 | d2 | d1 | d0 = p[i, j+2]

            ; calculo de elementos sobre p (horizontal)
            movdqu xmm5, xmm2                   ; xmm5 = b3 | b2 | b1 | b0
            psrldq xmm2, 8                      ; xmm2 = b1 | b0 | 0 | 0
            subps xmm2, xmm5                    ; xmm2 = b1 - b3 | b0 - b2 | 0 - b1 | 0 - b0

            movdqu xmm5, xmm3                   ; xmm5 = c3 | c2 | c1 | c0
            psrldq xmm3, 8                      ; xmm3 = c1 | c0 | 0 | 0
            subps xmm3, xmm5                    ; xmm3 = c1 - c3 | c0 - c2 | 0 - c1 | 0 - c0

            shufps xmm2, xmm3, 0x44             ; xmm2 = b1 - b3 | b0 - b2 | c1 - c3 | c0 - c2
            mulps xmm2, xmm0                    ; xmm2 = MF*N*(b1 - b3) | MF*N*(b0 - b2) | MF*N*(c1 - c3) | MF*N*(c0 - c2)

            ; calculo de elementos sobre p (vertical)
            subps xmm4, xmm1                    ; xmm4 = d3 - a3 | d2 - a2 | d1 - a1 | d0 - a0
            mulps xmm4, xmm0                    ; xmm4 = MF*N*(d3 - a3) | MF*N*(d2 - a2) | MF*N*(d1 - a1) | MF*N*(d0 - a0)

            ; calculo de posición (i, j+1)
            sub rax, r10                        ; rax = i + N * j + 2 * j + N + 2

            ; u[i, j+1]
            movdqu xmm3, [ r15 + rax * 4 ]
            subps xmm3, xmm2
            movdqu [ r15 + rax * 4 ], xmm3

            ; v[i, j+1]
            movdqu xmm3, [ r14 + rax * 4 ]
            subps xmm3, xmm4
            movdqu [ r14 + rax * 4 ], xmm3

            add rsi, 1                          ; j = j + 1
        jmp .solver_project_second_loop_j
        .solver_project_second_loop_j_exit:
            xor rsi, rsi                        ; j = 0
            add rdi, 4                          ; i = i + 4
    jmp .solver_project_second_loop_i
    .solver_project_second_loop_i_exit:

    mov rdi, rbx
    mov rsi, 1
    mov rdx, r15
    call solver_set_bnd

    mov rdi, rbx
    mov rsi, 2
    mov rdx, r14
    call solver_set_bnd

    add rsp, 8
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
ret

global solver_set_bnd:
; rdi = fluid_solver* solver
; esi = uint32_t b
; rdx = float* x
solver_set_bnd:
    xor r8, r8
    xor r9, r9
    xor r10, r10
    mov esi, esi

    ; inicializaciones - términos útiles para acceder a elementos de la matriz
        mov r8d, [ rdi + OFFSET_FLUID_SOLVER_N ]    ; r8 = N -

        mov rdi, rdx                                ; rdi no es necesario, dejo libre rdx para poder hacer multiplicaciones

        mov r9d, r8d                                ; r9 = N
        add r9, 2                                   ; r9 = N + 2 -

        mov r10d, r8d                               ; r10 = N
        shl r10, 1                                  ; r10 = 2 * N -

        mov eax, r8d                                ; eax = N
        mul r8d                                     ; edx:eax = N * N
        shl rdx, 32
        mov eax, eax                                ; rax = 0 | eax
        add rax, rdx                                ; rax = N * N
        mov rcx, rax                                ; rcx = N * N -

        ; estado de registros (64 bits)
        ; r8    = N
        ; r9    = N + 2
        ; r10   = 2 * N
        ; r11   = LIBRE
        ; rax   = LIBRE
        ; rcx   = N * N
        ; rdx   = LIBRE
        ; rdi   = x
        ; rsi   = b

        jmp .solver_set_bnd_horizontal
        .solver_set_bnd_horizontal_continue:

        jmp .solver_set_bnd_vertical
        .solver_set_bnd_vertical_continue:

        jmp .solver_set_bnd_corners
        .solver_set_bnd_corners_continue:

ret

.solver_set_bnd_horizontal:
    mov r11d, FLIP_SIGN_SINGLE_FLOAT_MASK
    movd xmm2, r11d
    shufps xmm2, xmm2, 0x00

    xor r11, r11                                        ; r11 = 0 - iterador (i)
    inc r11                                             ; r11 = 1
    .solver_set_bnd_horizontal_loop:
        cmp r11, r8
        jg .solver_set_bnd_horizontal_loop_exit

        ; acceso sobre (1, i)
        mov rdx, r11                                    ; rdx = i
        add rdx, r9                                     ; rdx = i + N + 2
        movdqu xmm0, [ rdi + rdx * 4 ]

        ; acceso sobre (N, i)
        mov rdx, r11                                    ; rdx = i
        add rdx, rcx                                    ; rdx = i + N * N
        add rdx, r10                                    ; rdx = i + N * N + 2 * N
        movdqu xmm1, [ rdi + rdx * 4 ]

        cmp rsi, 2                                      ; ¿ b == 2 ?
        jne .solver_set_bnd_horizontal_asign
            pxor xmm0, xmm2
            pxor xmm1, xmm2

        .solver_set_bnd_horizontal_asign:
            ; copio resultado de operación en (0, i)
            mov rdx, r11                                ; rdx = i
            movdqu [ rdi + rdx * 4 ], xmm0

            ; copio resultado de operación en (N+1, i)
            add rdx, rcx                                ; rdx = i + N * N
            add rdx, r10                                ; rdx = i + N * N + 2 * N
            add rdx, r9                                 ; rdx = i + N * N + 2 * N + N + 2
            movdqu [ rdi, rdx * 4 ], xmm1

        add r11, 4                                      ; proceso de a cuatro valores
    jmp .solver_set_bnd_horizontal_loop
    .solver_set_bnd_horizontal_loop_exit:
jmp .solver_set_bnd_horizontal_continue

.solver_set_bnd_vertical:
    mov r11d, FLIP_SIGN_SINGLE_FLOAT_MASK
    movd xmm2, r11d
    shufps xmm2, xmm2, 0xC3

    xor r11, r11                                        ; r11 = 0 - iterador (i)
    inc r11                                             ; r11 = 1
    .solver_set_bnd_vertical_loop:
        cmp r11, r8
        jg .solver_set_bnd_vertical_loop_exit

        mov rax, r9                                     ; rax = N + 2
        mul r11d                                        ; edx:eax = i * (N + 2)
        shl rdx, 32
        mov eax, eax                                    ; rax = 0 | eax
        add rdx, rax                                    ; rdx = i * (N + 2)
        add rdx, r8                                     ; rdx = i * (N + 2) + N

        movdqu xmm0, [ rdi + rdx * 4 ]

        pshufd xmm0, xmm0, 0xf0

        cmp rsi, 1                                      ; ¿ b == 1 ?
        jne .solver_set_bnd_vertical_asign
            pxor xmm0, xmm2

        .solver_set_bnd_vertical_asign:
            movdqu [ rdi + rdx * 4 ], xmm0

        inc r11                                         ; proceso de a 1 fila por vez
    jmp .solver_set_bnd_vertical_loop
    .solver_set_bnd_vertical_loop_exit:
        mov r11d, FLIP_SIGN_SINGLE_FLOAT_MASK
        ; tratamiento de primer elemento que quedo sin revisar (1, 1) -> (0, 1)
        xor rdx, rdx                                    ; rdx = 0
        inc rdx                                         ; rdx = 1
        add rdx, r9                                     ; rdx = 1 + (N + 2)
        mov eax, [ rdi + rdx * 4 ]
        cmp rsi, 1                                      ; ¿ b == 1 ?
        jne .solver_set_bnd_asign_first
            xor eax, r11d
        .solver_set_bnd_asign_first:
            mov rdx, r9                                 ; rdx = N + 2
            mov [ rdi + rdx * 4 ], eax

        ; tratamiento de segundo elemento que quedo sin revisar (N, N) -> (N + 1, N)
        mov rdx, rcx                                    ; rdx = N * N
        add rdx, r10                                    ; rdx = N * N + 2 * N
        add rdx, r8                                     ; rdx = N * N + 2 * N + N
        mov eax, [ rdi + rdx * 4 ]
        cmp rsi, 1                                      ; ¿ b = 1 ?
        jne .solver_set_bnd_asign_last
            xor eax, r11d
        .solver_set_bnd_asign_last:
            inc rdx                                     ; rdx = N * N + 2 * N + N + 1
            mov [ rdi + rdx * 4 ], eax
jmp .solver_set_bnd_vertical_continue

.solver_set_bnd_corners:
    mov r11d, MULTIPLICATION_FACTOR_POSITIVE
    movd xmm2, r11d                                     ; xmm2 = 0 | 0 | 0 | 0.5
    ; primera esquina (0, 0)
    movd xmm0, [ rdi +      4 ]                         ; xmm0 = 0 | 0 | 0 | x[1, 0]
    movd xmm1, [ rdi + r9 * 4 ]                         ; xmm1 = 0 | 0 | 0 | x[0, 1]
    addss xmm0, xmm1                                    ; xmm0 = 0 | 0 | 0 | x[1, 0] + x[0, 1]
    mulss xmm0, xmm2                                    ; xmm0 = 0 | 0 | 0 | (x[1, 0] + x[0, 1]) * 0.5
    movd [ rdi ], xmm0
    ; segunda esquina (0, N+1)
    mov rdx, rcx                                        ; rdx = N * N
    add rdx, r10                                        ; rdx = N * N + 2 * N
    movd xmm0, [ rdi + rdx * 4 ]                        ; xmm0 = 0 | 0 | 0 | x[0, N]
    add rdx, r9                                         ; rdx = N * N + 2 * N + N + 2
    mov rax, rdx                                        ; rax = N * N + 2 * N + N + 2
    inc rdx                                             ; rdx = N * N + 2 * N + N + 3
    movd xmm1, [ rdi + rdx * 4 ]                        ; xmm1 = 0 | 0 | 0 | x[1, N + 1]
    addss xmm0, xmm1                                    ; xmm0 = 0 | 0 | 0 | x[0, N] + x[1, N + 1]
    mulss xmm0, xmm2                                    ; xmm0 = 0 | 0 | 0 | (x[0, N] + x[1, N + 1]) * 0.5
    movd [ rdi + rax * 4 ], xmm0
    ; tercera esquina (N+1, 0)
    mov rdx, r8                                         ; rdx = N
    movd xmm0, [ rdi + rdx * 4 ]                        ; xmm0 = 0 | 0 | 0 | x[N, 0]
    inc rdx                                             ; rdx = N + 1
    mov rax, rdx                                        ; rax = N + 1
    add rdx, r9                                         ; rdx = (N + 1) + (N + 2)
    movd xmm1, [ rdi + rdx * 4 ]                        ; xmm1 = 0 | 0 | 0 | x[N + 1, 1]
    addss xmm0, xmm1                                    ; xmm0 = 0 | 0 | 0 | x[N, 0] + x[N + 1, 1]
    mulss xmm0, xmm2                                    ; xmm0 = 0 | 0 | 0 | (x[N, 0] + x[N + 1, 1]) * 0.5
    movd [ rdi + rax * 4 ], xmm0
    ; cuarta esquina (N+1, N+1)
    mov rdx, rcx                                        ; rdx = N * N
    add rdx, r10                                        ; rdx = N * N + 2 * N
    add rdx, r8                                         ; rdx = N * N + 2 * N + N
    inc rdx                                             ; rdx = N * N + 2 * N + N + 1
    movd xmm0, [ rdi + rdx * 4 ]                        ; xmm0 = 0 | 0 | 0 | x[N + 1, N]
    inc rdx                                             ; rdx = N * N + 2 * N + N + 2
    add rdx, r8                                         ; rdx = N * N + 2 * N + N + 2 + N
    movd xmm1, [ rdi + rdx * 4 ]                        ; xmm1 = 0 | 0 | 0 | x[N, N + 1]
    addss xmm0, xmm1                                    ; xmm0 = 0 | 0 | 0 | x[N + 1, N] + x[N, N + 1]
    mulss xmm0, xmm2                                    ; xmm0 = 0 | 0 | 0 | (x[N + 1, N] + x[N, N + 1]) * 0.5
    inc rdx                                             ; rdx = N * N + 2 * N + N + 2 + N + 1
    movd [ rdi + rdx * 4 ], xmm0
jmp .solver_set_bnd_corners_continue


;void solver_lin_solve ( fluid_solver* solver, uint32_t b, float * x, float * x0, float a, float c ){
;   uint32_t i, j, k;
;   for ( k=0 ; k<20 ; k++ ) {
;       FOR_EACH_CELL
;           x[IX(i,j)] = (x0[IX(i,j)] + a*(x[IX(i-1,j)]+x[IX(i+1,j)]+x[IX(i,j-1)]+x[IX(i,j+1)]))/c;
;       END_FOR
;       solver_set_bnd ( solver, b, x );
;   }
;}

; RDI = solver, ESI = b, RDX = &x, RCX = &x0, xmm0 = a, xmm1 = c
global solver_lin_solve
solver_lin_solve:
;stackframe
  push RBP                                ; pila alineada
  MOV RBP,RSP
  push RBX
  push R12                                ; pila alineada
  push R13
  push R14                                ; pila alineada
  push R15
  sub RSP,24                              ; pila alineada
; [RSP+8] = solver, [RSP+16] = b, RBX = x, RCX = x0
  mov [RSP + 8],ESI                      ; [RSP+16] = b
  mov RBX,RDX                             ; RBX = x
  mov [RSP + 16],RDX                      ; [RSP+24] = &x
  xor R10,R10                             ; R10 =0
  mov R10D,[RDI + offset_fluid_solver_N]  ; R10 = n
  mov [RSP],RDI                       ; [RSP+8] = &solver
  mov R11, R10                            ; R11 = n
  add R10,1                               ; R10 = n+1
  add R11,2                               ; R11 = n+2
  shl R11,2                               ; R11 = 4*(n+2)
; R12 = i, R13 = j, R14 = k
  xor R12,R12                             ; R12 = 0 = i
  xor R13,R13                             ; R13 = 0 = j
  xor R14,R14                             ; R14 = 0 = k
  inc R12                                 ; R12 = 1
  inc R13                                 ; R13 = 1
; RDI = 0x00000000ffffffff , R15 = 0xffffffff00000000
 xor RDI,RDI
 xor R15,R15
 mov RDI,[limpia_Arriba]
 mov R15,[limpia_Abajo]
; a como los datos de los floats de entrada asi los uso en simd
 xorps xmm8,xmm8                            ; xmm8 = 0
 movss xmm8,xmm0                            ; xmm8 = |0|0|0|a|
 xorps xmm0,xmm0                            ; xmm0 = 0
 movss xmm0,xmm8                            ; xmm0 = |0|0|0|a|
 pslldq xmm8,4                              ; xmm8 = |0|0|a|0|
 por xmm0,xmm8                              ; xmm0 = |0|0|a|a|
 pslldq xmm8,4                              ; xmm8 = |0|a|0|0|
 por xmm0,xmm8                              ; xmm0 = |0|a|a|a|
 pslldq xmm8,4                              ; xmm8 = |a|0|0|0|
 por xmm0,xmm8                              ; xmm0 = |a|a|a|a|
;---------------------------------------------------------------------------
 xorps xmm9,xmm9                            ; xmm9 = 0
 movss xmm9,xmm1                            ; xmm9 = |0|0|0|c|
 xorps xmm1,xmm1                            ; xmm1 = 0
 movss xmm1,xmm9                            ; xmm1 = |0|0|0|c|
 pslldq xmm9,4                              ; xmm9 = |0|0|c|0|
 por xmm1,xmm9                              ; xmm1 = |0|0|c|c|
 pslldq xmm9,4                              ; xmm9 = |0|c|0|0|
 por xmm1,xmm9                              ; xmm1 = |0|c|c|c|
 pslldq xmm9,4                              ; xmm9 = |c|0|0|0|
 por xmm1,xmm9                              ; xmm1 = |c|c|c|c|

;-----------------------aca comienza el triple ciclo-------------------------
ciclo_k:
 cmp R14,20                               ; for ( k=0 ; k<20 ; k++ )
 je terminar
ciclo_j:
 cmp R10,R12                              ; for ( i=1 ; i<n+1 ; i+4 )
 jne continuar_ciclo_j
 mov R12,1                                ; R12=1
 mov R13,1                                ; R13=1
 inc R14                                  ; R14=R14+1
 jmp ciclo_k
continuar_ciclo_j:
 cmp R10,R13                              ; for ( j=1 ; j<n+1 ; j+1 )
 jne continuar_ciclo_i
 mov R13,1                                ; R13=1
 add R12,4                                ; R12=R12+4
 jmp ciclo_j
continuar_ciclo_i:
    xor RAX,RAX                           ; RAX=0
    mov RAX,R13                           ; RAX=j
    mov R9,R11                            ; R9 = R11 = 4*(n+2)
    mul R11
    and RAX,R15                           ;limpia la parte de arriba RAX
    and RDX,RDI                           ;limpia la parte de abajo RDX
    or  RAX,RDX                           ;RAX = ((N+2)*4)*j
    lea RAX,[RAX + 4*R12]                 ;RAX = 4*i+((N+2)*4)*j
;RBX = x la direccion, RCX = x0 la direccion
    lea R8,[RBX + RAX]                    ;R8 = &x + 4*i+((N+2)*4)*j
    lea RDX,[RCX + RAX]                   ;RDX = &x0 + 4*i+((N+2)*4)*j
    movups xmm2,[RDX]                     ;xmm2 = | prev(i+3,j) | prev(i+2,j) | prev(i+1,j) | prev(i,j) |
    movups xmm3,[R8 - 4]                  ;xmm3 = | actual(i+2,j)| actual(i+1,j) | actual(i,j) | actual(i-1,j) |
    movups xmm4,[R8 + 4]                  ;xmm4 = | actual(i+4,j) | actual(i+3,j) | actual(i+2,j) | actual(i+1,j)] |
    movups xmm6,[R8 + R9]                 ;xmm6 = | actual(i+3,j+1) | actual(i+2,j+1) | actual(i+1,j+1) | actual(i,j+1) |
    mov RSI,R8                            ;RSI = &x + 4*i+((N+2)*4)*j
    sub R8,R9                             ;R8 = (&x + 4*i+((N+2)*4)*j) - 4*(n+2)
    movups xmm5,[R8]                      ;xmm5 = | actual(i+3,j-1) | actual(i+2,j-1) | actual(i+1,j-1) | actual(i,j-1) |
;----------------------------------voy alistar los datos para realizar las operaciones en simd -----------------------------

; xmmm0 =  |a|a|a|a|  xmm1= |c|c|c|c|

    movaps xmm12,xmm2                   ; xmm12 = | prev(i+3,j) | prev(i+2,j) | prev(i+1,j) | prev(i,j) |
    movaps xmm13,xmm4                   ; xmm13 = | actual(i+4,j) | actual(i+3,j) | actual(i+2,j) | actual(i+1,j)] |
    movaps xmm14,xmm5                   ; xmm14 = | actual(i+3,j-1) | actual(i+2,j-1) | actual(i+1,j-1) | actual(i,j-1) |
    movaps xmm15,xmm6                   ; xmm15 = | actual(i+3,j+1) | actual(i+2,j+1) | actual(i+1,j+1) | actual(i,j+1) |
    xorps xmm7,xmm7                     ; xmm7 = 0
    movss xmm7,xmm3                     ; xmm7 = | 0| 0 | 0 | actual(i-1,j) |
    movaps xmm8,xmm7                    ; xmm8 = | 0| 0 | 0 | actual(i-1,j) |
    pslldq xmm8,4                       ; xmm8 = | 0| 0 | actual(i-1,j) | 0 |
;------------------------------------------------------------------------------------------------------------------------
;   xmm8 = | 0| 0 | actual(i-1,j) | 0 |

    addps xmm7,xmm13                    ; xmm7 = | ? | ? | ? | actual(i-1,j) + actual(i+1,j) |
    addps xmm7,xmm14                    ; xmm7 = | ? | ? | ? | actual(i-1,j) + actual(i+1,j) + actual(i,j-1) |
    addps xmm7,xmm15                    ; xmm7 = | ? | ? | ? | actual(i-1,j) + actual(i+1,j) + actual(i,j-1) + actual(i,j+1) |
    mulps xmm7,xmm0                     ; xmm7 = | ? | ? | ? | a*(actual(i-1,j) + actual(i+1,j) + actual(i,j-1) + actual(i,j+1)) |
    addps xmm7,xmm12                    ; xmm7 = | ? | ? | ? | prev(i,j) + (a*(actual(i-1,j) + actual(i+1,j) + actual(i,j-1) + actual(i,j+1))) |
    divps xmm7,xmm1                     ; xmm7 = | ? | ? | ? | c/(prev(i,j) + (a*(actual(i-1,j) + actual(i+1,j) + actual(i,j-1) + actual(i,j+1)))) |

; k0 = c/(prev(i,j) + (a*(actual(i-1,j) + actual(i+1,j) + actual(i,j-1) + actual(i,j+1))))

    movss xmm8,xmm7                     ; xmm8 = | 0| 0 | actual(i-1,j) | k0 |
    pslldq xmm8,4                       ; xmm8 = | 0| actual(i-1,j) | k0 | 0 |
    psrldq xmm12,4                      ; xmm12 = | 0 | prev(i+3,j) | prev(i+2,j) | prev(i+1,j) |
    psrldq xmm13,4                      ; xmm13 = | 0 | actual(i+4,j) | actual(i+3,j) | actual(i+2,j) |
    psrldq xmm14,4                      ; xmm14 = | 0 | actual(i+3,j-1) | actual(i+2,j-1) | actual(i+1,j-1) |
    psrldq xmm15,4                      ; xmm15 = | 0 | actual(i+3,j+1) | actual(i+2,j+1) | actual(i+1,j+1) |
    addps xmm7,xmm13                    ; xmm7 = | ? | ? | ? | k0 + actual(i+2,j) |
    addps xmm7,xmm14                    ; xmm7 = | ? | ? | ? | k0 + actual(i+2,j) + actual(i+1,j-1) |
    addps xmm7,xmm15                    ; xmm7 = | ? | ? | ? | k0 + actual(i+2,j) + actual(i+1,j-1) + actual(i+1,j+1) |
    mulps xmm7,xmm0                     ; xmm7 = | ? | ? | ? | a*(k0 + actual(i+2,j) + actual(i+1,j-1) + actual(i+1,j+1)) |
    addps xmm7, xmm12                   ; xmm7 = | ? | ? | ? | prev(i+1,j) + a*(k0 + actual(i+2,j) + actual(i+1,j-1) + actual(i+1,j+1)) |
    divps xmm7,xmm1                     ; xmm7 = | ? | ? | ? | c/(prev(i+1,j) + a*(k0 + actual(i+2,j) + actual(i+1,j-1) + actual(i+1,j+1))) |

; k1 = c/(prev(i+1,j) + a*(k0 + actual(i+2,j) + actual(i+1,j-1) + actual(i+1,j+1)))

    movss xmm8,xmm7                     ; xmm8 = | 0| actual(i-1,j) | k0 | k1 |
    pslldq xmm8,4                       ; xmm8 = | actual(i-1,j) | k0 | k1 | 0 |
    psrldq xmm12,4                      ; xmm12 = | 0 | 0 | prev(i+3,j) | prev(i+2,j) |
    psrldq xmm13,4                      ; xmm13 = | 0 | 0 | actual(i+4,j) | actual(i+3,j) |
    psrldq xmm14,4                      ; xmm14 = | 0 | 0 | actual(i+3,j-1) | actual(i+2,j-1) |
    psrldq xmm15,4                      ; xmm15 = | 0 | 0 | actual(i+3,j+1) | actual(i+2,j+1) |
    addps xmm7,xmm13                    ; xmm7 = | ? | ? | ? | k1 + actual(i+3,j) |
    addps xmm7,xmm14                    ; xmm7 = | ? | ? | ? | k1 + actual(i+3,j) + actual(i+2,j-1) |
    addps xmm7,xmm15                    ; xmm7 = | ? | ? | ? | k1 + actual(i+3,j) + actual(i+2,j-1) + actual(i+2,j+1) |
    mulps xmm7,xmm0                     ; xmm7 = | ? | ? | ? | a*(k1 + actual(i+3,j) + actual(i+2,j-1) + actual(i+2,j+1)) |
    addps xmm7,xmm12                    ; xmm7 = | ? | ? | ? | prev(i+2,j) + a*(k1 + actual(i+3,j) + actual(i+2,j-1) + actual(i+2,j+1)) |
    divps xmm7,xmm1                     ; xmm7 = | ? | ? | ? | c/(prev(i+2,j) + a*(k1 + actual(i+3,j) + actual(i+2,j-1) + actual(i+2,j+1))) |

; k2 = c/(prev(i+2,j) + a*(k1 + actual(i+3,j) + actual(i+2,j-1) + actual(i+2,j+1)))

    movss xmm8,xmm7                     ; xmm8 = | actual(i-1,j) | k0 | k1 | k2 |
    movaps xmm7,xmm8                    ; xmm7 = | actual(i-1,j) | k0 | k1 | k2 |
    shufps xmm7,xmm8, 00011011b         ; xmm7 = | k2 | k1 | k0 | actual(i-1,j) |
    movaps xmm3,xmm7                    ; xmm3 = | k2 | k1 | k0 | actual(i-1,j) |
;-----------------------ahora vamos hacer los calculos de simd--------------------------------------------

    addps xmm3,xmm4
    addps xmm3,xmm5
    addps xmm3,xmm6
    mulps xmm3,xmm0
    addps xmm3,xmm2
    divps xmm3,xmm1
    movups[RSI],xmm3
    inc R13                           ; R13=R13+1
    jmp continuar_ciclo_j
terminar:
    mov RDI,[RSP ]
    mov ESI,[RSP + 8]
    mov RDX,[RSP + 16]
    call solver_set_bnd
;stackframe
  add RSP,24
  pop R15
  pop R14
  pop R13
  pop R12
  pop RBX
  POP RBP
ret

; global solver_set_bnd
; solver_set_bnd:
; ;stackframe
;   push RBP ;alineada
;   MOV RBP,RSP;alineada
; ;stackframe
;   POP RBP
; ret


; global solver_project
; solver_project:
; ;stackframe
;   push RBP ;alineada
;   MOV RBP,RSP;alineada
; ;stackframe
;   POP RBP
; ret
