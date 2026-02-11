.text
.global _start                      @ 声明_start为全局符号，告诉链接器这是程序的入口点

_start:
        LDR     R0,=0x56000050      @ R0 GPFCON @ 此为伪指令 实际会转换成合适的指令将 32 位地址加载到 R0(普通 MOV 指令无法直接加载 32 位立即数)
        MOV     R1,#0x00000500      @ 将立即数0x500加载到寄存器 R1 中
        STR     R1,[R0]             @ 将 R1 中的值（0x500）写入到 R0 指向的内存地址

        LDR     R0,=0x56000054      @ R0 GPBDAT
        MOV     R1,#0x00000000
        STR     R1,[R0]
MAIN_LOOP:
            B       MAIN_LOOP

@ MOV 操作 CPU 内部的立即数 / 寄存器，不访存；LDR 操作内存 / 外设的数据，必须访存
@ #是立即数 没有#即代表一块地址
