OPTMIZATIONS := -O2

# C compilation flags
CFLAGS := -DDEBUG -D__BPF_TRACING__ -D__KERNEL__
CFLAGS += $(OPTMIZATIONS)

# C compiler
CC := $(shell which clang)

# Kernel module inside container
INCLUDE_PATH := /usr/include/x86_64-linux-gnu/

# Source code directory
SRC := src

# Binary directory
BIN := bin

# Logs directory
LOGS := logs

# Log suffixes used to name files
LOG_SUFFIX ?= baseline
KERNEL_LOG_SUFFIX ?= kernel

# Kernel trace file which eBPF prints debug messages
TRACE_PIPE := /sys/kernel/debug/tracing/trace_pipe

# Network interface to attach eBPF program
IFACE ?= eth0


#
# ==============
# Target rules
#
.DEFAULT_GOAL: help

.PHONY: help
help: welcome
	@echo "--- Target rules ---"
	@echo
	@echo " compile             Compiles kernel and userspace program"
	@echo " kernel              Compiles kernel program"
	@echo " userspace           Compiles user space program"
	@echo " library             Builds bpfilter as a shared library"
	@echo " clean               Remove all files from compilation process"
	@echo " clean_logs          Remove all log files from log dir"
	@echo " load                Loads the eBPF program into interface"
	@echo " unload              Unloads the eBPF program from interface"
	@echo " debug               Tails the eBPF program logs (trace_pipe)"
	@echo " show                Shows the network interface link state"
	@echo " bpf_dev             Runs all targets to build load and debug eBPF"
	@echo " bpf                 Runs all targets to build load eBPF program"
	@echo


.PHONY: welcome
welcome:
	@echo "     ______          __ "
	@echo "     | ___ \        / _|"
	@echo "  ___| |_/ /__ _ __| |_ "
	@echo " / _ \  __/ _ \ '__|  _|"
	@echo "|  __/ | |  __/ |  | |  "
	@echo " \___\_|  \___|_|  |_|  "


#
# Targets to run XDP related tasks
#
.PHONY: compile
compile: welcome kernel userspace


.PHONY: kernel
kernel: $(SRC)/bpfilter_kern.c
	$(info Compiling eBPF kernel program)
	$(CC) -target bpf -c $< -o $(BIN)/bpfilter_kern.o -I $(INCLUDE_PATH) $(CFLAGS)


.PHONY: userspace
userspace: library
	$(info Compiling bpfilter userspace program standalone program)
	$(CC) $(SRC)/bpfilter.c $(BIN)/bpfilter_user.o -O2 -lbpf -lelf -lz -o $(BIN)/bpfilter


.PHONY: library
library: $(SRC)/bpfilter_user.c
	$(info Compiling bpfilter userspace as static library)
	gcc -c $< -fPIE -O2 -o $(BIN)/bpfilter_user.o


.PHONY: clean
clean:
	@rm -rvf $(BIN)/bpfilter $(wildcard $(BIN)/*.o)


.PHONY: clean_logs
clean_logs:
	@rm -rvf $(LOGS)/*.log


.PHONY: load
load:
	$(info Loading eBPF program on interface $(IFACE))
	ip link set dev $(IFACE) xdp obj $(BIN)/bpfilter_kern.o sec bpfilter


.PHONY: unload
unload:
	$(info Unloading eBPF program from interface $(IFACE))
	ip link set dev $(IFACE) xdp off


.PHONY: debug
debug:
	$(info Entering debug mode)
	@cat $(TRACE_PIPE) | tee $(LOGS)/$(shell date +%s)-$(KERNEL_LOG_SUFFIX).log


.PHONY: show
show:
	@ip link show dev $(IFACE)


.PHONY: bpf_dev
bpf_dev: welcome unload compile load debug


.PHONY: bpf
bpf: welcome unload compile load
