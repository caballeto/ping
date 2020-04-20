//
// Created by vladyslav on 17.04.20.
//

#include "ping.h"

Ping::Ping(const std::string& ip_addr)
  : ip_addr_(ip_addr)
{ }

Ping::~Ping() { }

void Ping::StartPing() {
  clock_gettime(CLOCK_MONOTONIC, &ping_start_);
  while (run_ping_) SendPacket();
  clock_gettime(CLOCK_MONOTONIC, &ping_end_);
  Stats();
}

void Ping::Stats() {
  double ms = ((double) (ping_end_.tv_nsec - ping_start_.tv_nsec)) / 1000000.0;
  double total_run_time = (ping_end_.tv_sec - ping_start_.tv_sec) * 1000.0 + ms;

  std::cout << std::endl << "--- " << ip_addr_ << " ping statistics ---" << std::endl;
  std::cout << total_sent_ << " transmitted, " << total_received_ << " received, "
            << ((total_sent_ - total_received_) / (double) total_sent_) * 100 << "% packet loss, running time "
            << total_run_time << "ms" << std::endl;
}


void Ping::FlipRunPing() {
  run_ping_ = !run_ping_;
}

int32_t Ping::Checksum(uint16_t *buf, int32_t len) {
  uint16_t answer = 0;
  uint32_t sum = 0;

  while (len > 1)  {
    sum += *buf++;
    len -= 2;
  }

  if (len == 1) {
    sum += *(uint16_t*)buf;
  }

  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  answer = ~sum;
  return answer;
}
