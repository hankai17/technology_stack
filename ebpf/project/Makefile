OUTPUT := .output
CLANG ?= clang
LIBBPF_SRC := $(abspath ../third_party/libbpf/src)
BPFTOOL_SRC := $(abspath ../third_party/bpftool/src)
LIBBPF_OBJ := $(abspath $(OUTPUT)/libbpf.a)
BPFTOOL_OUTPUT ?= $(abspath $(OUTPUT)/bpftool)
BPFTOOL ?= $(BPFTOOL_OUTPUT)/bootstrap/bpftool

ARCH ?= $(shell uname -m | sed 's/x86_64/x86/' \
                         | sed 's/arm.*/arm/' \
                         | sed 's/aarch64/arm64/' \
                         | sed 's/ppc64le/powerpc/' \
                         | sed 's/mips.*/mips/' \
                         | sed 's/riscv64/riscv/' \
                         | sed 's/loongarch64/loongarch/')

VMLINUX := ../third_party/vmlinux/$(ARCH)/vmlinux.h

INCLUDES := -I$(OUTPUT) -I../third_party/libbpf/include/uapi -I$(dir $(VMLINUX))
CFLAGS := -g -Wall -O1

# ==================== 用户程序和 BPF 程序定义 ====================
APPS = main

# 用户态额外源码
EXTRA_SRCS = config.c cmdline.c
EXTRA_OBJS = $(addprefix $(OUTPUT)/,$(EXTRA_SRCS:.c=.o))

# BPF 程序（新增/修改此处）
BPF_SRCS = xdp_filter.bpf.c xdp_prog_kern.bpf.c
BPF_OBJS = $(addprefix $(OUTPUT)/,$(BPF_SRCS:.c=.o))

# ==================== 工具链设置 ====================
CLANG_BPF_SYS_INCLUDES ?= $(shell $(CLANG) -v -E - </dev/null 2>&1 \
        | sed -n '/<...> search starts here:/,/End of search list./{ s| \(/.*\)|-idirafter \1|p }')

ifeq ($(V),1)
    Q =
    msg =
else
    Q = @
    msg = @printf ' %-8s %s%s\n' "$(1)" "$(patsubst $(abspath $(OUTPUT))/%,%,$(2))" "$(if $(3), $(3))";
    MAKEFLAGS += --no-print-directory
endif

define allow-override
  $(if $(or $(findstring environment,$(origin $(1))),\
            $(findstring command line,$(origin $(1)))),,\
    $(eval $(1) = $(2)))
endef

$(call allow-override,CC,$(CROSS_COMPILE)cc)
$(call allow-override,LD,$(CROSS_COMPILE)ld)

.PHONY: all
all: $(APPS) $(BPF_OBJS)

.PHONY: clean
clean:
	$(call msg,CLEAN)
	$(Q)rm -rf $(OUTPUT) $(APPS)

$(OUTPUT) $(OUTPUT)/libbpf $(BPFTOOL_OUTPUT):
	$(call msg,MKDIR,$@)
	$(Q)mkdir -p $@

# Build libbpf
$(LIBBPF_OBJ): $(wildcard $(LIBBPF_SRC)/*.[ch] $(LIBBPF_SRC)/Makefile) | $(OUTPUT)/libbpf
	$(call msg,LIB,$@)
	$(Q)$(MAKE) -C $(LIBBPF_SRC) BUILD_STATIC_ONLY=1 \
	            OBJDIR=$(dir $@)/libbpf DESTDIR=$(dir $@) \
	            INCLUDEDIR= LIBDIR= UAPIDIR= install

# Build bpftool
$(BPFTOOL): | $(BPFTOOL_OUTPUT)
	$(call msg,BPFTOOL,$@)
	$(Q)$(MAKE) ARCH= CROSS_COMPILE= OUTPUT=$(BPFTOOL_OUTPUT)/ -C $(BPFTOOL_SRC) bootstrap

# ===================== BPF 编译规则 =====================
$(OUTPUT)/%.bpf.o: %.bpf.c $(VMLINUX) | $(OUTPUT) $(BPFTOOL) $(LIBBPF_OBJ)
	$(call msg,BPF,$@)
	$(Q)$(CLANG) -g -O2 -target bpf -D__TARGET_ARCH_$(ARCH) \
	             $(INCLUDES) $(CLANG_BPF_SYS_INCLUDES) \
	             -c $< -o $(patsubst %.bpf.o,%.tmp.bpf.o,$@)
	$(Q)$(BPFTOOL) gen object $@ $(patsubst %.bpf.o,%.tmp.bpf.o,$@)

# 生成 skeleton
$(OUTPUT)/%.skel.h: $(OUTPUT)/%.bpf.o | $(OUTPUT) $(BPFTOOL)
	$(call msg,GEN-SKEL,$@)
	$(Q)$(BPFTOOL) gen skeleton $< > $@

# ===================== 用户态编译规则 =====================
# xdp 用户程序依赖两个 skeleton
$(OUTPUT)/main.o: $(OUTPUT)/xdp_filter.skel.h $(OUTPUT)/xdp_prog_kern.skel.h main.c | $(OUTPUT)
	$(call msg,CC,$@)
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) -c main.c -o $@

$(OUTPUT)/%.o: %.c | $(OUTPUT)
	$(call msg,CC,$@)
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# 链接最终二进制
$(APPS): %: $(OUTPUT)/%.o $(EXTRA_OBJS) $(LIBBPF_OBJ) | $(OUTPUT)
	$(call msg,BINARY,$@)
	$(Q)$(CC) $(CFLAGS) $^ $(LDFLAGS) -lelf -lz -lconfig -o $@

.DELETE_ON_ERROR:
.SECONDARY:
