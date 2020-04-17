all:
	@g++ -o pingcc main.cc ping.cc ping_ipv4.cc ping_ipv6.cc

clean:
	@rm pingcc