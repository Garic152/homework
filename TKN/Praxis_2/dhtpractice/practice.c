#include <arpa/inet.h>
#include <openssl/sha.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 256
#define TABLE_SIZE 100

typedef struct {
  char name[MAX_NAME];
  int age;
  //...
} person;

person *hash_table[TABLE_SIZE];

unsigned int hash(char *name) {
  uint8_t digest[SHA256_DIGEST_LENGTH];
  SHA256((uint8_t *)name, strlen(name), digest);
  return (htons(*((uint16_t *)digest))) % 100;
}

void init_hash_table() {
  for (int i = 0; i < TABLE_SIZE; i++) {
    hash_table[i] = NULL;
  }
  // table is empty
}

void print_table() {
  for (int i = 0; i < TABLE_SIZE; i++) {
    if (hash_table[i] == NULL) {
      printf("\t%i\t---\n", i);
    } else {
      printf("\t%i\t%s\n", i, hash_table[i]->name);
    }
  }
}

bool hash_table_insert(person *p) {
  if (p == NULL)
    return false;
  int index = hash(p->name);
  for (int i = 0; i < TABLE_SIZE; i++) {
    int try = (i + index) % TABLE_SIZE;
    if (hash_table[try] == NULL) {
      hash_table[try] = p;
      return true;
    }
  }
  return false;
}

person *hash_table_lookup(char *name) {
  int index = hash(name);
  if (hash_table[index] != NULL &&
      strncmp(hash_table[index]->name, name, TABLE_SIZE) == 0) {
    return hash_table[index];
  } else {
    return NULL;
  }
}

person *hash_table_delete(char *name) {
  int index = hash(name);
  if (hash_table[index] != NULL &&
      strncmp(hash_table[index]->name, name, TABLE_SIZE) == 0) {
    person *tmp = hash_table[index];
    hash_table[index] = NULL;
    return tmp;
  } else {
    return NULL;
  }
}

int main() {
  init_hash_table();

  person jacob = {.name = "Jacob", .age = 32};
  person steven = {.name = "Steven", .age = 55};
  person will = {.name = "Will", .age = 99};

  printf("%u", hash("Tom"));

  hash_table_insert(&jacob);
  hash_table_insert(&steven);
  hash_table_insert(&will);

  print_table();

  person *tmp = hash_table_lookup("Steven");

  printf("%s\n", tmp->name);
  return 0;
}
