# bpfilter

Simple eBPF + XDP tool to filter network packets.

-----

1. Build the container:

```
$ docker build . -t bpfilter:1.0.0
```

2. Exec the container in privileged mode:

```
$ docker run -it --privileged bpfilter:1.0.0
```

3. Mount the debugfs:

```
$ mount -t debugfs none /sys/kernel/debug
```