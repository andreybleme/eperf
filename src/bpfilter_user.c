#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <net/if.h>
#include <arpa/inet.h>

/* Linux OS dependent */
#include <linux/bpf.h>
#include <linux/if_link.h>


/* libbpf included headers */
#include <bpf/bpf.h>
#include <bpf/libbpf.h>

#include "bpfilter_user.h"

/* Network interface index */
static int BPFILTER_IFINDEX;

/* eBPF program ID */
static __u32 BPFILTER_IFINDEX;

static unsigned int BPFILTER_XDP_FLAGS = XDP_FLAGS_UPDATE_IF_NOEXIST;


/**
 * SIGINT - Interruption signal callback
 * It unloads eBPF program from interface
 */
void bpfilter_sigint_callback (int signal)
{
  printf("[bpfilter] Action=exit, Signal=INT\n");

  __u32 prog_id = 0;

  if ( bpf_get_link_xdp_id(BPFILTER_IFINDEX, &prog_id, BPFILTER_XDP_FLAGS) ) {
    printf("[bpfilter] Error=CallError, Type=BPF, Function=bpf_get_link_xdp_id\n");
    exit(EXIT_FAILURE);
  }

  if ( prog_id == BPFILTER_IFINDEX ) {

    bpf_set_link_xdp_fd(BPFILTER_IFINDEX, -1, BPFILTER_XDP_FLAGS);
    printf("[bpfilter] Action=Unload, Type=BPF, InterfaceIndex=%d\n", BPFILTER_IFINDEX);

  } else if (!prog_id) {
    printf("[bpfilter] Error=NotFound, Type=BPF, Message=No program found\n");

  } else {
    printf("[bpfilter] Action=Update, Type=BPF\n");
  }

  exit(EXIT_SUCCESS);
}


/**
 * SIGTERM - Termination signal callback
 * It unloads eBPF program from interface
 */
void bpfilter_sigterm_callback(int signal)
{
  printf("[bpfilter] Action=exit, Signal=TERM\n");

  /* Meanwhile we just call the same behavior as SIGINT */
  bpfilter_sigint_callback(signal);
}

/**
 * Reads the network interface index from
 * a given name
 */
void bpfilter_get_interface (bpfilter_t *bpfilter, char if_name[])
{
  bpfilter->if_index = if_nametoindex(if_name);
  BPFILTER_IFINDEX = bpfilter->if_index;

  if ( !bpfilter->if_index ) {
    printf("[bpfilter] Error=Name to index failed, Type=OS, Interface=%s\n", if_name);
    exit(EXIT_FAILURE);
  }

  printf("[bpfilter] Action=Read, Type=OS, Interface=%d\n", bpfilter->if_index);
}


/**
 * Reads the .o object program to be loaded in the
 * kernel. This functions reads all maps and the
 * kernel hook function
 */
void bpfilter_read_object (bpfilter_t *bpfilter, char bpf_prog_path[])
{
  struct bpf_prog_load_attr prog_attr = {
    .prog_type	= BPF_PROG_TYPE_XDP,
    .file       = bpf_prog_path,
  };
  struct bpf_object *obj;

  if ( bpf_prog_load_xattr(&prog_attr, &obj, &bpfilter->prog_fd) ) {
    printf("[bpfilter] Error=Load prog attr failed, Type=BPF\n");
    exit(EXIT_FAILURE);
  }
  printf("[bpfilter] Action=Load, Type=BPF, Object=%s\n", bpf_prog_path);

  if ( !bpfilter->prog_fd ) {
    printf("[bpfilter] Error=%s, Type=BPF, Function=bpf_prog_load_xattr\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}


/**
 * Loads the eBPF kernel program on the linux kernel
 * XDP hook on the network interface
 */
void bpfilter_load (bpfilter_t *bpfilter)
{
  if (bpf_set_link_xdp_fd(bpfilter->if_index, bpfilter->prog_fd, BPFILTER_XDP_FLAGS) < 0) {
    printf("[bpfilter] Error=Link setup failed, Type=BPF, Function=bpf_set_link_xdp_fd\n");
    exit(EXIT_FAILURE);
  }

  struct bpf_prog_info info = {};
  __u32 info_len = sizeof(info);

  if (bpf_obj_get_info_by_fd(bpfilter->prog_fd, &info, &info_len)) {
    printf("[bpfilter] Error=%s, Type=BPF, Function=bpf_obj_get_info_by_fd\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  BPFILTER_IFINDEX = info.id;

  printf("[bpfilter] Action=Setup, Type=BPF, Hook=XDP\n");
}
