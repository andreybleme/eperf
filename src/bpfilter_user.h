#ifndef BPFILTER_H
#define BPFILTER_H

#include <arpa/inet.h>

/* bpfilter public variables */
struct bpfilter_meta
{
  /* Interface Index */
  int if_index;

  /* eBPF program file descriptor */
  int prog_fd;
};

/* Create a public type for struct */
typedef struct bpfilter_meta bpfilter_t;

/*
 * Public functions of bpfilter module
 */
void bpfilter_sigint_callback (int signal);
void bpfilter_sigterm_callback (int signal);
void bpfilter_get_interface (bpfilter_t *bpfilter, char if_name[]);
void bpfilter_read_object (bpfilter_t *bpfilter, char bpf_prog_path[]);
void bpfilter_load (bpfilter_t *bpfilter);

#endif
