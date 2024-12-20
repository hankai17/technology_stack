source ./dpdk.rc
cd ${RTE_SDK}/${RTE_TARGET}/kmod/
modprobe uio
modprobe uio_pci_generic
insmod igb_uio.ko                                   # 将网卡寄存器 映射到 用户态的uio的PMD中(这一步也称为将网卡绑定到dpdk的用户态)  以便用户态在PMD中处理包
lsmod | grep uio

cd ${RTE_SDK}/usertools/
./dpdk-devbind.py --status-dev net
ifconfig ens33 down
ifconfig ens37 down
./dpdk-devbind.py --bind=igb_uio 0000:02:01.0       # 将网卡绑定 igb_uio驱动
#./dpdk-devbind.py --bind=uio_pci_generic 0000:02:01.0
./dpdk-devbind.py --bind=igb_uio 0000:02:05.0
./dpdk-devbind.py --status-dev net

ls -l /dev/uio*
ls -l /sys/class/uio/uio0/maps/

ip l

cd ${RTE_SDK}/examples/helloworld
make
./build/helloworld

#cd ${RTE_SDK}/examples/l2fwd
#make
#./build/l2fwd

# ./dpdk-devbind.py -b 0000:02:01.0
# ifconfig xx up

