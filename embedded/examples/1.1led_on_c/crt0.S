.text
.global _start

_start:
        ldr     r0, =0x53000000 @ 看门狗寄存器地址
        mov     r1, #0x0
        str     r1, [r0]        @ 写入0 即禁止看门狗 否则cpu将不断重启

        ldr     sp, =1024*4     @ 设置栈指针 不能>4K 因为nand_flash在上电后会将前4k复制到SRAM中
        bl      main            @ 跳转到main 并将返回地址(即halt_loop这一行地址)存到lr寄存器
halt_loop:
            b       halt_loop

@ C Runtime: 跟纯汇编不一样 需要以下几个动作
@ 硬件初始化:
@ 1.喂狗: 关闭看门狗
@ 2.设置时钟 200MHZ
@ 3.初始化SDRAM 以防片内RAM(即SRAM)不够用

@ C语音运行的硬件环境:
@ 1.给(main)函数入参及函数内部变量数提供 栈空间 即初始化sp指针
@ 2.设置返回地址
@ 3.运行main函数
@ 4.运行main函数完毕 则清理
