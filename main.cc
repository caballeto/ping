#include <iostream>
#include <functional>
#include <arpa/inet.h>
#include <netdb.h>

#include "ping.h"
#include "ping_ipv4.h"
#include "ping_ipv6.h"

std::function<void()> sigint_handler;

void signal_handler(int x) {
  sigint_handler();
}

void Usage() {
  std::cout << "Usage:" << std::endl;
  std::cout << "\tsudo ./pingcc <hostname> [ttl]" << std::endl;
  std::cout << "\tsudo ./pingcc <ipv4> [ttl]" << std::endl;
  std::cout << "\tsudo ./pingcc <ipv> [ttl]" << std::endl;
}

bool IsIpV4(const std::string& s) {
  sockaddr_in sa;
  return inet_pton(AF_INET, s.c_str(), &sa.sin_addr) != 0;
}

bool IsIpV6(const std::string& s) {
  sockaddr_in6 sa;
  return inet_pton(AF_INET6, s.c_str(), &sa.sin6_addr);
}

std::string GetIpFromHostName(const std::string& s) {
  struct hostent* host = gethostbyname(s.c_str());
  if (host == nullptr) {
    std::cerr << "Invalid host name '" << s << "'" << std::endl;
    exit(1);
  }

  return std::string(inet_ntoa(*((in_addr*) host->h_addr_list[0])));
}

int main(int argc, char* argv[]) {
  if (argc < 2 || argc > 3) {
    Usage();
    exit(1);
  }

  std::string ip = std::string(argv[1]);
  int ttl = 56;
  if (argc >= 3){
    ttl = atoi(argv[2]);
    ttl = (ttl <= 0 || ttl >= 250) ? 55 : ttl;
  }

  Ping* ping;
  if (IsIpV4(ip)) {
    ping = new PingIpV4(ip, ttl);
  } else if (IsIpV6(ip)) {
    ping = new PingIpV6(ip, ttl);
  } else {
    ip = GetIpFromHostName(ip);
    ping = new PingIpV4(ip, ttl);
  }

  std::cout << "PING " << ip << " 56(84) bytes of data." << std::endl;
  sigint_handler = [&]() { ping->FlipRunPing(); };
  signal(SIGINT, signal_handler);
  ping->StartPing();
  return 0;
}
