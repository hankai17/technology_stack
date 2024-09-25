
yum -y install systemtap
debuginfo-install kernel
yum -y install systemtap-runtime
yum -y install kernel-devel
yum -y install systemtap-client
yum -y install systemtap-devel
yum -y install kernel-debug-devel
yum -y install kernel-debug-debuginfo
yum -y install wget
yum -y install dpkg


stap -l 'kernel.function("*")'
stap -l 'kernel.trace("*")'
stap -l 'syscall.*'
stap -l 'vfs.*'

#每行都捕获了哪些变量
stap -L 'kernel.statement("tcp_write_xmit@*:*")'
#stap -L 'kernel.statement("cubictcp_cong_avoid")' #查看一些信息
#kernel.statement 的行数 可以随意更改
