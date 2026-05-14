bpftool net detach xdp dev ens33
bpftool prog load xdp_filter.bpf.o /sys/fs/bpf/xdp_filter type xdp
ll /sys/fs/bpf/xdp_filter
bpftool net attach xdp pinned /sys/fs/bpf/xdp_filter dev ens33 
bpftool net show dev ens33
xdp:
ens33(2) generic id 87

tc:

flow_dissector:

netfilter:

bpftool prog show id 87
87: xdp  name xdp_filter  tag 7067a6b11c0e4935  gpl
        loaded_at 2026-05-14T15:43:41+0800  uid 0
        xlated 2592B  jited 1517B  memlock 4096B  map_ids 11,13,12,14,17,15,16
        btf_id 154

bpftool map show id 13
13: hash  name blacklist_v4  flags 0x0
        key 4B  value 4B  max_entries 4096  memlock 341824B

bpftool map update id 11 key hex c0 a8 20 01 value hex 00 00 00 01
bpftool map update id 13 key hex c0 a8 20 01 value hex 00 00 00 01
bpftool map delete id 11 key hex c0 a8 20 01
