#include "hashing.h"
#include <arpa/inet.h>
#include <openssl/sha.h>
#include <string.h>

uint16_t hash(const char *str) {
  uint8_t digest[SHA256_DIGEST_LENGTH];
  SHA256((uint8_t *)str, strlen(str), digest);
  return htons(*((uint16_t *)digest));
}

int is_responsible(const char *str, int ip) { return 0; }
