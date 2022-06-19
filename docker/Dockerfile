FROM ubuntu:22.10

RUN apt-get update && \
    apt-get install -y build-essential \
        software-properties-common \
        zlib1g-dev libevent-dev \
        git cmake clang llvm vim libelf-dev \
        iputils-ping dnsutils iproute2 tcpdump \
        libc6-dev-i386

RUN mkdir /src && git init

WORKDIR /src

# Mounts kernel tracing debug filesystem for debugging eBPF
# mount -t debugfs none /sys/kernel/debug

# Needed to link asm/byteorder.h into eBPF
RUN ln -s /usr/include/x86_64-linux-gnu/asm/ /usr/include/asm

# Clone and build libbpf as staticlibrary
RUN git clone https://github.com/libbpf/libbpf-bootstrap.git && \
    cd libbpf-bootstrap && \
    git submodule update --init --recursive

RUN cd libbpf-bootstrap/libbpf/src && \
    make BUILD_STATIC_ONLY=y && \
    make install BUILD_STATIC_ONLY=y LIBDIR=/usr/lib/x86_64-linux-gnu/

# Clones the master branch of linux kernel
RUN git clone --depth 1 git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git && \
    # Overrides all bpf headers with the latest from linux kernel source
    cp linux/include/uapi/linux/bpf* /usr/include/linux/
