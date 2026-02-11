@******************************************************************************
@ File：head.S
@ 功能：初始化，设置中断模式、管理模式的栈，设置好中断处理函数
@******************************************************************************       
   
.extern     main
.text 
.global _start 
_start:
@******************************************************************************       
@ 中断向量，本程序中，除Reset和HandleIRQ外，其它异常都没有使用
@******************************************************************************       
    b   Reset           @  复位异常（Reset）——上电/复位时执行，唯一真正用到的入口 ARM 架构要求 0x0 地址开始存放 8 个异常处理入口

@ 0x04: 未定义指令中止模式的向量地址
HandleUndef:
    b   HandleUndef 
 
@ 0x08: 管理模式的向量地址，通过SWI指令进入此模式
HandleSWI:
    b   HandleSWI

@ 0x0c: 指令预取终止导致的异常的向量地址
HandlePrefetchAbort:
    b   HandlePrefetchAbort

@ 0x10: 数据访问终止导致的异常的向量地址
HandleDataAbort:
    b   HandleDataAbort

@ 0x14: 保留
HandleNotUsed:
    b   HandleNotUsed

@ 0x18: 中断模式的向量地址
    b   HandleIRQ

@ 0x1c: 快中断模式的向量地址
HandleFIQ:
    b   HandleFIQ

Reset:                  
    ldr sp, =4096           @ 设置栈指针，默认是管理模式, 以下都是C函数，调用前需要设好栈
    bl  disable_watch_dog   @ 关闭WATCHDOG，否则CPU会不断重启
    
    msr cpsr_c, #0xd2       @ 切换到IRQ中断模式，并关闭中断 设置禁止中断与快中断 并 设置普通中断模式(7种中断的一种常见中断) P76
    ldr sp, =3072           @ 设置中断模式栈指针

    msr cpsr_c, #0xd3       @ 切回管理模式 @ cpsr寄存器 cpu只有一个
    ldr sp, =4096           @ 设置管理模式栈指针，
                            @ 其实复位之后，CPU就处于管理模式，
                            @ 前面的“ldr sp, =4096”完成同样的功能，此句可省略

    bl  init_led            @ 初始化LED的GPIO管脚
    bl  init_irq            @ 调用中断初始化函数，在init.c中
    msr cpsr_c, #0x5f       @ 开启总中断
    
    ldr lr, =halt_loop      @ 设置返回地址
    ldr pc, =main           @ 调用main函数
halt_loop:
    b   halt_loop

HandleIRQ:
    sub lr, lr, #4                  @ 计算返回地址
                                    @ CPU进入中断时，lr会自动保存“中断返回后要执行的指令地址”，
                                    @ 但ARM流水线机制会导致lr多4字节，所以减4修正
    stmdb   sp!,    { r0-r12,lr }   @ 保存现场 @ 中断发生前的 CPU 状态（管理 / 用户模式）的通用寄存器，保存到 IRQ 模式的栈中
                                    @ 用 IRQ 模式的sp（栈指针，已设置为 3072）作为栈基址；
                                    @ 把共享的r0-r12（管理模式下的数值）、IRQ 模式的lr（修正后的返回地址），批量压入 IRQ 模式的栈中；
    
    ldr lr, =int_return             @ 设置调用ISR即EINT_Handle函数后的返回地址  
    ldr pc, =EINT_Handle            @ 调用中断服务函数，在interrupt.c中
int_return:
    ldmia   sp!,    { r0-r12,pc }^  @ 把这些值恢复到r0-r12，保证管理模式的程序能继续正常执行
    

