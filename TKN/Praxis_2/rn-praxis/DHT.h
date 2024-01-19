#include <arpa/inet.h>

#include "structures.h"
#include <stdbool.h>

#ifndef HASHING_H
#define HASHING_H

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_ERROR 3

extern int currentLogLevel;

#define LOG(level, fmt, ...)                                                   \
  if (level >= currentLogLevel)                                                \
  fprintf(stderr, fmt, ##__VA_ARGS__)

struct LookupMessage {
  int message_type;
  uint32_t hash_id;
  uint32_t node_id;
  in_addr_t node_ip;
  int node_port;
};

struct Destination {
  in_addr_t node_ip;
  int node_port;
};

uint16_t hash(const char *str);
bool is_responsible(uint32_t current_id, uint32_t predecessor_id,
                    uint32_t hash);
int send_lookup(struct LookupMessage *message, struct Destination destination,
                int sockfd);
int receive_lookup(struct LookupMessage *message, DHT_NODE *node, int sockfd);

#endif // HASHING_H
