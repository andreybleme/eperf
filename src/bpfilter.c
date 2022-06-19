#include <stdlib.h>
#include <signal.h>
#include "bpfilter_user.h"

int main(int argc, char **argv)
{
  bpfilter_t bpfilter;

  bpfilter_get_interface(&bpfilter, "eth0");
  bpfilter_read_object(&bpfilter, "/src/bpfilter/bin/bpfilter_kern.o");

  signal(SIGINT, bpfilter_sigint_callback);
  signal(SIGTERM, bpfilter_sigterm_callback);

  bpfilter_load(&bpfilter);

  return EXIT_SUCCESS;
}
