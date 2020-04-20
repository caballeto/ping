//
// Created by vladyslav on 17.04.20.
//

#include "ping_ipv6.h"

PingIpV6::PingIpV6(const std::string &ip_addr, int ttl) : Ping(ip_addr) {
  this->ttl_ = ttl;
  this->total_sent_ = 0;
  this->total_received_ = 0;
  this->run_ping_ = true;
  socket_ = socket(PF_INET6, SOCK_RAW, 1);
  if (socket_ <= 0) {
    perror("Socket error");
    exit(1);
  }

  int setTTL = setsockopt(socket_, SOL_IPV6, IP_TTL, &ttl_, sizeof(ttl_));
  if (setTTL != 0) {
    perror("Could not set ttl");
    exit(1);
  }
}

int PingIpV6::SendPacket() {
  icmp_header_t icmp_hdr;

  icmp_hdr.type = ICMP_ECHO_REQUEST;
  icmp_hdr.code = 0;
  icmp_hdr.checksum = 0;
  icmp_hdr.id = getpid();
  icmp_hdr.icmp_seq = total_sent_++;
  icmp_hdr.data = 0;

  icmp_hdr.checksum = Checksum((uint16_t *) &icmp_hdr, sizeof(icmp_hdr));

  sockaddr_in6 ip_hdr;

  ip_hdr.sin6_family = AF_INET6;
  ip_hdr.sin6_port = 0; // automatic port selection
  inet_pton(AF_INET6, ip_addr_.c_str(), &ip_hdr.sin6_addr);

  usleep(SLEEP_TIME);

  clock_gettime(CLOCK_MONOTONIC, &pack_send_start_);
  int result = sendto(socket_, &icmp_hdr, sizeof(icmp_hdr), 0, (sockaddr*) &ip_hdr, sizeof(ip_hdr));
  if (result < 0) {
    perror("Could not send packet");
    exit(1);
  }

  HandleResponse();
  return 0;
}

void PingIpV6::HandleResponse() {
  unsigned char res[30] = "";
  sockaddr_in6 resAddr;
  socklen_t len6 = sizeof(resAddr);

  RECEIVE_PACKET:
  int response = recvfrom(socket_, res, sizeof(res), 0, (sockaddr*) &resAddr, &len6);

  if (response > 0) {
    clock_gettime(CLOCK_MONOTONIC, &pack_send_end_);
    icmp_response_t* icmp_response = (icmp_response_t*) &res[20];

    double ms = ((double) (pack_send_end_.tv_nsec - pack_send_start_.tv_nsec)) / 1000000.0;
    double rtt = (pack_send_end_.tv_sec - pack_send_start_.tv_sec) * 1000 + ms;

    if (icmp_response->type == ICMP_DEST_UNREACHABLE) {
      std::cout << "Reply from " << ip_addr_ << ": Destination host unreachable" << std::endl;
    } else if (icmp_response->type == ICMP_TTL_EXCEEDED) {
      std::cout << "Reply from " << ip_addr_ << ": Time to live exceeded" << std::endl;
    } else if (icmp_response->type == ICMP_ECHO_REQUEST) { // in case of requests to this computer (e.g. localhost)
      goto RECEIVE_PACKET;
    } else if (icmp_response->type != ICMP_ECHO_REPLY) {
      std::cout << "Error: ICMP type " << icmp_response->type << " code " << icmp_response->code << std::endl;
    } else {
      std::cout << PACKET_SIZE << " bytes from " << ip_addr_ << ": "
                << "icmp_seq=" << total_sent_ << " ttl=" << ttl_ << " time=" << rtt << " ms" << std::endl;
      total_received_++;
    }
  } else {
    std::cerr << "Could not receive response" << std::endl;
  }
}

