#include <arpa/inet.h>
#include <time.h>

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
  if (level >= currentLogLevel) {                                              \
    struct timespec ts;                                                        \
    clock_gettime(CLOCK_REALTIME, &ts);                                        \
    time_t rawtime = ts.tv_sec;                                                \
    struct tm *timeinfo = localtime(&rawtime);                                 \
    char buffer[26];                                                           \
    strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);                    \
    fprintf(stderr, "%s.%06ld %s " fmt "\n", buffer, ts.tv_nsec / 1000,        \
            level == LOG_LEVEL_DEBUG   ? "\x1B[34mDEBUG\x1B[0m"                \
            : level == LOG_LEVEL_INFO  ? "\x1B[32mINFO\x1B[0m"                 \
            : level == LOG_LEVEL_WARN  ? "\x1B[33mWARN\x1B[0m"                 \
            : level == LOG_LEVEL_ERROR ? "\x1B[31mERROR\x1B[0m"                \
                                       : "UNKNOWN",                            \
            ##__VA_ARGS__);                                                    \
  }

#pragma pack(1)
struct LookupMessage {
  int8_t message_type;
  uint16_t hash_id;
  uint16_t node_id;
  in_addr_t node_ip;
  int16_t node_port;
};
#pragma pack()

struct Destination {
  in_addr_t node_ip;
  int16_t node_port;
};

uint16_t hash(const char *str);
bool is_responsible(uint16_t current_id, uint16_t predecessor_id,
                    uint16_t hash);
int send_lookup(struct LookupMessage *message, struct Destination destination,
                int sockfd);
int receive_lookup(struct LookupMessage *message, DHT_NODE *node, int sockfd);

#endif // HASHING_H
