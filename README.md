# Ping

This is a simple implementation of a subset of ping command.
Following features are implemented:

- lookup by ipv4 address
- lookup by host name
- setting ttl value
- reporting round trip time, ttl value, icmp sequence on each message
- reporting total packets, total received, percentage loss on exit

**Extra**

The IpV6 implementation is not tested. It is somewhere between the lines,
but no IpV6 interface on my PC, so can't test it.

## Install

```
git clone https://github.com/caballeto/ping
cd ping/
make
sudo ./pingcc <ip> [ttl] 
```

## Usage

[Simple lookup](./images/use-1.png)

[Set ttl](./images/use-ttl.png)
