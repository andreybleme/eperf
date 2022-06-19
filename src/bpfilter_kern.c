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
    .key_size = sizeof(__be32),

    /* A integer counter of the connections */
    .value_size = sizeof(int),

    .max_entries = 10,
};

SEC("bpfilter")
int handle_tp(struct xdp_md *ctx)
{
  bpf_printk("starting xdp ip filter\n");

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
  bpf_printk("source ip: %u\n", ip_src);

  __u32 ip_dst = iph->daddr;
  bpf_printk("destination ip is: %u\n", ip_dst);

	int pkt_sz = data_end - data;
	bpf_printk("[XDP] packet size: %d", pkt_sz);

  return XDP_PASS;
}



char _license[] SEC("license") = "GPL";