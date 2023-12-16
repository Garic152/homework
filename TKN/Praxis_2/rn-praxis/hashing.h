#include <arpa/inet.h>

#ifndef HASHING_H
#define HASHING_H

uint16_t hash(const char *str);
int is_responsible(const char *str, int);

#endif // HASHING_H
