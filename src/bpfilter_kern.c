/* Linux headers for bpf and networking */
#include <linux/bpf.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/udp.h>

/* libbpf header files */
#include "bpf/bpf_helpers.h"
#include "bpf/bpf_endian.h"

/* Map that controls connections counter by source IP */
struct bpf_map_def SEC("maps") counters = {

    .type = BPF_MAP_TYPE_HASH,

    /* A 32 bits representing the source IPv4 address */
    .key_size = sizeof(int),

    /* A integer counter of the connections */
    .value_size = sizeof(int),

    .max_entries = 24,
};

SEC("bpfilter")
int handle_tp(struct xdp_md *ctx)
{
    __u64 begin_in_ns = bpf_ktime_get_ns();
    bpf_printk("\n[XDP] starting xdp ip filter\n");

    void *data_end = (void *)(long)ctx->data_end;
    void *data     = (void *)(long)ctx->data;
    struct ethhdr *eth = data;

    // check packet size
    if (eth + 1 > data_end) {
        return XDP_PASS;
    }

    // get the source address of the packet
    struct iphdr *iph = data + sizeof(struct ethhdr);
    if (iph + 1 > data_end) {
        return XDP_PASS;
    }

    __u32 ip_src = iph->saddr;
    bpf_printk("[XDP] source ip: %u", ip_src);

    __u32 ip_dst = iph->daddr;
    bpf_printk("[XDP] destination ip: %u", ip_dst);

	int pkt_sz = data_end - data;
	bpf_printk("[XDP] packet size: %d", pkt_sz);

    int key = 1337;
    int value = 1234;
    if (1 == 1) {
        if(bpf_map_update_elem(&counters, &key, &value, BPF_ANY) == -1) {
            bpf_printk("[XDP] map insert error");
        } else {
            bpf_printk("[XDP] map insert success");
        }
    }

    __u64 end_in_ns = bpf_ktime_get_ns();
    __u64 elapsed_us = (end_in_ns - begin_in_ns) / 1000;
    bpf_printk(
        "[XDP] BlockDuration=%llu, Begin=%llu, End=%llu\n",
        elapsed_us, begin_in_ns, end_in_ns
    );

    return XDP_PASS;
}


char _license[] SEC("license") = "GPL";
