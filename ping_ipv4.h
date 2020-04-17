//
// Created by vladyslav on 17.04.20.
//

#ifndef PING_SRC_PING_IPV4_H_
#define PING_SRC_PING_IPV4_H_

#include "ping.h"

class PingIpV4 : public Ping {
 public:
  PingIpV4(const std::string& ip_addr, int ttl = 56);

  int SendPacket() override;
  void HandleResponse() override;
};

#endif //PING_SRC_PING_IPV4_H_
