#include <arpa/inet.h>

#include <stdbool.h>

#ifndef HASHING_H
#define HASHING_H

struct LookupMessage {
  int message_type;
  uint32_t hash_id;
  uint32_t node_id;
  const char *node_ip;
  int node_port;
};

uint16_t hash(const char *str);
bool is_responsible(uint32_t current_id, uint32_t successor_id, uint32_t hash);
int send_lookup(struct LookupMessage message);

#endif // HASHING_H
