bpftool btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h
clang -g -O2 -target bpf -D__TARGET_ARCH_x86_64__ -I. -c xdp_filter.c -o xdp_filter.o
clang -g -O2 -target bpf -D__TARGET_ARCH_x86_64__ -I. -c xdp_prog_kern.c -o xdp_prog_kern.o

gcc -g -o main main.c config.c utils.c cmdline.c \
  -lbpf \
  -lelf \
  -lxdp \
  -lconfig \
  -I/usr/include/x86_64-linux-gnu \
  -I/usr/include \
  -O0
./main --config xdpfw.conf 
