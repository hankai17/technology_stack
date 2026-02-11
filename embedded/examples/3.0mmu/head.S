@ 功能：设置SDRAM，将第二部分代码复制到SDRAM，设置页表，启动MMU 然后跳到SDRAM继续执行

.text
.global _start
_start:
    ldr sp, =4096                       @ 设置栈指针，以下都是C函数，调用前需要设好栈
    bl  disable_watch_dog               @ 关闭WATCHDOG 否则CPU会不断重启
    bl  memsetup                        @ 设置存储控制器以使用SDRAM
    bl  copy_2th_to_sdram               @ 将第二部分代码(leds.c)复制到SDRAM的物理地址(0x3x开头的)上
    bl  create_page_table               @ 设置页表 (完成物理地址->链接地址映射)
    bl  mmu_init                        @ 启动MMU  此函数之后的地址访问均得通过mmu
    ldr sp, =0xB4000000                 @ 重设栈指针，指向SDRAM顶端(使用虚拟地址)
    ldr pc, =0xB0004000                 @ 跳到SDRAM中继续执行第二部分代码 @这就是链接地址
halt_loop:
    b   halt_loop


