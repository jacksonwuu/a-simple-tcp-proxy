# A very simple TCP proxy

The TCP proxy will listen on a port.

The proxy will create a connection to upstream server for every client connection.

When client send something to proxy, the proxy will create a thread to it (forward everything to upstream server).

That's it.

You can learn the basic mechanism of tcp proxy here.


## Quick start

build:

```shell
make proxy
make server
```

run:

```shell
./server
```

```shell
./proxy
```

in different terminal windows.


You can use `nc` to simulate a tcp client.

```shell
nc localhost 12345
```

`nc` will establish a connection to `proxy`, and proxy will forward everything to `server` (it will print all data it received to terminal).