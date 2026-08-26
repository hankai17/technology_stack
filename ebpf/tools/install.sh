#!/bin/bash

set -e
set -x

INSTALL_DIR="/Application/KEDGE_DDoS/xdp_toolkit"

# 启用 devtoolset-9
export PATH="/opt/rh/devtoolset-9/root/usr/bin:$PATH"
export LD_LIBRARY_PATH="/opt/rh/devtoolset-9/root/usr/lib64:/opt/rh/devtoolset-9/root/usr/lib:$LD_LIBRARY_PATH"

# 检查 GCC 版本
gcc_version=$(gcc --version | awk '/gcc/ {print $3}')
echo "Current GCC version: $gcc_version"

install_dependencies() {
    # 检查并安装 devtoolset-9（如果还没有安装）
    if ! yum list installed | grep -q devtoolset-9; then
        echo "Installing devtoolset-9..."
        sudo yum install -y centos-release-scl
        sudo yum install -y devtoolset-9
    fi

    if ! command -v cmake3 &> /dev/null; then
        echo "Installing cmake3..."
        cd $INSTALL_DIR/cmake-3.20.0
        ./bootstrap --prefix=/usr/local
        make -j$(nproc)
        sudo make install
        sudo ln -sf /usr/local/bin/cmake /usr/local/bin/cmake3
        cd $INSTALL_DIR
    fi
}





setup_kernel_headers() {
    echo "Setting up kernel headers..."
    KERNEL_HEADERS_RPM="kernel-headers-4.18.0-305.12.2.el7.x86_64.rpm"
    KERNEL_HEADERS_DIR="$INSTALL_DIR/kernel-headers"

    if [ ! -f "$INSTALL_DIR/$KERNEL_HEADERS_RPM" ]; then
        echo "Error: $KERNEL_HEADERS_RPM not found in $INSTALL_DIR"
        exit 1
    fi

    if [ ! -d "$KERNEL_HEADERS_DIR" ]; then
        mkdir -p "$KERNEL_HEADERS_DIR" && \
        cd "$KERNEL_HEADERS_DIR" && \
        rpm2cpio "$INSTALL_DIR/$KERNEL_HEADERS_RPM" | cpio -idmv && \
        cd "$INSTALL_DIR"
        
        if [ $? -ne 0 ]; then
            echo "Error: Failed to extract kernel headers"
            rm -rf "$KERNEL_HEADERS_DIR"
            exit 1
        fi
        echo "Kernel headers extracted to $KERNEL_HEADERS_DIR"
    else
        echo "Kernel headers directory already exists"
    fi

    if [ ! -d "$KERNEL_HEADERS_DIR/usr/include" ]; then
        echo "Error: Expected directory structure not found in extracted kernel headers"
        exit 1
    fi
}


compile_llvm() {
    echo "Compiling and installing LLVM/Clang..."
    rm -rf $INSTALL_DIR/llvm
    cd $INSTALL_DIR/llvm-project
    rm -rf build
    mkdir -p build
    cd build

    cmake3 -DLLVM_ENABLE_PROJECTS="clang;lld" \
        -DCMAKE_BUILD_TYPE=Release \
        -DLLVM_TARGETS_TO_BUILD="BPF;X86" \
        -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR/llvm \
        -DLLVM_USE_LINKER=gold \
        -DCMAKE_C_FLAGS="-m64" \
        -DCMAKE_CXX_FLAGS="-m64" \
        ../llvm

    make -j$(nproc)
    make install
    cd $INSTALL_DIR
}


compile_libbpf() {
    echo "Compiling and installing libbpf..."
    rm -rf $INSTALL_DIR/libbpf/build
    cd $INSTALL_DIR/libbpf/src
    make clean
    make
    make install PREFIX=$INSTALL_DIR/libbpf
    cd $INSTALL_DIR
}

compile_bpftool() {
    echo "Compiling and installing bpftool..."
    rm -rf $INSTALL_DIR/bpftool/build
    cd $INSTALL_DIR/bpftool/src
    make clean
    make
    make install prefix=$INSTALL_DIR/bpftool
    cd $INSTALL_DIR
}

compile_iproute2() {
    echo "Compiling and installing iproute2..."
    cd $INSTALL_DIR/iproute2
    make clean
    ./configure
    make
    make install DESTDIR=$INSTALL_DIR/iproute2
    cd $INSTALL_DIR
}

update_deploy_script() {
    echo "Updating XDP deployment script..."
    cat > $INSTALL_DIR/deploy_xdp.sh << EOL
#!/bin/bash

set -e
set -x

export PATH="$INSTALL_DIR/llvm/bin:$INSTALL_DIR/bpftool/sbin:$INSTALL_DIR/iproute2/sbin:$PATH"
export LD_LIBRARY_PATH="$INSTALL_DIR/llvm/lib:$INSTALL_DIR/libbpf/lib64:$LD_LIBRARY_PATH"

$INSTALL_DIR/llvm/bin/clang -O2 -g -target bpf -D__KERNEL__ -D__BPF_TRACING__ \
    -I$INSTALL_DIR/kernel-headers/usr/include \
    -I$INSTALL_DIR/libbpf/include \
    -I$INSTALL_DIR/libbpf/include/uapi \
    -nostdinc \
    -isystem $INSTALL_DIR/llvm/lib/clang/14.0.0/include \
    -c xdp_filter.c -o xdp_filter.o

sudo rm -f /sys/fs/bpf/xdp_filter

sudo $INSTALL_DIR/bpftool/sbin/bpftool net detach xdp dev eth0

sudo $INSTALL_DIR/bpftool/sbin/bpftool prog load xdp_filter.o /sys/fs/bpf/xdp_filter type xdp

sudo $INSTALL_DIR/bpftool/sbin/bpftool net attach xdp pinned /sys/fs/bpf/xdp_filter dev eth0

sudo $INSTALL_DIR/bpftool/sbin/bpftool net show dev eth0

sudo $INSTALL_DIR/bpftool/sbin/bpftool map dump name ssh_port
EOL

    chmod +x $INSTALL_DIR/deploy_xdp.sh
}

main_install() {
    install_dependencies
    setup_kernel_headers
    compile_llvm
    compile_libbpf
    compile_bpftool
    compile_iproute2
    update_deploy_script

    echo "Installation completed. To deploy the XDP program, run:"
    echo "$INSTALL_DIR/deploy_xdp.sh"
}

main_install
