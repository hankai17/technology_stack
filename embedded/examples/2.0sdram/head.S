.equ        MEM_CTL_BASE,       0x48000000
.equ        SDRAM_BASE,         0x30000000

.text
.global _start
_start:
    bl  disable_watch_dog               @ 
    bl  memsetup                        @ 设置存储控制器
    bl  copy_steppingstone_to_sdram     @ 复制代码到SDRAM上
                                        @ bl 指令是相对跳转指令 虽然运行地址0x300...早已在编译的时候决定了 但是相对跳转指令跟地址无关 它只跟偏移有关
    ldr pc, =on_sdram                   @ 跳到SDRAM上执行main函数 反汇编pc指向了300...x地址处
on_sdram:
    ldr sp, =0x34000000                 @ pc栈指向64MB(2块sdram 每块32MB)处
    bl  main
halt_loop:
    b   halt_loop

disable_watch_dog:
    mov r1,     #0x53000000
    mov r2,     #0x0
    str r2,     [r1]
    mov pc,     lr      @ 返回到bl处 (lr的值即是bl指令的下一行地址)

copy_steppingstone_to_sdram:
    @ 将Steppingstone(垫脚石即SRAM)全部4K数据 复制到SDRAM
    @ Steppingstone起始地址为0x00000000  SDRAM起始地址为0x30000000
    
    mov r1, #0
    ldr r2, =SDRAM_BASE
    mov r3, #4*1024
1:  
    ldr r4, [r1],#4     @ 从Steppingstone读取4字节数据 并让r1 + 4
    str r4, [r2],#4     @ 把从sram上读到的数据 复制到sdram 并让r2 + 4
    cmp r1, r3          @ 同下
    bne 1b              @
    mov pc,     lr      @

memsetup:
    mov r1,     #MEM_CTL_BASE       @ r1即总线寄存器地址
    adrl    r2, mem_cfg_val         @ r2指向下面代码块开头
    add r3,     r1, #52             @ 指向结束处 @ 13*4 = 54   13个寄存器
1:  
    ldr r4,     [r2], #4            @ 读取r2中的值放到r4上 读取完后r2 + 4
    str r4,     [r1], #4            @ 将r4上的值 写到r1处  写完后r1 + 4
    cmp r1,     r3                  @ 判断是否设置完所有13个寄存器值
    bne 1b                          @ continue
    mov pc,     lr                  @ 返回


.align 4
mem_cfg_val:
    .long   0x22011110      @ BWSCON
    .long   0x00000700      @ BANKCON0
    .long   0x00000700      @ BANKCON1
    .long   0x00000700      @ BANKCON2
    .long   0x00000700      @ BANKCON3  
    .long   0x00000700      @ BANKCON4
    .long   0x00000700      @ BANKCON5
    .long   0x00018005      @ BANKCON6
    .long   0x00018005      @ BANKCON7
    .long   0x008C07A3      @ REFRESH
    .long   0x000000B1      @ BANKSIZE
    .long   0x00000030      @ MRSRB6
    .long   0x00000030      @ MRSRB7
