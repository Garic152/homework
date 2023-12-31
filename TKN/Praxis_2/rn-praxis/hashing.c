#include "hashing.h"
#include <arpa/inet.h>
#include <openssl/sha.h>
#include <string.h>

#include <stdbool.h>

uint16_t hash(const char *str) {
  uint8_t digest[SHA256_DIGEST_LENGTH];
  SHA256((uint8_t *)str, strlen(str), digest);
  return htons(*((uint32_t *)digest));
}

bool is_responsible(uint32_t *current_id, uint32_t *successor_id,
                    uint32_t *hash) {
  if (*current_id > *successor_id) {
    return *hash<*current_id && * hash> * successor_id;
  } else {
    return *hash < *current_id || *hash > *successor_id;
  }
}
