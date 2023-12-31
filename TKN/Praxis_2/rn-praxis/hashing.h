#include <arpa/inet.h>

#include <stdbool.h>

#ifndef HASHING_H
#define HASHING_H

uint16_t hash(const char *str);
bool is_responsible(uint32_t *current_id, uint32_t *successor_id,
                    uint32_t *hash);

#endif // HASHING_H
