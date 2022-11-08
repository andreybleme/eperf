# bpfilter

A plain simple eBPF + XDP program to process network packets.

This dummy application listen for incoming TCP packets printing its source and destination IP, and pkt size.

There is also a map insert/update operation that changes a BPF_MAP for every incoming pkt (see [bpfilter_kern.c](https://github.com/andreybleme/eperf/blob/master/src/bpfilter_kern.c) file). It works as a simple demo for BPF map operations, highlighting a few performance aspects of eBPF when we share data between kernel and user space.

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

----

Sending packages from an external container:

First create the network:
```
$ docker network create bpfilter
````

Attach the containers into the network using the cname or cid:
```
$ docker network connect bpfilter bcbf3d78c0ca
$ docker network connect bpfilter da460234fb42
```

After connecting to the docker network, there is a new IFACE: eth2.
It might be necessary to compile the eBPF program again attaching the XDP program into the new IFACE.

From within the client container, use bombardier to send 10 pkts:

```
/go/pkg/mod/github.com/codesenberg/bombardier@v1.2.5# go run . -c 1 -n 10 172.18.0.3
```

Benchmarking tools usually require a server listening to requests coming from a given port.
If you want to see accurate results, you can run a Node server, making sure you're exporting port 8000 during the "docker run" command:


```
$ docker run -it -p 8000:8000 --privileged bpfilter:1.0.0
```
