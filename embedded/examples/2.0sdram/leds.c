
#define	GPFCON		(*(volatile unsigned long *)0x56000050)
#define	GPFDAT		(*(volatile unsigned long *)0x56000054)

#define	GPF4_out	(1<<(4*2))
#define	GPF5_out	(1<<(5*2))
#define	GPF6_out	(1<<(6*2))

void  wait(volatile unsigned long dly)
{
	for(; dly > 0; dly--);
}

int main(void)
{
	unsigned long i = 0;
	
	GPFCON = GPF4_out|GPF5_out|GPF6_out;

	while(1){
		wait(30000);
		GPFDAT = (~(i<<4));
		if(++i == 8)
			i = 0;
	}

	return 0;
}

// 存储管理器是焊死到cpu板子里的 跟外设(SDRAM: HY57V561620C(L)T(P))是解耦的
// SDRAM配置:
//
// 总线宽度: 16 * 2(块)
//      1111 1111 1111 1111
//      2^16 = 64k个地址 每个地址代表一个byte
//      16个数据线宽 则每个byte地址均能访问到 
//      15个数据线宽 则每2个byte地址均能访问到 
//      14个数据线宽 则每3个byte地址均能访问到 
//      13个数据线宽 则每4个byte地址均能访问到 
//      
//      2440有总线(32位)寄存器(废话肯定有) 这个寄存器可同时最多配置8个总线(eg: SRAM|SDRAM类的需总线的外设) 也就是说每个总线(外设)占用4个bit位用于配置
//          其实跟led寄存器一样
//      每个总线 对应一个bank(每个bank最大支持寻址128MB) 所以2440又有8个bank寄存器(band0~5只支持RAM Like设备 band6~7支持SDRAM)
//
// 13行 9列

// 刷新寄存器
//      64ms/8k
//
// bank 每块有4个

// SRAM的访问很简单(比较贵 2440中为4k) 无需配置上述的bank|行列|刷新周期等
// 网卡|norflash接口跟SRAM一样统称为RAM Like设备 访问亦很简单

// nor启动:
//      0地址指向 norflash上的开头
// nand启动:
//      0地址指向片内SRAM(4k) 然后cpu从nandflash上拷贝前4k到SRAM上 之后开始执行程序
// sdram启动:
//      nand启动后 然后喂狗|设置存储控制器 然后再把sram上的代码拷贝到SDRAM上
