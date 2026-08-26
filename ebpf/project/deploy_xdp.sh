#rm -f /sys/fs/bpf/xdp_filter
#bpftool net detach xdp dev ens36
#bpftool prog load xdp_filter.o /sys/fs/bpf/xdp_filter type xdp
#bpftool net attach xdp pinned /sys/fs/bpf/xdp_filter dev ens36
#bpftool net show dev ens36
#bpftool map dump name ssh_port

rm -f /sys/fs/bpf/xdp_filter
bpftool net detach xdp dev ens36
bpftool prog load xdp_prog_kern.o /sys/fs/bpf/xdp_filter type xdp
bpftool net attach xdp pinned /sys/fs/bpf/xdp_filter dev ens36
bpftool net show dev ens36
bpftool map dump name ssh_port



rm -f /sys/fs/bpf/xdp_filter 2>/dev/null || true
bpftool net detach xdp dev eth0 2>/dev/null || true
bpftool prog load xdp_filter.o /sys/fs/bpf/xdp_filter type xdp
bpftool net attach xdp pinned /sys/fs/bpf/xdp_filter dev eth0
bpftool net show dev eth0 | grep -q 'xdp'
bpftool net show dev eth0 | grep 'driver id' 
bpftool prog show id 上面的driverid  ---> 获取map信息
从map信息里提取 map_ids
bpftool map show id map_ids中的id ---> 找到白名单的map id
bpftool map dump id white_map_id  ---> 查看白名单有哪些规则
bpftool map update id white_map_id key hex c0 a8 20 01 value hex 00 00 00 01
bpftool map dump id white_map_id  ---> 查看白名单有哪些规则


bpftool net detach xdp dev eth0 2>/dev/null || true
