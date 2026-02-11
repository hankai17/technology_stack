//#include<stdint.h>
//#define	GPFCON      (*(volatile uint32_t *)0x56000050)

#define	GPFCON      (*(volatile unsigned long *)0x56000050)
#define	GPFDAT      (*(volatile unsigned long *)0x56000054)

// 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00
//                 8   76   54   32   10 移动位数

#define	GPF4_out    (1<<(4*2))
#define	GPF5_out    (1<<(5*2))
#define	GPF6_out    (1<<(6*2))
#define	GPF7_out    (1<<(7*2))

void wait(volatile unsigned long dly) {
	for(; dly > 0; dly--);
}

int main(void)
{
	unsigned long i = 0;

	GPFCON = GPF4_out|GPF5_out|GPF6_out;    // 设置456端口为输出模式

	while (1) {
		wait(30000);
		GPFDAT = (~(i<<4));
		if (++i == 8) {
			i = 0;
        }
	}

	return 0;
}
