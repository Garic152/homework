#include "DHT.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

uint16_t hash(const char *str) {
  uint8_t digest[SHA256_DIGEST_LENGTH];
  SHA256((uint8_t *)str, strlen(str), digest);
  return htons(*((uint32_t *)digest));
}

bool is_responsible(uint32_t current_id, uint32_t successor_id, uint32_t hash) {
  if (current_id > successor_id) {
    return hash < current_id && hash > successor_id;
  } else {
    return hash < current_id || hash > successor_id;
  }
}

int send_lookup(struct LookupMessage message, struct Destination destination) {
  int sockfd;
  struct sockaddr_in nodeAddr;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("Socket error");
    return -1;
  }

  memset(&nodeAddr, 0, sizeof(nodeAddr));
  nodeAddr.sin_family = AF_INET;
  nodeAddr.sin_port = htons(destination.node_port);
  nodeAddr.sin_addr.s_addr = destination.node_ip;

  message.message_type = htonl(message.message_type);
  message.hash_id = htonl(message.hash_id);
  message.node_id = htonl(message.node_id);
  message.node_port = htonl(message.node_port);

  if (sendto(sockfd, &message, sizeof(message), 0, (struct sockaddr *)&nodeAddr,
             sizeof(nodeAddr)) < 0) {
    perror("Lookup error");
    close(sockfd);
    return -1;
  }
  close(sockfd);
  return 0;
}
