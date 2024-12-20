/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/queue.h>

#include <rte_memory.h>
#include <rte_launch.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_debug.h>

static int
lcore_hello(__attribute__((unused)) void *arg)
{
	unsigned lcore_id;
	lcore_id = rte_lcore_id();                              // 用于获取当前执行的逻辑核心lcore的ID
	printf("hello from core %u\n", lcore_id);
	return 0;
}

int
main(int argc, char **argv)
{
	int ret;
	unsigned lcore_id;

	ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_panic("Cannot init EAL\n");

	/* call lcore_hello() on every slave lcore */
	RTE_LCORE_FOREACH_SLAVE(lcore_id) {
		rte_eal_remote_launch(lcore_hello, NULL, lcore_id); // 多线程
	}

	/* call it on master lcore too */
	lcore_hello(NULL);

	rte_eal_mp_wait_lcore();

	/* clean up the EAL */
	rte_eal_cleanup();

	return 0;
}

/*
EAL: Detected 4 lcore(s)                                    // 有 4 个逻辑核心可用
EAL: Detected 1 NUMA nodes                                  // 1 个 NUMA非一致性内存访问节点
EAL: Multi-process socket /var/run/dpdk/rte/mp_socket       // 多进程消息传递
EAL: Selected IOVA mode 'PA'                                // 选择I/O虚拟地址IOVA模式为物理地址(PA) 这是DPDK中用于内存管理的一种模式
EAL: Probing VFIO support...                                // 正在探测VFIO(虚拟功能I/O)支持 这是一种Linux内核的设备驱动模型
EAL: PCI device 0000:02:01.0 on NUMA socket -1              // 检测到PCI设备 位于NUMA节点-1 表示没有特定NUMA节点
EAL:   probe driver: 8086:100f net_e1000_em                 // 为该PCI设备探测到驱动程序 显示设备的厂商ID和设备ID
EAL: PCI device 0000:02:05.0 on NUMA socket -1
EAL:   probe driver: 8086:100f net_e1000_em
EAL: PCI device 0000:02:06.0 on NUMA socket -1
EAL:   probe driver: 8086:100f net_e1000_em
hello from core 1
hello from core 2
hello from core 3
hello from core 0

网卡特指多队列网卡 dpdk可以将队列指定给特定的核
*/
