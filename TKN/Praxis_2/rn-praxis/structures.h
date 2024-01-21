#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"

typedef struct {
  uint16_t id;
  // struct in_addr ip;
  const char *ip;
  // uint16_t port;
  const char *port;
  struct tuple resources[50];
} DHT;

typedef struct {
  DHT predecessor;
  DHT current;
  DHT successor;
} DHT_NODE;

#endif
