#!/bin/bash

# 在脚本开头添加
trap cleanup SIGINT SIGTERM

cleanup() {
    echo -e "\n${YELLOW}正在清理测试环境...${NC}"
    ssh ${SSH_USER}@${SERVER_IP} "pkill -9 iperf3; pkill -9 mpstat" 2>/dev/null
    pkill -9 iperf3 2>/dev/null
    exit 1
}

# 在其他配置参数后面添加
START_PORT=${START_PORT:-5201}  # 默认从5201开始

THREADS=${THREADS:-$(nproc)}  # 默认使用CPU核心数

STAGES=(
    "环境准备"
    "无XDP基准测试"
    "启用XDP测试"
    "XDP+黑名单测试"
    "XDP+白名单测试"
    "收尾工作"
)

# 显示帮助信息的函数
show_usage() {
    echo "用法: $0 [-s <阶段号>] [-t <线程数>] [-p <起始端口>] [-d <测试时长>] [-b <黑名单大小>] [-w <白名单大小>] [-h]"
    echo "选项:"
    echo "  -s <阶段号>      指定要运行的阶段(1-${#STAGES[@]}), 多个阶段用逗号分隔"
    echo "  -t <线程数>      指定测试线程数（默认：CPU核心数）"
    echo "  -p <起始端口>    指定起始端口号（默认：5201）"
    echo "  -d <测试时长>    指定测试持续时间（秒）（默认：1）"
    echo "  -b <黑名单大小>  指定黑名单规则数量（默认：50000）"
    echo "  -w <白名单大小>  指定白名单规则数量（默认：50000）"
    echo "  -m <小包大小>    指定小包测试的包大小（默认：200字节）"
    echo "  -l <大包大小>    指定大包测试的包大小（默认：1400字节）"
    echo "  -P <PPS包大小>   指定PPS测试的包大小（默认：64字节）"
    echo "  -h               显示此帮助"
    echo
    echo "可用阶段:"
    for i in "${!STAGES[@]}"; do
        echo "  $((i+1)). ${STAGES[$i]}"
    done
    echo
    echo "示例:"
    echo "  $0                     # 运行所有阶段"
    echo "  $0 -s 2 -d 60         # 只运行无XDP基准测试，测试时长60秒"
    echo "  $0 -s 2,3 -t 16       # 运行指定阶段，使用16个线程"
    echo "  $0 -b 100000 -w 50000 # 使用自定义黑白名单大小"
}

# 验证阶段号的函数
validate_stages() {
    local stages=$1
    local valid=true
    
    # 检查格式
    if ! echo "$stages" | grep -qE '^[0-9]+(,[0-9]+)*$'; then
        echo -e "${RED}错误: 无效的阶段格式。请使用逗号分隔的数字。${NC}"
        return 1
    fi  # 这里缺少了一个 fi
    
    # 检查每个阶段号是否有效
    for stage in $(echo "$stages" | tr ',' ' '); do
        if [ "$stage" -lt 1 ] || [ "$stage" -gt "${#STAGES[@]}" ]; then
            echo -e "${RED}错误: 无效的阶段号 $stage。有效范围: 1-${#STAGES[@]}${NC}"
            valid=false
        fi
    done
    
    [ "$valid" = true ]
}


# 颜色定义
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

# 配置
SERVER_IP="10.144.114.142"
SSH_USER="root"
TEST_DURATION=1  # 修改为60秒
SMALL_PACKET=${SMALL_PACKET:-200}  # 默认小包大小200字节
LARGE_PACKET=1400                  # 大包测试1400字节
PPS_PACKET=64                      # PPS测试使用64字节
XDP_PATH="/Application/KEDGE_DDoS/xdp_toolkit"
BPFTOOL="${XDP_PATH}/bpftool/sbin/bpftool"
BLACKLIST_SIZE=50000
WHITELIST_SIZE=50000   
LATENCY_COUNT=100     # 添加延迟测试包数量
LATENCY_PACKET=64      # 添加延迟测试包大小


# 系统性能优化参数
SYSCTL_PARAMS=(
    "net.core.rmem_max=268435456"
    "net.core.wmem_max=268435456"
    "net.core.rmem_default=268435456"
    "net.core.wmem_default=268435456"
    "net.ipv4.tcp_rmem=4096 87380 268435456"
    "net.ipv4.tcp_wmem=4096 87380 268435456"
    "net.core.netdev_budget=1000"
    "net.core.netdev_budget_usecs=2000"
    "net.ipv4.tcp_max_syn_backlog=32768"
    "net.core.somaxconn=65535"
    "net.ipv4.tcp_max_tw_buckets=1440000"
    "net.ipv4.tcp_tw_reuse=1"
    "net.ipv4.tcp_fin_timeout=10"
    "net.ipv4.tcp_slow_start_after_idle=0"
    "net.ipv4.tcp_mtu_probing=1"
    "net.core.optmem_max=268435456"
    "net.ipv4.ip_local_port_range=1024 65535"
    "net.ipv4.tcp_window_scaling=1"
    "net.ipv4.tcp_timestamps=1"
    "net.ipv4.tcp_sack=1"
)

get_nic_info() {
    local host=$1
    local cmd="
        # 获取驱动信息
        driver=\$(ethtool -i eth0 2>/dev/null | grep 'driver:' | awk -F': ' '{print \$2}' | tr -d ' ') && \
        # 获取版本息
        version=\$(ethtool -i eth0 2>/dev/null | grep '^version:' | awk -F': ' '{print \$2}' | tr -d ' ') && \
        # 获取固件版本
        firmware=\$(ethtool -i eth0 2>/dev/null | grep 'firmware-version:' | awk -F': ' '{print \$2}' | tr -d ' ') && \
        # 获取网卡型号
        vendor=\$(lspci -v | grep -A1 'Ethernet controller' | head -n1 | sed 's/.*Ethernet controller: //') && \
        # 获取光模块信息
        vendor_name=\$(ethtool -m eth0 2>/dev/null | grep 'Vendor name' | awk -F': ' '{print \$2}' | tr -d ' ') && \
        vendor_pn=\$(ethtool -m eth0 2>/dev/null | grep 'Vendor PN' | awk -F': ' '{print \$2}' | tr -d ' ') && \
        # 输出所有信息，用管道符分隔
        echo \"\${driver}|\${version}|\${firmware}|\${vendor}|\${vendor_name}|\${vendor_pn}\""

    if [ "$host" = "local" ]; then
        eval "$cmd"
    else
        ssh ${SSH_USER}@${SERVER_IP} "$cmd"
    fi
}

# 获取XDP模式函数
get_xdp_mode() {
    local host=$1
    local cmd="
        output=\$(${BPFTOOL} net show dev eth0 2>/dev/null) && \
        if [ -n \"\$output\" ] && echo \"\$output\" | grep -q \"xdp:\"; then
            xdp_info=\$(echo \"\$output\" | grep -A1 \"xdp:\" | tail -n1 | tr -s ' ')
            if echo \"\$xdp_info\" | grep -q \"driver\"; then
                prog_id=\$(echo \"\$xdp_info\" | grep -o 'id [0-9]*' | awk '{print \$2}')
                echo \"已加载XDP(原生模式 prog_id \$prog_id)\"
            elif echo \"\$xdp_info\" | grep -q \"generic\"; then
                prog_id=\$(echo \"\$xdp_info\" | grep -o 'id [0-9]*' | awk '{print \$2}')
                echo \"已加载XDP(通用模式 prog_id \$prog_id)\"
            elif echo \"\$xdp_info\" | grep -q \"hw\"; then
                prog_id=\$(echo \"\$xdp_info\" | grep -o 'id [0-9]*' | awk '{print \$2}')
                echo \"已加载XDP(硬件模式 prog_id \$prog_id)\"
            elif echo \"\$xdp_info\" | grep -q \"id\"; then
                prog_id=\$(echo \"\$xdp_info\" | grep -o 'id [0-9]*' | awk '{print \$2}')
                echo \"已加载XDP(prog_id \$prog_id)\"
            else
                echo \"已加载XDP(未知模式)\"
            fi
        else
            echo \"未加载XDP程序\"
        fi"

    if [ "$host" = "local" ]; then
        eval "$cmd"
    else
        ssh ${SSH_USER}@${SERVER_IP} "$cmd"
    fi
}

# 检查本地依赖
install_local_dependencies() {
    echo -e "${BLUE}[*] 检查本地依赖...${NC}"
    if [ -f /etc/redhat-release ]; then
        sudo yum install -y epel-release iperf3 jq bc ethtool
    elif [ -f /etc/debian_version ]; then
        sudo apt-get update
        sudo apt-get install -y iperf3 jq bc ethtool
    else
        echo -e "${RED}错误: 不支持的操作系统${NC}"
        exit 1
    fi
}

# 系统参数优化
optimize_system() {
    local host=$1
    local is_remote=$2
    
    echo -e "${BLUE}[*] 优化系统参数 ($host)...${NC}"
    
    if [ "$is_remote" = "true" ]; then
        ssh ${SSH_USER}@${SERVER_IP} "
            # 应用系统参数
            for param in \"${SYSCTL_PARAMS[@]}\"; do
                sysctl -w \$param >/dev/null 2>&1
            done
            
            # CPU和中断优化
            num_cores=\$(nproc)
            
            # 网卡队列优化
            ethtool -G eth0 rx 4096 tx 4096 2>/dev/null || true
            ethtool -K eth0 gro on gso on tso on sg on tx-nocache-copy off 2>/dev/null || true
            ethtool -A eth0 autoneg off rx off tx off 2>/dev/null || true
            ethtool -L eth0 combined \$num_cores 2>/dev/null || true
            
            # 增加传队列长度
            ip link set eth0 txqueuelen 10000 2>/dev/null || true
            
            # IRQ亲和性优化
            eth_interrupts=\$(cat /proc/interrupts | grep eth0 | awk '{print \$1}' | tr -d ':')
            core=0
            for irq in \$eth_interrupts; do
                echo \$core > /proc/irq/\$irq/smp_affinity_list 2>/dev/null || true
                core=\$(( (core + 1) % num_cores ))
            done
            
            # RPS/RFS优化
            echo 32768 > /proc/sys/net/core/rps_sock_flow_entries 2>/dev/null || true
            for queue in /sys/class/net/eth0/queues/rx-*; do
                echo 4096 > \$queue/rps_flow_cnt 2>/dev/null || true
            done
            
            # 设置RPS CPU掩码
            all_mask=\$(printf '%x' \$((2**\$num_cores-1)))
            for rx in /sys/class/net/eth0/queues/rx-*/rps_cpus; do
                echo \$all_mask > \$rx 2>/dev/null || true
            done
            
            # 其他网络优化
            echo 1000000 > /proc/sys/net/core/netdev_budget 2>/dev/null || true
            echo 300 > /proc/sys/net/core/netdev_budget_usecs 2>/dev/null || true
            
            # 启用网卡多队列
            num_queues=\$(ethtool -l eth0 2>/dev/null | grep -i \"combined\" | head -n1 | awk '{print \$2}')
            if [ ! -z \"\$num_queues\" ]; then
                ethtool -L eth0 combined \$num_queues 2>/dev/null || true
            fi
            
            # 禁用irqbalance服务
            systemctl stop irqbalance 2>/dev/null || true
            systemctl disable irqbalance 2>/dev/null || true
        "
    else
        # 本地系统优化
        # 应用系统参数
        for param in "${SYSCTL_PARAMS[@]}"; do
            sysctl -w "$param" >/dev/null 2>&1
        done
        
        # 网卡优化
        sudo ethtool -G eth0 rx 4096 tx 4096 2>/dev/null || true
        sudo ethtool -K eth0 gro on gso on tso on sg on tx-nocache-copy off 2>/dev/null || true
        sudo ethtool -A eth0 autoneg off rx off tx off 2>/dev/null || true
        sudo ethtool -L eth0 combined $(nproc) 2>/dev/null || true
        
        # 增加传输队列长度
        sudo ip link set eth0 txqueuelen 10000 2>/dev/null || true
        
        # 本地其他优化参数
        echo 1000000 | sudo tee /proc/sys/net/core/netdev_budget >/dev/null 2>&1
        echo 300 | sudo tee /proc/sys/net/core/netdev_budget_usecs >/dev/null 2>&1
    fi
}

# 准备服务端环境
setup_server() {
    echo -e "${BLUE}[*] 准备服务端环境...${NC}"
    
    ssh ${SSH_USER}@${SERVER_IP} "
        # 安装依赖
        if [ -f /etc/redhat-release ]; then
            echo '安装必要软件包...'
            yum install -y epel-release iperf3 sysstat ethtool >/dev/null 2>&1
        elif [ -f /etc/debian_version ]; then
            echo '安装必要软件包...'
            apt-get update >/dev/null 2>&1
            apt-get install -y iperf3 sysstat ethtool >/dev/null 2>&1
        fi
        
        # 系统优化
        echo '优化系统参数...'
        for param in \
            \"net.core.rmem_max=268435456\" \
            \"net.core.wmem_max=268435456\" \
            \"net.core.rmem_default=268435456\" \
            \"net.core.wmem_default=268435456\" \
            \"net.ipv4.tcp_rmem=4096 87380 268435456\" \
            \"net.ipv4.tcp_wmem=4096 87380 268435456\" \
            \"net.core.netdev_budget=1000\" \
            \"net.core.netdev_budget_usecs=2000\" \
            \"net.ipv4.tcp_max_syn_backlog=32768\" \
            \"net.core.somaxconn=65535\" \
            \"net.ipv4.tcp_max_tw_buckets=1440000\" \
            \"net.ipv4.tcp_tw_reuse=1\" \
            \"net.ipv4.tcp_fin_timeout=10\" \
            \"net.ipv4.tcp_slow_start_after_idle=0\" \
            \"net.ipv4.tcp_mtu_probing=1\" \
            \"net.core.optmem_max=268435456\" \
            \"net.ipv4.ip_local_port_range=1024 65535\" \
            \"net.ipv4.tcp_window_scaling=1\" \
            \"net.ipv4.tcp_timestamps=1\" \
            \"net.ipv4.tcp_sack=1\"; do
            sysctl -w \$param >/dev/null 2>&1
        done
        
        # 网卡优化
        echo '优化网卡参数...'
        ethtool -G eth0 rx 4096 tx 4096 2>/dev/null || true
        ethtool -K eth0 gro on gso on tso on sg on tx-nocache-copy off 2>/dev/null || true
        ethtool -A eth0 autoneg off rx off tx off 2>/dev/null || true
        
        # 设置队列长度
        ip link set eth0 txqueuelen 10000 2>/dev/null || true
        
        # 创建并清理测试目录
        echo '准备测试目录...'
        mkdir -p ${XDP_PATH}/test_results
        chmod 777 ${XDP_PATH}/test_results
        rm -rf ${XDP_PATH}/test_results/*
        
        # 清理现有程序和进程
        echo '清理现有环境...'
        ${BPFTOOL} net detach xdp dev eth0 2>/dev/null || true
        rm -f /sys/fs/bpf/xdp_filter 2>/dev/null || true
        rm -f /sys/fs/bpf/log_buffer 2>/dev/null || true
        
        # 停止旧进程
        pkill -9 iperf3 2>/dev/null || true
        pkill -9 mpstat 2>/dev/null || true
        
        # 等待进程完全停
        while pgrep -x iperf3 >/dev/null || pgrep -x mpstat >/dev/null; do
            sleep 1
        done
        
        # 验证环境
        echo '验证境配置...'
        if ! command -v iperf3 >/dev/null 2>&1; then
            echo '错误: iperf3 未安装成功'
            exit 1
        fi
        
        if ! command -v mpstat >/dev/null 2>&1; then
            echo '错误: sysstat 未安装成功'
            exit 1
        fi
        
        # 确认网卡设置
        if ! ip link show eth0 >/dev/null 2>&1; then
            echo '错误: 未找到eth0网卡'
            exit 1
        fi
        
        echo '✓ 服务端环境准备就绪'
    "
    
    local ret=$?
    if [ $ret -eq 0 ]; then
        echo -e "${GREEN}[+] 服务端环境准备完成${NC}"
    else
        echo -e "${RED}错误: 服务端环境准备失败 (返回码: $ret)${NC}"
        exit 1
    fi
}

# 创建本地测试目录
setup_local() {
    echo -e "${BLUE}[*] 准备本地环境...${NC}"
    
    # 清理并创建测试目录
    if [ -d "test_results" ]; then
        rm -rf test_results
    fi
    mkdir -p test_results
    
    # 优本地系统
    optimize_system "local" "false"
}

# 检查网络状态
check_network() {
    echo -e "${BLUE}[*] 检查网络状态...${NC}"
    
    # 本地网卡状态
    echo "本网卡状态："
    ethtool eth0 | grep -E "Speed|Duplex|Link"
    
    # 服务端网卡状态
    echo -e "\n服务端网卡状态："
    ssh ${SSH_USER}@${SERVER_IP} "ethtool eth0 | grep -E 'Speed|Duplex|Link'"
    
    # 网络延迟测试
    echo -e "\n网络延迟测试："
    ping -c 2 $SERVER_IP
    
    # MTU检查
    echo -e "\nMTU配置："
    echo "本地MTU: $(ip link show eth0 | grep mtu)"
    echo "服务端MTU: $(ssh ${SSH_USER}@${SERVER_IP} 'ip link show eth0 | grep mtu')"
}

# XDP操作
xdp_operations() {
    local action=$1
    
    case "$action" in
        "load")
            echo -e "${BLUE}[*] 加载XDP程序...${NC}"
            ssh ${SSH_USER}@${SERVER_IP} "
                # 确保目录存在
                mkdir -p /sys/fs/bpf
                
                # 完全清理旧的XDP程序
                ${BPFTOOL} net detach xdp dev eth0 2>/dev/null || true
                rm -f /sys/fs/bpf/xdp_filter 2>/dev/null || true
                sleep 1
                
                # 检查程序文件
                if [ ! -f ${XDP_PATH}/xdp_filter.o ]; then
                    echo '错误: XDP程序文件不存在'
                    ls -l ${XDP_PATH}/
                    exit 1
                fi
                
                # 加载新程序
                cd ${XDP_PATH}
                ${BPFTOOL} prog load xdp_filter.o /sys/fs/bpf/xdp_filter type xdp
                if [ \$? -ne 0 ]; then
                    echo '错误: 加载XDP程序失败'
                    exit 1
                fi
                
                # 附加到网卡
                ${BPFTOOL} net attach xdp pinned /sys/fs/bpf/xdp_filter dev eth0
                if [ \$? -ne 0 ]; then
                    echo '错误: 附加XDP程序到网卡失败'
                    exit 1
                fi
                
                # 验证XDP程序状态
                if ${BPFTOOL} net show dev eth0 | grep -q 'xdp'; then
                    echo '✓ XDP程序加载成功'
                    ${BPFTOOL} net show dev eth0
                else
                    echo '错误: XDP程序未成功附加'
                    exit 1
                fi
            "
            ;;
            
        "unload")
            echo -e "${BLUE}[*] 卸载XDP程序...${NC}"
            ssh ${SSH_USER}@${SERVER_IP} "
                ${BPFTOOL} net detach xdp dev eth0 2>/dev/null || true
                rm -f /sys/fs/bpf/xdp_filter 2>/dev/null || true
                echo '✓ XDP程序已卸载'
            "
            ;;
    esac
    
    local ret=$?
    if [ $ret -ne 0 ]; then
        echo -e "${RED}XDP操作失败 (返回码: $ret)${NC}"
        return 1
    fi
}

# 获取map ID通过称
get_map_id() {
    local map_name=$1
    ssh ${SSH_USER}@${SERVER_IP} "
        prog_id=\$(${BPFTOOL} prog show | grep xdp_filter | awk '{print \$1}' | sed 's/://')
        if [ -z \"\$prog_id\" ]; then
            exit 1
        fi
        
        map_ids=\$(${BPFTOOL} prog show id \$prog_id | grep map_ids | sed 's/.*map_ids //' | tr -d '[]' | tr ',' ' ')
        
        for map_id in \$map_ids; do
            name=\$(${BPFTOOL} map show id \$map_id | grep name | awk '{print \$3}')
            if [ \"\$name\" = \"$map_name\" ]; then
                echo \$map_id
                exit 0
            fi
        done
        exit 1
    "
}

# 白名单管理
manage_whitelist() {
    local action=$1
    
    echo -e "${BLUE}[*] 白名单操作: $action${NC}"
    
    ssh ${SSH_USER}@${SERVER_IP} "
        # 首先检查XDP程序是否已加载
        if ! ${BPFTOOL} net show dev eth0 | grep -q 'xdp'; then
            echo '错误: XDP程序未加载'
            ${BPFTOOL} net show dev eth0
            exit 1
        fi
        
        # 获取XDP程序ID (使用driver id)
        prog_id=\$(${BPFTOOL} net show dev eth0 | grep 'driver id' | awk '{print \$NF}')
        if [ -z \"\$prog_id\" ]; then
            echo '错: 无法获取XDPID'
            echo '当前XDP状态:'
            ${BPFTOOL} net show dev eth0
            exit 1
        fi
        
        echo \"找到XDP程序ID: \$prog_id\"
        
        # 取map IDs (使用更直接的式)
        map_info=\$(${BPFTOOL} prog show id \$prog_id)
        echo \"程序信息: \$map_info\"
        
        map_ids=\$(echo \"\$map_info\" | grep -oP 'map_ids \K[0-9,]+' | tr -d '[]' || true)
        if [ -z \"\$map_ids\" ]; then
            echo '错误: 无法获取map IDs'
            echo '程序详情:'
            echo \"\$map_info\"
            exit 1
        fi
        
        echo \"找到map IDs: \$map_ids\"
        
        # 查找whitelist_v4的map ID
        whitelist_map_id=''
        for id in \$(echo \$map_ids | tr ',' ' '); do
            map_info=\$(${BPFTOOL} map show id \$id 2>/dev/null)
            if echo \"\$map_info\" | grep -q 'name whitelist_v4'; then
                whitelist_map_id=\$id
                break
            fi
        done
        
        if [ -z \"\$whitelist_map_id\" ]; then
            echo '错误: 未找到whitelist_v4 map'
            echo '可用的maps:'
            for id in \$(echo \$map_ids | tr ',' ' '); do
                echo \"Map ID \$id:\"
                ${BPFTOOL} map show id \$id 2>/dev/null || echo \"(无法访问)\"
            done
            exit 1
        fi
        
        echo \"找到whitelist_v4 map ID: \$whitelist_map_id\"
        
        case \"$action\" in
            \"add\")
                echo '添加IP到白名单...'
                total_ips=$WHITELIST_SIZE
                batch_size=\$(( total_ips / 10 ))  # 每次添加总量的十分之一
                
                while true; do
                    # 获取当前白名单数量
                    start_count=\$(${BPFTOOL} map dump id \$whitelist_map_id | wc -l)
                    
                    if [ \$start_count -ge \$total_ips ]; then
                        echo \" 名单已完成 (当前: \$start_count, 目标: \$total_ips)\"
                        break
                    fi
                    
                    # 计算本次需要添加的数量
                    remaining=\$(( total_ips - start_count ))
                    current_batch=\$(( remaining > batch_size ? batch_size : remaining ))
                    
                    echo \"当前进度: \$start_count/\$total_ips\"
                    echo \"本次将添加: \$current_batch 个IP\"
                    
                    # 计算起始值
                    # 从10.1.1.1开始，确保不会生成0.0.0.0
                    base_count=\$start_count
                    section=\$(( (base_count / 250) + 1 ))
                    ip=\$(( (base_count % 250) + 1 ))
                    
                    # 创建临时批处理文件
                    tmp_file=\$(mktemp)
                    count=0
                    
                    echo \"生成批处理命令...\"
                    while [ \$count -lt \$current_batch ]; do
                        # 确保section不超过254
                        if [ \$section -gt 254 ]; then
                            echo \"错误: IP地址段超出范围\"
                            rm -f \$tmp_file
                            exit 1
                        fi
                        
                        # 确保IP地址效
                        if [ \$section -gt 0 ] && [ \$ip -gt 0 ] && [ \$ip -lt 255 ]; then
                            printf 'map update id %d key hex 0a 01 %02x %02x value hex 00 00 00 01\n' \
                                \$whitelist_map_id \$section \$ip >> \$tmp_file
                            
                            count=\$((count + 1))
                            if [ \$((count % 1000)) -eq 0 ]; then
                                printf \"\\r生成进度: %d/%d (当前IP: 10.1.%d.%d)\" \
                                    \$count \$current_batch \$section \$ip
                            fi
                        fi
                        
                        # 更新IP地址计数
                        ip=\$((ip + 1))
                        if [ \$ip -ge 255 ]; then
                            ip=1
                            section=\$((section + 1))
                        fi
                    done
                    
                    # 执行批处理命令
                    echo -e \"\\n执行批量更新...\"
                    ${BPFTOOL} batch file \$tmp_file
                    
                    # 清理临时文件
                    rm -f \$tmp_file
                    
                    # 验证本次添加
                    new_count=\$(${BPFTOOL} map dump id \$whitelist_map_id | wc -l)
                    added=\$(( new_count - start_count ))
                    echo \"本批次添加完成:\"
                    echo \"- 起始数量: \$start_count\"
                    echo \"- 当前数量: \$new_count\"
                    echo \"- 实际添加: \$added\"
                    echo \"- 预期加: \$current_batch\"
                    
                    if [ \$added -ne \$current_batch ]; then
                        echo \"✗ 警告: 添加数量不符合预期\"
                        exit 1
                    fi
                    
                    # 验证添加的IP否正确
                    echo \"验证新添加的IP:\"
                    ${BPFTOOL} map dump id \$whitelist_map_id | tail -n 5 | while read -r line; do
                        key=\$(echo \$line | grep -oP 'key: \\K[0-9a-f ]+' | tr -d ' ')
                        if [ -n \"\$key\" ]; then
                            a=\$((0x\${key:0:2}))
                            b=\$((0x\${key:2:2}))
                            c=\$((0x\${key:4:2}))
                            d=\$((0x\${key:6:2}))
                            if [ \$a -eq 0 ] || [ \$b -eq 0 ] || [ \$c -eq 0 ] || [ \$d -eq 0 ] || \
                               [ \$d -eq 255 ]; then
                                echo \"警告: 测到无效IP: \$a.\$b.\$c.\$d\"
                                exit 1
                            fi
                        fi
                    done
                    
                    echo \"当总进度: \$new_count/\$total_ips\"
                    echo \"\"
                done
                
                # 最终验证
                final_count=\$(${BPFTOOL} map dump id \$whitelist_map_id | wc -l)
                echo \"白名单添加完成:\"
                echo \"预期总数: \$total_ips\"
                echo \"实际数: \$final_count\"
                
                if [ \$final_count -eq \$total_ips ]; then
                    echo \"✓ 验证成功: 总数符合预期\"
                else
                    echo \"✗ 验证失败: 总数不符合预期\"
                    exit 1
                fi
                ;;
                
            \"clear\")
                echo '空白名单...'
                ${BPFTOOL} map flush id \$whitelist_map_id >/dev/null 2>&1
                count=\$(${BPFTOOL} map dump id \$whitelist_map_id | wc -l)
                echo \"白名单已清空 (当前条目数: \$count)\"
                ;;
                
            \"count\")
                count=\$(${BPFTOOL} map dump id \$whitelist_map_id | wc -l)
                echo \"当前白名单包含 \$count 个IP\"
                ;;
        esac
    "
    
    local ret=$?
    if [ $ret -ne 0 ]; then
        echo -e "${RED}白名单操作失败 (返回码: $ret)${NC}"
        return 1
    fi
}


# 黑名单管理
manage_blacklist() {
    local action=$1
    
    echo -e "${BLUE}[*] 黑名单操: $action${NC}"
    
    ssh ${SSH_USER}@${SERVER_IP} "
        # 首先检查XDP程序是否已加载
        if ! ${BPFTOOL} net show dev eth0 | grep -q 'xdp'; then
            echo '错误: XDP程序未加载'
            ${BPFTOOL} net show dev eth0
            exit 1
        fi
        
        # 获取XDP程序ID (使用driver id)
        prog_id=\$(${BPFTOOL} net show dev eth0 | grep 'driver id' | awk '{print \$NF}')
        if [ -z \"\$prog_id\" ]; then
            echo '错误: 无法获取XDP程序ID'
            echo '当前XDP状态:'
            ${BPFTOOL} net show dev eth0
            exit 1
        fi
        
        echo \"找到XDP程序ID: \$prog_id\"
        
        # 获取map IDs (使用更直接的方式)
        map_info=\$(${BPFTOOL} prog show id \$prog_id)
        echo \"程序信息: \$map_info\"
        
        map_ids=\$(echo \"\$map_info\" | grep -oP 'map_ids \K[0-9,]+' | tr -d '[]' || true)
        if [ -z \"\$map_ids\" ]; then
            echo '错误: 无法获取map IDs'
            echo '程序详情:'
            echo \"\$map_info\"
            exit 1
        fi
        
        echo \"找到map IDs: \$map_ids\"
        
        # 查找blacklist_v4的map ID
        blacklist_map_id=''
        for id in \$(echo \$map_ids | tr ',' ' '); do
            map_info=\$(${BPFTOOL} map show id \$id 2>/dev/null)
            if echo \"\$map_info\" | grep -q 'name blacklist_v4'; then
                blacklist_map_id=\$id
                break
            fi
        done
        
        if [ -z \"\$blacklist_map_id\" ]; then
            echo '错误: 未找到blacklist_v4 map'
            echo '可用的maps:'
            for id in \$(echo \$map_ids | tr ',' ' '); do
                echo \"Map ID \$id:\"
                ${BPFTOOL} map show id \$id 2>/dev/null || echo \"(无法访问)\"
            done
            exit 1
        fi
        
        echo \"找到blacklist_v4 map ID: \$blacklist_map_id\"
        
        case \"$action\" in
            \"add\")
                echo '添加IP到黑名单...'
                total_ips=$BLACKLIST_SIZE
                batch_size=\$(( total_ips / 10 ))  # 每次添加总量的十分之一
                
                while true; do
                    # 获取当前黑名单数量
                    start_count=\$(${BPFTOOL} map dump id \$blacklist_map_id | wc -l)
                    
                    if [ \$start_count -ge \$total_ips ]; then
                        echo \"✓ 黑名单已完成 (当前: \$start_count, 目标: \$total_ips)\"
                        break
                    fi
                    
                    # 计算本次需要添加的数量
                    remaining=\$(( total_ips - start_count ))
                    current_batch=\$(( remaining > batch_size ? batch_size : remaining ))
                    
                    echo \"当前进度: \$start_count/\$total_ips\"
                    echo \"本次将添加: \$current_batch 个IP\"
                    
                    # 计算起始值
                    # 从10.1.1.1开始，确保不会生成0.0.0.0
                    base_count=\$start_count
                    section=\$(( (base_count / 250) + 1 ))
                    ip=\$(( (base_count % 250) + 1 ))
                    
                    # 创建临时批处理文件
                    tmp_file=\$(mktemp)
                    count=0
                    
                    echo \"生成批处理命令...\"
                    while [ \$count -lt \$current_batch ]; do
                        # 确保section不超过254
                        if [ \$section -gt 254 ]; then
                            echo \"错误: IP地址段超出范围\"
                            rm -f \$tmp_file
                            exit 1
                        fi
                        
                        # 确保IP地址有效
                        if [ \$section -gt 0 ] && [ \$ip -gt 0 ] && [ \$ip -lt 255 ]; then
                            printf 'map update id %d key hex 0a 01 %02x %02x value hex 00 00 00 01\n' \
                                \$blacklist_map_id \$section \$ip >> \$tmp_file
                            
                            count=\$((count + 1))
                            if [ \$((count % 1000)) -eq 0 ]; then
                                printf \"\\r生成进度: %d/%d (当前IP: 10.1.%d.%d)\" \
                                    \$count \$current_batch \$section \$ip
                            fi
                        fi
                        
                        # 更新IP地址计数
                        ip=\$((ip + 1))
                        if [ \$ip -ge 255 ]; then
                            ip=1
                            section=\$((section + 1))
                        fi
                    done
                    
                    # 执行批处理命令
                    echo -e \"\\n执行批量更新...\"
                    ${BPFTOOL} batch file \$tmp_file
                    
                    # 清理时文
                    rm -f \$tmp_file
                    
                    # 验证本次添加
                    new_count=\$(${BPFTOOL} map dump id \$blacklist_map_id | wc -l)
                    added=\$(( new_count - start_count ))
                    echo \"本批次添加完成:\"
                    echo \"- 起始数量: \$start_count\"
                    echo \"- 当前数量: \$new_count\"
                    echo \"- 实际添加: \$added\"
                    echo \"- 预期添加: \$current_batch\"
                    
                    if [ \$added -ne \$current_batch ]; then
                        echo \"✗ 警告: 添加数量不符合预期\"
                        exit 1
                    fi
                    
                    # 验证添加的IP是否正确
                    echo \"验证新添加的IP:\"
                    ${BPFTOOL} map dump id \$blacklist_map_id | tail -n 5 | while read -r line; do
                        key=\$(echo \$line | grep -oP 'key: \\K[0-9a-f ]+' | tr -d ' ')
                        if [ -n \"\$key\" ]; then
                            a=\$((0x\${key:0:2}))
                            b=\$((0x\${key:2:2}))
                            c=\$((0x\${key:4:2}))
                            d=\$((0x\${key:6:2}))
                            if [ \$a -eq 0 ] || [ \$b -eq 0 ] || [ \$c -eq 0 ] || [ \$d -eq 0 ] || \
                               [ \$d -eq 255 ]; then
                                echo \"警告: 检测到无效IP: \$a.\$b.\$c.\$d\"
                                exit 1
                            fi
                        fi
                    done
                    
                    echo \"当前总进度: \$new_count/\$total_ips\"
                    echo \"\"
                done
                
                # 最终验证
                final_count=\$(${BPFTOOL} map dump id \$blacklist_map_id | wc -l)
                echo \"黑名单添加完成:\"
                echo \"预期总数: \$total_ips\"
                echo \"实际总数: \$final_count\"
                
                if [ \$final_count -eq \$total_ips ]; then
                    echo \"✓ 验证成功: 总数符合预期\"
                else
                    echo \" 验证失败: 总数不符合预期\"
                    exit 1
                fi
                ;;
                
            \"clear\")
                echo '清空黑名单...'
                ${BPFTOOL} map flush id \$blacklist_map_id >/dev/null 2>&1
                count=\$(${BPFTOOL} map dump id \$blacklist_map_id | wc -l)
                echo \"黑名单清空 (当前条目数: \$count)\"
                ;;
                
            \"count\")
                count=\$(${BPFTOOL} map dump id \$blacklist_map_id | wc -l)
                echo \"当前黑名单包含 \$count 个IP\"
                ;;
        esac
    "
    
    local ret=$?
    if [ $ret -ne 0 ]; then
        echo -e "${RED}黑名单操作失败 (返回码: $ret)${NC}"
        return 1
    fi
}


run_test() {
    local test_name=$1
    local packet_size=$2
    local description=$3
    
    # 记录测试开始时的网卡统计
    local start_stats=$(ssh ${SSH_USER}@${SERVER_IP} "ethtool -S eth0")
    local start_rx_packets=$(echo "$start_stats" | grep "rx_packets:" | awk '{print $2}')
    local start_tx_packets=$(echo "$start_stats" | grep "tx_packets:" | awk '{print $2}')
    local start_rx_bytes=$(echo "$start_stats" | grep "rx_bytes:" | awk '{print $2}')
    local start_tx_bytes=$(echo "$start_stats" | grep "tx_bytes:" | awk '{print $2}')
    
    # 参数验证
    if [ -z "$packet_size" ]; then
        echo -e "${RED}错误: 无效的包大小${NC}"
        return 1
    fi
    
    echo -e "${GREEN}[+] 开始测试: $description (包大小: $packet_size bytes)${NC}"
    
    # 确保测试结果目录存在
    local result_dir="test_results/${test_name}_${packet_size}"
    rm -rf "$result_dir"  # 清理可能存在的旧目录
    mkdir -p "$result_dir"
    
    # 1. 停止所有旧进程并等待完全停止
    echo "停止旧进程..."
    ssh -q ${SSH_USER}@${SERVER_IP} "
        pkill -9 iperf3 2>/dev/null || true
        pkill -9 mpstat 2>/dev/null || true
        sleep 1
        while pgrep iperf3 >/dev/null; do
            sleep 1
        done
    " 2>/dev/null
    
    # 2. 计算进程数（基于CPU核心数）
    local num_cores=$(nproc)
    local num_processes=$THREADS
    echo "系统信息:"
    echo "- CPU核心数: $num_cores"
    echo "- 测试进程数: $num_processes"
    
    # 3. 启动服务端进程
    echo "启动服务端进程..."
    ssh -q ${SSH_USER}@${SERVER_IP} "
        # 确保没有残留进程和端口
        pkill -9 iperf3 2>/dev/null || true
        sleep 1
        while ss -tln | grep -q ':520[0-9]\\+'; do
            sleep 1
        done
        
        # 清理旧的日志文件
        rm -f ${XDP_PATH}/test_results/iperf3_server_*.log
        
        # 提高系统限制
        ulimit -n 1000000
        sysctl -w net.core.somaxconn=65535 >/dev/null 2>&1
        
        # 创建启动脚本
        cat > /tmp/start_iperf.sh << 'EOFMARKER'
#!/bin/bash

num_processes=\$1
xdp_path=\$2
num_cores=\$(nproc)
START_PORT=\$3

start_server() {
    local port=\$1
    local logfile=\$2
    local cpu_id=\$((port - START_PORT))
    cpu_id=\$((cpu_id % num_cores))
    local max_retries=3
    local retry=0
    
    #echo \"启动服务器: port=\${port} cpu=\${cpu_id}\"
    
    while [ \$retry -lt \$max_retries ]; do
        if ss -tln | grep -q \":\${port}[[:space:]]\"; then
            echo \"端口 \${port} 已被占用，正在清理...\"
            fuser -k \${port}/tcp 2>/dev/null || true
            sleep 1
        fi
        
        # 使用基本的iperf3服务端参数
        taskset -c \${cpu_id} iperf3 \
            -s \
            -p \${port} \
            -D \
            -1 \
            --logfile \"\${logfile}\"
        
        for ((i=0; i<10; i++)); do
            if ss -tln | grep -q \":\${port}[[:space:]]\"; then
                return 0
            fi
            sleep 0.1
        done
        
        retry=\$((retry + 1))
        [ \$retry -lt \$max_retries ] && sleep 1
    done
    
    return 1
}

verify_servers() {
    local expected_count=\$1
    local port_base=\$START_PORT
    local all_ok=true
    
    for ((port=port_base; port<port_base+expected_count; port++)); do
        if ! ss -tln | grep -q \":\${port}[[:space:]]\"; then
            echo \"端口 \${port} 未正常监听\"
            all_ok=false
        fi
    done
    
    if [ \"\$all_ok\" = true ]; then
        return 0
    else
        echo \"当前监听的端口:\"
        ss -tln | grep -E \":\${START_PORT}[0-9]*\"
        echo \"当前运行的iperf3进程:\"
        ps aux | grep iperf3 | grep -v grep
        return 1
    fi
}

# 启动所有服务器进程
echo \"正在启动 \${num_processes} 个服务器程...\"
failed_ports=()
success_count=0

for ((port=START_PORT; port<START_PORT+num_processes; port++)); do
    logfile=\"\${xdp_path}/test_results/iperf3_server_\${port}.log\"
    if start_server \"\${port}\" \"\${logfile}\"; then
        echo -n \".\"
        success_count=\$((success_count + 1))
    else
        failed_ports+=(\"\${port}\")
        echo \"F\"
    fi
done
echo

sleep 1

if verify_servers \"\$num_processes\"; then
    echo \"所有服务器进程启动并验证成功 (\${success_count}/\${num_processes})\"
    exit 0
else
    echo \"服务器验证失败\"
    echo \"启动失败的端口: \${failed_ports[*]}\"
    echo \"成功启动数量: \${success_count}/\${num_processes}\"
    exit 1
fi
EOFMARKER

        chmod +x /tmp/start_iperf.sh
        /tmp/start_iperf.sh $num_processes $XDP_PATH $START_PORT
        status=\$?
        rm -f /tmp/start_iperf.sh
        exit \$status
    "
    
    local server_status=$?
    if [ $server_status -ne 0 ]; then
        echo -e "${RED}错误: 服务端启动失败${NC}"
        return 1
    fi
    
    echo "等待服务就绪..."
    sleep 5
    
    echo "执行最终验证..."
    local verify_status=$(ssh -q ${SSH_USER}@${SERVER_IP} "
        failures=0
        for ((port=$START_PORT; port<$START_PORT+$num_processes; port++)); do
            if ! ss -tln | grep -q \":\${port}[[:space:]]\"; then
                echo \"端口 \${port} 就绪\"
                failures=\$((failures + 1))
            fi
        done
        echo \$failures
    ")
    
    if ! [[ "$verify_status" =~ ^[0-9]+$ ]]; then
        echo -e "${RED}错误: 验证状态无效${NC}"
        return 1
    fi
    
    if [ "$verify_status" -ne 0 ]; then
        echo -e "${RED}错误: 在 $verify_status 个端未就绪${NC}"
        return 1
    fi
    
    # 在启动iperf3客户端测试之前启动CPU监控
    echo "启动CPU监控..."
    ssh ${SSH_USER}@${SERVER_IP} "
        # 确保目录存在
        mkdir -p ${XDP_PATH}/test_results
        
        # 记录初始软中断值
        grep NET_RX /proc/softirqs > ${XDP_PATH}/test_results/${test_name}_softirqs_start.txt
        
        # 启动CPU监控（不使用后台运行）
        mpstat -P ALL 1 ${TEST_DURATION} > ${XDP_PATH}/test_results/${test_name}_mpstat.txt
        
        # 记录结束软中断值
        grep NET_RX /proc/softirqs > ${XDP_PATH}/test_results/${test_name}_softirqs_end.txt
        
        # 处理CPU数据
        awk '
            /^Average:.*all/ {
                cpu_total = 100-\$NF     # idle在最后一列
                cpu_user = \$3           # usr在第3列
                cpu_sys = \$5            # sys在第5列
                cpu_soft = \$7           # soft在第7列
                printf \"%.2f %.2f %.2f %.2f\", cpu_total, cpu_user, cpu_sys, cpu_soft
            }
        ' ${XDP_PATH}/test_results/${test_name}_mpstat.txt > ${XDP_PATH}/test_results/${test_name}_cpu_stats.txt
        
        # 处理软中断数据
        start_total=\$(awk '{sum=0; for(i=2;i<=NF;i++) sum+=\$i; print sum}' ${XDP_PATH}/test_results/${test_name}_softirqs_start.txt)
        end_total=\$(awk '{sum=0; for(i=2;i<=NF;i++) sum+=\$i; print sum}' ${XDP_PATH}/test_results/${test_name}_softirqs_end.txt)
        
        if [ ! -z \"\$start_total\" ] && [ ! -z \"\$end_total\" ]; then
            irq_per_sec=\$(( (end_total - start_total) / ${TEST_DURATION} ))
            echo -n \" \$irq_per_sec\" >> ${XDP_PATH}/test_results/${test_name}_cpu_stats.txt
        else
            echo -n \" 0\" >> ${XDP_PATH}/test_results/${test_name}_cpu_stats.txt
        fi
        
        # 确保文件存在且有正确的权限
        chmod 644 ${XDP_PATH}/test_results/${test_name}_cpu_stats.txt
    " &
    MONITOR_PID=$!
    
    # 5. 启动客户端测试
    echo "开始性能测试..."
    local pids=()
    for ((port=START_PORT; port<START_PORT+num_processes; port++)); do
        local cpu_id=$((port - START_PORT))
        cpu_id=$((cpu_id % num_cores))
        #echo "启动客户端: port=${port} cpu=${cpu_id}"
        # 使用基本的iperf3客户端参数
        taskset -c $cpu_id iperf3 \
            -c "$SERVER_IP" \
            -p $port \
            -l $packet_size \
            -t $TEST_DURATION \
            -i 1 \
            -Z \
            -w 4M \
            -P 1 \
            --json > "${result_dir}/result_${port}.json" 2>"${result_dir}/error_${port}.log" &
        pids+=($!)
        sleep 0.1  # 添加小延迟避免同时启动太多进程
    done
    
    echo "等待测试完成..."
    for pid in "${pids[@]}"; do
        wait $pid
    done
    
    echo "处理测试结果..."
    local total_bits=0
    local total_bytes=0
    local failed_tests=0
    
    for ((port=START_PORT; port<START_PORT+num_processes; port++)); do
        local result_file="${result_dir}/result_${port}.json"
        local error_file="${result_dir}/error_${port}.log"
        
        if [ -f "$result_file" ]; then
            if jq empty "$result_file" 2>/dev/null; then
                local bits=$(jq -r '.end.sum_received.bits_per_second' "$result_file")
                local bytes=$(jq -r '.end.sum_received.bytes' "$result_file")
                
                if [ "$bits" != "null" ] && [ "$bytes" != "null" ]; then
                    total_bits=$(echo "$total_bits + $bits" | bc)
                    total_bytes=$(echo "$total_bytes + $bytes" | bc)
                else
                    failed_tests=$((failed_tests + 1))
                    echo "警告: port $port 结果无效"
                    cat "$error_file"
                fi
            else
                failed_tests=$((failed_tests + 1))
                echo "警告: port $port JSON解析失败"
                cat "$error_file"
            fi
        else
            failed_tests=$((failed_tests + 1))
            echo "警告: port $port 结果文件不存在"
            [ -f "$error_file" ] && cat "$error_file"
        fi
    done
    
    if [ $failed_tests -eq 0 ]; then
        local total_mbits=$(echo "scale=2; $total_bits / 1000000" | bc)
        echo "$total_mbits" > "test_results/${test_name}_${packet_size}_throughput.txt"
        
        echo -e "${GREEN}测试结果:${NC}"
        echo "总带宽: ${total_mbits} Mbits/sec"
        
        if [ "$packet_size" = "$PPS_PACKET" ]; then
            local packet_overhead=42  # Ethernet(14) + IP(20) + UDP(8)
            local total_packet_size=$((packet_size + packet_overhead))
            local total_packets=$((total_bytes / total_packet_size))
            local pps=$(echo "scale=2; $total_packets / $TEST_DURATION" | bc)
            
            echo "PPS: ${pps} 包/秒"
            echo "$pps" > "test_results/${test_name}_${packet_size}_pps.txt"
        fi
    else
        echo -e "${RED}错误: $failed_tests 个测试结果无效${NC}"
        return 1
    fi
    
    echo "收集CPU统计..."
    ssh -q ${SSH_USER}@${SERVER_IP} "mpstat 1 ${TEST_DURATION}" | \
        awk '/^Average:/ {print 100-$NF}' > "test_results/${test_name}_cpu_stats.txt"
    
    # 验证CPU统计文件
    if [ ! -s "test_results/${test_name}_cpu_stats.txt" ]; then
        echo "0" > "test_results/${test_name}_cpu_stats.txt"
    fi
    
    echo "清理测试环境..."
    ssh -q ${SSH_USER}@${SERVER_IP} "pkill -9 iperf3" 2>/dev/null || true
    sleep 1
    
    echo -e "${GREEN}测试完成${NC}"
    
    # 记录测试结束时的网卡统计
    local end_stats=$(ssh ${SSH_USER}@${SERVER_IP} "ethtool -S eth0")
    local end_rx_packets=$(echo "$end_stats" | grep "rx_packets:" | awk '{print $2}')
    local end_tx_packets=$(echo "$end_stats" | grep "tx_packets:" | awk '{print $2}')
    local end_rx_bytes=$(echo "$end_stats" | grep "rx_bytes:" | awk '{print $2}')
    local end_tx_bytes=$(echo "$end_stats" | grep "tx_bytes:" | awk '{print $2}')
    
    # 计算网卡统计的结果
    local rx_bytes_diff=$((end_rx_bytes - start_rx_bytes))
    local tx_bytes_diff=$((end_tx_bytes - start_tx_bytes))
    local rx_packets_diff=$((end_rx_packets - start_rx_packets))
    local tx_packets_diff=$((end_tx_packets - start_tx_packets))
    
    # 在计算速率之前添验证
    if [ -z "$start_rx_bytes" ] || [ -z "$end_rx_bytes" ] || \
       [ -z "$start_tx_bytes" ] || [ -z "$end_tx_bytes" ] || \
       [ -z "$start_rx_packets" ] || [ -z "$end_rx_packets" ] || \
       [ -z "$start_tx_packets" ] || [ -z "$end_tx_packets" ]; then
        echo "警告: 无法获取完整的网卡统计数据"
        rx_mbps=0
        tx_mbps=0
        rx_pps=0
        tx_pps=0
    else
        # 计算速率
        rx_bytes_diff=$((end_rx_bytes - start_rx_bytes))
        tx_bytes_diff=$((end_tx_bytes - start_tx_bytes))
        rx_packets_diff=$((end_rx_packets - start_rx_packets))
        tx_packets_diff=$((end_tx_packets - start_tx_packets))
        
        rx_mbps=$(echo "scale=2; ($rx_bytes_diff * 8) / ($TEST_DURATION * 1000000)" | bc)
        tx_mbps=$(echo "scale=2; ($tx_bytes_diff * 8) / ($TEST_DURATION * 1000000)" | bc)
        rx_pps=$(echo "scale=2; $rx_packets_diff / $TEST_DURATION" | bc)
        tx_pps=$(echo "scale=2; $tx_packets_diff / $TEST_DURATION" | bc)
    fi
    
    # 保存iperf3结果（带宽和PPS）
    echo "${total_mbits}|${pps:-0}" > "test_results/${test_name}_${packet_size}_iperf.txt"
    
    # 保存NIC结果（接收带宽|发送带宽|接收PPS|发送PPS）
    echo "${rx_mbps}|${tx_mbps}|${rx_pps:-0}|${tx_pps:-0}" > "test_results/${test_name}_${packet_size}_nic.txt"
    
    echo -e "${GREEN}测试完成${NC}"
    echo "iperf3 带宽: ${total_mbits} Mbps"
    echo "NIC 接收带宽: ${rx_mbps} Mbps"
    echo "NIC 发送带宽: ${tx_mbps} Mbps"
    if [ "$packet_size" = "$PPS_PACKET" ]; then
        echo "iperf3 PPS: ${pps}"
        echo "NIC 接收 PPS: ${rx_pps}"
        echo "NIC 发送 PPS: ${tx_pps}"
    fi
    
    # 等待测试完成后，收集CPU统计
    wait $MONITOR_PID
    
    ssh ${SSH_USER}@${SERVER_IP} "
        # 记录结束软中断值
        grep NET_RX /proc/softirqs > ${XDP_PATH}/test_results/${test_name}_softirqs_end.txt
        
        # 处理CPU数据
        awk '
            /^Average:.*all/ {
                cpu_total = 100-\$NF     # idle在最后一列
                cpu_user = \$3           # usr在第3列
                cpu_sys = \$5            # sys在第5列
                cpu_soft = \$7           # soft在第7列
                printf \"%.2f %.2f %.2f %.2f\", cpu_total, cpu_user, cpu_sys, cpu_soft
            }
        ' ${XDP_PATH}/test_results/${test_name}_mpstat.txt > ${XDP_PATH}/test_results/${test_name}_cpu_stats.txt
        
        # 处理软中断数据
        start_total=\$(awk '{sum=0; for(i=2;i<=NF;i++) sum+=\$i; print sum}' ${XDP_PATH}/test_results/${test_name}_softirqs_start.txt)
        end_total=\$(awk '{sum=0; for(i=2;i<=NF;i++) sum+=\$i; print sum}' ${XDP_PATH}/test_results/${test_name}_softirqs_end.txt)
        
        if [ ! -z \"\$start_total\" ] && [ ! -z \"\$end_total\" ]; then
            irq_per_sec=\$(( (end_total - start_total) / ${TEST_DURATION} ))
            echo -n \" \$irq_per_sec\" >> ${XDP_PATH}/test_results/${test_name}_cpu_stats.txt
        else
            echo -n \" 0\" >> ${XDP_PATH}/test_results/${test_name}_cpu_stats.txt
        fi
        
        # 确保文件存在且有正确的权限
        chmod 644 ${XDP_PATH}/test_results/${test_name}_cpu_stats.txt
    "
    
    # 复制CPU统计结果到本地
    scp -q ${SSH_USER}@${SERVER_IP}:${XDP_PATH}/test_results/${test_name}_cpu_stats.txt \
        test_results/ || echo "0.00 0.00 0.00 0.00 0" > "test_results/${test_name}_cpu_stats.txt"
    
    # 验证文件内容
    if [ ! -s "test_results/${test_name}_cpu_stats.txt" ]; then
        echo "0.00 0.00 0.00 0.00 0" > "test_results/${test_name}_cpu_stats.txt"
    fi
}


# 收集诊断信息
collect_diagnostics() {
    echo -e "${BLUE}[*] 收集诊断信息...${NC}"
    
    ssh ${SSH_USER}@${SERVER_IP} "
        {
            echo '=== 详细网卡统计 ==='
            ethtool -S eth0
            
            echo '=== 网卡队列状态 ==='
            for q in /sys/class/net/eth0/queues/rx-*; do
                echo \"Queue \$(basename \$q)\"
                cat \$q/rps_cpus 2>/dev/null || echo 'N/A'
                cat \$q/rps_flow_cnt 2>/dev/null || echo 'N/A'
            done
            
            echo '=== 网卡硬件特性 ==='
            ethtool -k eth0
        } > ${XDP_PATH}/test_results/nic_stats.txt
    "
}

# 获取CPU统计数据
fetch_cpu_stats() {
    echo -e "${BLUE}[*] 获取CPU统计数据...${NC}"
    
    # 确保本地目录存在
    mkdir -p test_results
    
    # 添加白名单场景
    for scenario in "no_xdp" "with_xdp" "with_xdp_blacklist" "with_xdp_whitelist"; do
        # 复制CPU统计文件
        scp -q ${SSH_USER}@${SERVER_IP}:${XDP_PATH}/test_results/${scenario}_cpu_stats.txt \
            test_results/ 2>/dev/null || echo "0 0" > "test_results/${scenario}_cpu_stats.txt"
        
        # 验证文件内容
        if [ ! -s "test_results/${scenario}_cpu_stats.txt" ]; then
            echo "0 0" > "test_results/${scenario}_cpu_stats.txt"
        fi
        
        # 验证数据格式
        if ! awk '{if(NF!=2 || $1!~/^[0-9.]+$/ || $2!~/^[0-9.]+$/){exit 1}}' \
            "test_results/${scenario}_cpu_stats.txt"; then
            echo "0 0" > "test_results/${scenario}_cpu_stats.txt"
        fi
    done
}

# 添加get_value函数定义
get_value() {
    local scenario=$1
    local metric=$2
    local value="N/A"
    
    case "$metric" in
        "small_iperf")
            if [ -f "test_results/${scenario}_${SMALL_PACKET}_iperf.txt" ]; then
                value=$(cut -d'|' -f1 "test_results/${scenario}_${SMALL_PACKET}_iperf.txt")
            fi
            ;;
        "small_nic_rx")
            if [ -f "test_results/${scenario}_${SMALL_PACKET}_nic.txt" ]; then
                value=$(cut -d'|' -f1 "test_results/${scenario}_${SMALL_PACKET}_nic.txt")
            fi
            ;;
        "small_nic_tx")
            if [ -f "test_results/${scenario}_${SMALL_PACKET}_nic.txt" ]; then
                value=$(cut -d'|' -f2 "test_results/${scenario}_${SMALL_PACKET}_nic.txt")
            fi
            ;;
        "large_iperf")
            if [ -f "test_results/${scenario}_${LARGE_PACKET}_iperf.txt" ]; then
                value=$(cut -d'|' -f1 "test_results/${scenario}_${LARGE_PACKET}_iperf.txt")
            fi
            ;;
        "large_nic_rx")
            if [ -f "test_results/${scenario}_${LARGE_PACKET}_nic.txt" ]; then
                value=$(cut -d'|' -f1 "test_results/${scenario}_${LARGE_PACKET}_nic.txt")
            fi
            ;;
        "large_nic_tx")
            if [ -f "test_results/${scenario}_${LARGE_PACKET}_nic.txt" ]; then
                value=$(cut -d'|' -f2 "test_results/${scenario}_${LARGE_PACKET}_nic.txt")
            fi
            ;;
        "pps_iperf")
            if [ -f "test_results/${scenario}_${PPS_PACKET}_iperf.txt" ]; then
                value=$(cut -d'|' -f2 "test_results/${scenario}_${PPS_PACKET}_iperf.txt")
            fi
            ;;
        "pps_nic")
            if [ -f "test_results/${scenario}_${PPS_PACKET}_nic.txt" ]; then
                value=$(cut -d'|' -f3 "test_results/${scenario}_${PPS_PACKET}_nic.txt")
            fi
            ;;
        "latency_min")
            if [ -f "test_results/${scenario}_latency.txt" ]; then
                # 修改最小延迟提取方式
                value=$(grep "rtt min/avg/max" "test_results/${scenario}_latency.txt" | cut -d= -f2 | cut -d/ -f1 | awk '{print $1}')
            fi
            ;;
        "latency_avg")
            if [ -f "test_results/${scenario}_latency.txt" ]; then
                # 修改平均延迟提取方式
                value=$(grep "rtt min/avg/max" "test_results/${scenario}_latency.txt" | cut -d= -f2 | cut -d/ -f2 | awk '{print $1}')
            fi
            ;;
        "latency_max")
            if [ -f "test_results/${scenario}_latency.txt" ]; then
                # 修改最大延迟提取方式
                value=$(grep "rtt min/avg/max" "test_results/${scenario}_latency.txt" | cut -d= -f2 | cut -d/ -f3 | awk '{print $1}')
            fi
            ;;
        "packet_loss")
            if [ -f "test_results/${scenario}_latency.txt" ]; then
                value=$(grep "packet loss" "test_results/${scenario}_latency.txt" | awk '{print $6}' | tr -d '%')
            fi
            ;;
        # 添加CPU相关指标
        "CPU总占用率(%)")
            if [ -f "test_results/${scenario}_cpu_stats.txt" ]; then
                value=$(awk '{print $1}' "test_results/${scenario}_cpu_stats.txt")
            fi
            ;;
        "用户态CPU(%)")
            if [ -f "test_results/${scenario}_cpu_stats.txt" ]; then
                value=$(awk '{print $2}' "test_results/${scenario}_cpu_stats.txt")
            fi  
            ;;
        "系统态CPU(%)")
            if [ -f "test_results/${scenario}_cpu_stats.txt" ]; then
                value=$(awk '{print $3}' "test_results/${scenario}_cpu_stats.txt")
            fi
            ;;
        "软中断CPU(%)")
            if [ -f "test_results/${scenario}_cpu_stats.txt" ]; then
                value=$(awk '{print $4}' "test_results/${scenario}_cpu_stats.txt")
            fi
            ;;
    esac
    
    # 确保返回有效数值
    if [ -z "$value" ] || [ "$value" = "null" ]; then
        value="N/A"
    fi
    
    echo "$value"
}

# 生成测试报告
generate_report() {
    echo -e "\n${BLUE}测试结果：${NC}"
    echo "--------------------------------------------------------------------------------------------------------"
    printf "%-20s %-16s %-16s %-16s %-16s\n" \
        "指标\场景" "无XDP" "启用XDP" "XDP+黑名单" "XDP+白名单"
    echo "--------------------------------------------------------------------------------------------------------"
    
    # 定义要显示的指标
    local metrics=(
        "小包iperf(Mbps)"
        "小包NIC收(Mbps)"
        "小包NIC发(Mbps)"
        "大包iperf(Mbps)"
        "大包NIC收(Mbps)"
        "大包NIC发(Mbps)"
        "PPS(iperf)"
        "PPS(NIC)"
        "最小延迟(ms)"
        "平均延迟(ms)"
        "最大延迟(ms)"
        "丢包率(%)"
        # 添加CPU相关指标
        "CPU总占用率(%)"
        "用户态CPU(%)"
        "系统态CPU(%)"
        "软中断(次/秒)"  # 修改为更准确的描述
    )
    
    # 读取各场景的数据
    for metric in "${metrics[@]}"; do
        printf "%-20s" "$metric"
        
        for scenario in "no_xdp" "with_xdp" "with_xdp_blacklist" "with_xdp_whitelist"; do
            local value="N/A"
            case "$metric" in
                # 保持原有性能指标的case
                "小包iperf(Mbps)") value=$(get_value "$scenario" "small_iperf") ;;
                "小包NIC收(Mbps)") value=$(get_value "$scenario" "small_nic_rx") ;;
                "小包NIC发(Mbps)") value=$(get_value "$scenario" "small_nic_tx") ;;
                "大包iperf(Mbps)") value=$(get_value "$scenario" "large_iperf") ;;
                "大包NIC收(Mbps)") value=$(get_value "$scenario" "large_nic_rx") ;;
                "大包NIC发(Mbps)") value=$(get_value "$scenario" "large_nic_tx") ;;
                "PPS(iperf)") value=$(get_value "$scenario" "pps_iperf") ;;
                "PPS(NIC)") value=$(get_value "$scenario" "pps_nic") ;;
                "最小延迟(ms)") value=$(get_value "$scenario" "latency_min") ;;
                "平均延迟(ms)") value=$(get_value "$scenario" "latency_avg") ;;
                "最大延迟(ms)") value=$(get_value "$scenario" "latency_max") ;;
                "丢包率(%)") value=$(get_value "$scenario" "packet_loss") ;;
                # 添加CPU指标的处理
                "CPU总占用率(%)")
                    if [ -f "test_results/${scenario}_cpu_stats.txt" ]; then
                        value=$(awk '{print $1}' "test_results/${scenario}_cpu_stats.txt")
                    fi
                    ;;
                "用户态CPU(%)")
                    if [ -f "test_results/${scenario}_cpu_stats.txt" ]; then
                        value=$(awk '{print $2}' "test_results/${scenario}_cpu_stats.txt")
                    fi
                    ;;
                "系统态CPU(%)")
                    if [ -f "test_results/${scenario}_cpu_stats.txt" ]; then
                        value=$(awk '{print $3}' "test_results/${scenario}_cpu_stats.txt")
                    fi
                    ;;
                "软中断(次/秒)")
                    if [ -f "test_results/${scenario}_cpu_stats.txt" ]; then
                        value=$(awk '{print $4}' "test_results/${scenario}_cpu_stats.txt")
                    fi
                    ;;
            esac
            
            # 格式化数值输出
            if [ ! -z "$value" ] && [ "$value" != "N/A" ]; then
                printf "%-16.2f" "$value"
            else
                printf "%-16s" "N/A"
            fi
        done
        echo
    done
    echo "--------------------------------------------------------------------------------------------------------"
    
    echo -e "\n${BLUE}测试配置信息：${NC}"
    echo "-------------------------------------------------------------------------------------"
    echo "测试持续时间: ${TEST_DURATION} 秒"
    echo "测试线程数: ${THREADS}"
    echo "PPS测试包大小: ${PPS_PACKET} 字节"
    echo "小包测试包大小: ${SMALL_PACKET} 字节"
    echo "大包测试包大小: ${LARGE_PACKET} 字节"
    echo "延迟测试包大小: ${LATENCY_PACKET} 字节"
    echo "黑名单大小: ${BLACKLIST_SIZE} 条"
    echo "白名单大小: ${WHITELIST_SIZE} 条"
    echo "-------------------------------------------------------------------------------------"
    
    echo -e "\n${BLUE}网络环境信息：${NC}"
    echo "-------------------------------------------------------------------------------------"
    
    # 获取本地网卡信息
    local local_info=$(get_nic_info "local")
    IFS='|' read -r driver version firmware vendor vendor_name vendor_pn <<< "$local_info"
    
    echo "本地网卡信息:"
    echo "  厂商型号: $vendor"
    echo "  驱动: $driver"
    echo "  版本: $version"
    echo "  固件: $firmware"
    echo "  光模块厂商: $vendor_name"
    echo "  光模块型号: $vendor_pn"
    echo "  速率: $(ethtool eth0 2>/dev/null | grep "Speed:" | cut -d: -f2 | tr -d ' ')"
    
    # 获取服务端网卡信息
    local server_info=$(get_nic_info "remote")
    IFS='|' read -r driver version firmware vendor vendor_name vendor_pn <<< "$server_info"
    
    echo -e "\n服务端网卡信息:"
    echo "  厂商型号: $vendor"
    echo "  驱动: $driver"
    echo "  版本: $version"
    echo "  固件: $firmware"
    echo "  光模块厂商: $vendor_name"
    echo "  光模块型号: $vendor_pn"
    echo "  速率: $(ssh ${SSH_USER}@${SERVER_IP} "ethtool eth0" 2>/dev/null | grep "Speed:" | cut -d: -f2 | tr -d ' ')"
    echo "  XDP模式: $(get_xdp_mode "remote")"
    
    # 使用之前获取的XDP模式
    echo -e "\nXDP运行模式: $server_xdp_mode"
    
    # 网卡队列配置
    echo -e "\n${BLUE}网卡队列配置：${NC}"
    echo "本地网卡队列数:"
    ethtool -l eth0 2>/dev/null || echo "  未能获取队列信息"
    
    echo -e "\n服务端网卡队列数:"
    ssh ${SSH_USER}@${SERVER_IP} "ethtool -l eth0" 2>/dev/null || echo "  未能获取队列信息"
    
    echo -e "\n${GREEN}================================================${NC}"
}

analyze_results() {
    local test_name=$1
    local packet_size=$2
    
    # 检查结果文件是否存在
    if [ -f "test_results/${test_name}_${packet_size}_throughput.txt" ]; then
        local throughput=$(cat "test_results/${test_name}_${packet_size}_throughput.txt")
        
        # 只在非PPS测试时显示带宽
        if [ "$packet_size" != "$PPS_PACKET" ]; then
            echo -e "\n${GREEN}带宽测试结果 (包大小: ${packet_size}字节):${NC}"
            echo "带宽: ${throughput} Mbits/sec"
        fi
        
        # 只在PPS测试时显示PPS
        if [ "$packet_size" = "$PPS_PACKET" ] && [ -f "test_results/${test_name}_${packet_size}_pps.txt" ]; then
            local pps=$(cat "test_results/${test_name}_${packet_size}_pps.txt")
            echo -e "\n${GREEN}PPS测试结果 (包大小: ${packet_size}字节):${NC}"
            echo "PPS: ${pps} 包/秒"
        fi
    else
        echo "未找到测试结果文件"
    fi
}

# 添加迟测试函数
test_latency() {
    local test_name=$1
    local description=$2
    
    echo -e "${BLUE}[*] 执行延迟测试: $description${NC}"
    
    # 确保目录存在
    mkdir -p test_results
    
    # 在本地（客户端）执行ping测试到服务器
    echo -e "${BLUE}测试客户端到服务器(${SERVER_IP})的延迟...${NC}"
    
    # 使用ping进行延迟测试（更可靠）
    ping -c ${LATENCY_COUNT} -i 0.1 ${SERVER_IP} > "test_results/${test_name}_latency.txt"
    
    # 显示测试结果
    if [ -f "test_results/${test_name}_latency.txt" ]; then
        echo -e "${GREEN}延迟测试结果:${NC}"
        if grep -q "rtt min/avg/max" "test_results/${test_name}_latency.txt"; then
            grep -E "rtt min/avg/max|packets transmitted|packet loss" "test_results/${test_name}_latency.txt"
        else
            echo -e "${RED}延迟测试未返回有效果${NC}"
            echo "详细输出:"
            cat "test_results/${test_name}_latency.txt"
        fi
    else
        echo -e "${RED}延迟测试结果文件不存在${NC}"
    fi
}

# 主测试流程
main() {
    local selected_stages=""
    
    # 解析命令行参数
    while getopts "s:t:p:d:b:w:m:l:P:h" opt; do
        case $opt in
            s)
                selected_stages=$OPTARG
                ;;
            t)
                THREADS=$OPTARG
                ;;
            p)
                START_PORT=$OPTARG
                ;;
            d)
                TEST_DURATION=$OPTARG
                ;;
            b)
                BLACKLIST_SIZE=$OPTARG
                ;;
            w)
                WHITELIST_SIZE=$OPTARG
                ;;
            m)
                SMALL_PACKET=$OPTARG
                ;;
            l)
                LARGE_PACKET=$OPTARG
                ;;
            P)
                PPS_PACKET=$OPTARG
                ;;
            h)
                show_usage
                exit 0
                ;;
            \?)
                show_usage
                exit 1
                ;;
        esac
    done
    
    # 如果没有指定阶段，运行所有阶段
    if [ -z "$selected_stages" ]; then
        selected_stages=$(seq -s, 1 ${#STAGES[@]})
    fi
    
    # 验证选择的阶段
    if ! validate_stages "$selected_stages"; then
        exit 1
    fi
    
    echo -e "${GREEN}[+] XDP性能测试工具${NC}"
    echo -e "${BLUE}选择的测试阶段:${NC}"
    for stage in $(echo "$selected_stages" | tr ',' ' '); do
        echo "$stage. ${STAGES[$((stage-1))]}"
    done
    echo
    
    # 执行选择的阶段
    for stage in $(echo "$selected_stages" | tr ',' ' '); do
        case $stage in
            1)  # 环境准备
                echo -e "\n${GREEN}=== 阶段1: 环境准备 ===${NC}"
                echo "服务器IP: $SERVER_IP"
                echo -e "${BLUE}[1/4] 检查基础连接...${NC}"
                
                if ! ping -c 1 -W 2 $SERVER_IP >/dev/null 2>&1; then
                    echo -e "${RED}错误: 法连接到服务器 $SERVER_IP${NC}"
                    exit 1
                fi
                
                echo -e "${BLUE}[2/4] 检查SSH连接...${NC}"
                if ! ssh -q -o BatchMode=yes -o ConnectTimeout=5 ${SSH_USER}@${SERVER_IP} "echo OK" >/dev/null 2>&1; then
                    echo -e "${RED}错误: SSH连接失败${NC}"
                    echo "请确保已配SSH免密登录:"
                    echo "ssh-copy-id ${SSH_USER}@${SERVER_IP}"
                    exit 1
                fi
                
                echo -e "${BLUE}[3/4] 准备测试环境...${NC}"
                install_local_dependencies
                setup_local
                setup_server
                
                echo -e "${BLUE}[4/4] 检查网络状态...${NC}"
                check_network
                ;;
                
            2)  # 无XDP基准测试
                echo -e "\n${GREEN}=== 阶段2: 无XDP基准测试 ===${NC}"
                
                echo -e "${BLUE}[1/4] 执行PPS测试...${NC}"
                xdp_operations "unload"
                run_test "no_xdp" $PPS_PACKET "无XDP置 - PPS测试"
                analyze_results "no_xdp" $PPS_PACKET

                echo -e "${BLUE}[2/4] 执行小包测试...${NC}"
                run_test "no_xdp" $SMALL_PACKET "无XDP配置 - 小包测试"
                analyze_results "no_xdp" $SMALL_PACKET

                echo -e "${BLUE}[3/4] 执行大包测试...${NC}"
                run_test "no_xdp" $LARGE_PACKET "无XDP配置 - 大包试"
                analyze_results "no_xdp" $LARGE_PACKET
                
                echo -e "${BLUE}[4/4] 执行延迟测试...${NC}"
                test_latency "no_xdp" "无XDP配置 - 延迟测试"
                ;;
                
            3)  # 启用XDP测试
                echo -e "\n${GREEN}=== 阶段3: 启用XDP测试 ===${NC}"
                
                echo -e "${BLUE}[1/4] 执行PPS测试...${NC}"
                xdp_operations "load"
                run_test "with_xdp" $PPS_PACKET "启用XDP - PPS测试"
                analyze_results "with_xdp" $PPS_PACKET

                echo -e "${BLUE}[2/4] 执行小包试...${NC}"
                run_test "with_xdp" $SMALL_PACKET "启用XDP - 小包测试"
                analyze_results "with_xdp" $SMALL_PACKET

                echo -e "${BLUE}[3/4] 执行大包测试...${NC}"
                run_test "with_xdp" $LARGE_PACKET "启XDP - 大包测试"
                analyze_results "with_xdp" $LARGE_PACKET
                
                echo -e "${BLUE}[4/4] 执行延迟测试...${NC}"
                test_latency "with_xdp" "启用XDP - 延迟测试"
                ;;
                        
            4)  # XDP+黑名单测试
                echo -e "\n${GREEN}=== 阶段4: XDP+黑名单测试 ===${NC}"
                
                echo -e "${BLUE}[1/6] 检查并加载XDP程序...${NC}"
                xdp_operations "load"
                
                echo -e "${BLUE}[2/6] 配置黑名单...${NC}"
                manage_blacklist "add"

                echo -e "${BLUE}[3/6] 执行PPS测试...${NC}"
                run_test "with_xdp_blacklist" $PPS_PACKET "启用XDP并添黑名单 - PPS测试"
                analyze_results "with_xdp_blacklist" $PPS_PACKET

                echo -e "${BLUE}[4/6] 执行小包测试...${NC}"
                run_test "with_xdp_blacklist" $SMALL_PACKET "启用XDP并添加黑名单 - 小包测试"
                analyze_results "with_xdp_blacklist" $SMALL_PACKET

                echo -e "${BLUE}[5/6] 执行大包测试...${NC}"
                run_test "with_xdp_blacklist" $LARGE_PACKET "启用XDP并添加黑名单 - 大包测试"
                analyze_results "with_xdp_blacklist" $LARGE_PACKET
                
                echo -e "${BLUE}[6/6] 执行延迟测试...${NC}"
                test_latency "with_xdp_blacklist" "启用XDP并添加黑名单 - 延迟测试"
                ;;
                
            5)  # XDP+白名单测试
                echo -e "\n${GREEN}=== 阶段5: XDP+白名单测试 ===${NC}"
                
                echo -e "${BLUE}[1/6] 检查并加载XDP程序...${NC}"
                xdp_operations "load"
                
                echo -e "${BLUE}[2/6] 配置白名单...${NC}"
                manage_whitelist "add"

                echo -e "${BLUE}[3/6] 执行PPS测试...${NC}"
                run_test "with_xdp_whitelist" $PPS_PACKET "启用XDP并添加白名单 - PPS测试"
                analyze_results "with_xdp_whitelist" $PPS_PACKET

                echo -e "${BLUE}[4/6] 执行小包测试...${NC}"
                run_test "with_xdp_whitelist" $SMALL_PACKET "启用XDP并添加白名单 - 小包测试"
                analyze_results "with_xdp_whitelist" $SMALL_PACKET

                echo -e "${BLUE}[5/6] 执行大包测试...${NC}"
                run_test "with_xdp_whitelist" $LARGE_PACKET "启用XDP并添加白名单 - 大包测试"
                analyze_results "with_xdp_whitelist" $LARGE_PACKET
                
                echo -e "${BLUE}[6/6] 执行延迟测试...${NC}"
                test_latency "with_xdp_whitelist" "启用XDP并添加白名单 - 延迟测试"
                ;;

            6)  # 收尾工作
                echo -e "\n${GREEN}=== 阶段5: 收尾工作 ===${NC}"
                echo -e "${BLUE}[1/4] 收集诊断信息...${NC}"
                collect_diagnostics
                
                echo -e "${BLUE}[2/4] 清理黑名单...${NC}"
                manage_blacklist "clear"
                
                echo -e "${BLUE}[3/4] 卸载XDP程序...${NC}"
                
                echo -e "${BLUE}[4/4] 生成最终报告...${NC}"
                generate_report
                xdp_operations "unload"
                ;;
        esac
    done

    echo -e "\n${GREEN}[+] 测试完成!${NC}"
}

# 运行主程序（末尾）
main "$@"


get_nic_offload_info() {
    local host=$1
    local cmd="ethtool -k eth0 | grep -E 'generic-receive-offload|generic-segmentation-offload|tx-checksumming|rx-checksumming|scatter-gather'"
    
    if [ "$host" = "local" ]; then
        eval "$cmd"
    else
        ssh ${SSH_USER}@${SERVER_IP} "$cmd"
    fi
}

get_nic_stats() {
    local duration=$1
    # 分别获取接收和发送字节数
    local start_rx=$(ethtool -S eth0 | grep "rx_bytes:" | awk '{print $2}')
    local start_tx=$(ethtool -S eth0 | grep "tx_bytes:" | awk '{print $2}')
    sleep $duration
    local end_rx=$(ethtool -S eth0 | grep "rx_bytes:" | awk '{print $2}')
    local end_tx=$(ethtool -S eth0 | grep "tx_bytes:" | awk '{print $2}')
    
    # 分别计算接收和发送速率
    local rx_diff=$((end_rx - start_rx))
    local tx_diff=$((end_tx - start_tx))
    
    # 输出格式：rx_mbps|tx_mbps
    echo "scale=2; $rx_diff * 8 / ($duration * 1000000)" | bc
    echo "scale=2; $tx_diff * 8 / ($duration * 1000000)" | bc
}