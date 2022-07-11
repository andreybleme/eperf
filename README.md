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
It might be necessary to compile the eBPF code again attaching the XDP program into the new IFACE.

From within the client container, use bombardier to send 10 reqs:

```
/go/pkg/mod/github.com/codesenberg/bombardier@v1.2.5# go run . -c 1 -n 10 172.18.0.3
```