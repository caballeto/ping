//
// Created by vladyslav on 17.04.20.
//

#ifndef PING__PING_OOP_H_
#define PING__PING_OOP_H_

#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SLEEP_TIME 500000
#define PACKET_SIZE 64

#define ICMP_ECHO_REPLY 0
#define ICMP_DEST_UNREACHABLE 3
#define ICMP_ECHO_REQUEST 8
#define ICMP_TTL_EXCEEDED 11

struct icmp_header_t {
  uint8_t type;
  uint8_t code;
  uint16_t checksum;
  uint16_t id;
  uint16_t icmp_seq;
  uint32_t data;
};

struct icmp_response_t {
  uint8_t type;
  uint8_t code;
  uint16_t checksum;
  uint16_t id;
  uint16_t icmp_seq;
};

class Ping {
 public:
  Ping(const std::string& ip_addr);

  virtual int SendPacket() = 0;
  virtual void HandleResponse() = 0;

  virtual ~Ping() = 0;

  void StartPing();
  void Stats();
  void FlipRunPing();

 protected:
  int32_t Checksum(uint16_t* buf, int32_t len);

  const std::string& ip_addr_;
  int ttl_;

  int socket_;
  timespec ping_start_;
  timespec ping_end_;
  timespec pack_send_start_;
  timespec pack_send_end_;

  int total_sent_;
  int total_received_;

  bool run_ping_;
};

#endif //PING__PING_OOP_H_
