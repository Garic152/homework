#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "DHT.h"

uint16_t hash(const char *str) {
  uint8_t digest[SHA256_DIGEST_LENGTH];
  SHA256((uint8_t *)str, strlen(str), digest);
  return htons(*((uint16_t *)digest));
}

bool is_responsible(uint16_t current_id, uint16_t predecessor_id,
                    uint16_t hash) {
  LOG(LOG_LEVEL_DEBUG, "Entering is_responsible, %d", current_id);
  if (current_id > predecessor_id) {
    return hash <= current_id && hash > predecessor_id;
  } else {
    return hash <= current_id || hash > predecessor_id;
  }
}

int send_message(struct LookupMessage *message, struct Destination destination,
                 int sockfd) {

  struct sockaddr_in nodeAddr;

  // Set necessary information f:w
  // or sendto()
  memset(&nodeAddr, 0, sizeof(nodeAddr));
  nodeAddr.sin_family = AF_INET;
  nodeAddr.sin_port = htons(destination.node_port);
  nodeAddr.sin_addr.s_addr = destination.node_ip;

  // Convert the message to network byte order
  message->message_type = htonl(message->message_type);
  message->hash_id = htons(message->hash_id);
  message->node_id = htons(message->node_id);
  message->node_port = htons(message->node_port);

  if (sendto(sockfd, message, sizeof(*message), 0, (struct sockaddr *)&nodeAddr,
             sizeof(nodeAddr)) < 0) {
    perror("Lookup error");
    close(sockfd);
    return -1;
  }
  return 0;
}

int send_lookup(struct LookupMessage *message, struct Destination destination,
                int sockfd) {
  LOG(LOG_LEVEL_DEBUG, "Entering send_lookup");
  LOG(LOG_LEVEL_INFO, "Now sending message from node %u into dht on socket: %u",
      message->node_port, sockfd);
  struct LookupMessage reply;
  struct sockaddr_in nodeAddr;
  socklen_t addr_len = sizeof(nodeAddr);

  char buffer[1024];

  // Set necessary information for receive()
  memset(&nodeAddr, 0, sizeof(nodeAddr));
  nodeAddr.sin_family = AF_INET;
  nodeAddr.sin_port = htons(message->node_port);
  nodeAddr.sin_addr.s_addr = message->node_ip;

  if (send_message(message, destination, sockfd) < 0) {
    close(sockfd);
    perror("send_message");
  }

  LOG(LOG_LEVEL_INFO, "Now trying to receive on socket %d", sockfd);
  // Wait for reply
  int received_bytes = recvfrom(sockfd, buffer, 1024, 0,
                                (struct sockaddr *)&nodeAddr, &addr_len);
  if (received_bytes == -1) {
    LOG(LOG_LEVEL_WARN, "ERROR RECEIVING MESSAGE");
    perror("recfrom");
    close(sockfd);
  } else if (received_bytes == sizeof(struct LookupMessage)) {
    struct LookupMessage *received_message = (struct LookupMessage *)buffer;

    // Convert received message from network byte order to host byte order
    received_message->message_type = ntohl(received_message->message_type);
    received_message->hash_id = ntohs(received_message->hash_id);
    received_message->node_id = ntohs(received_message->node_id);
    received_message->node_port = ntohs(received_message->node_port);

    LOG(LOG_LEVEL_INFO, "Successfully received message from node %d",
        received_message->node_port);

    *message = *received_message;

    // close(sockfd);
    return 0;
  } else {
    // close(sockfd);
    perror("receive error bottom");
    return -1;
  }
  perror("receive error bottom");
  return -1;
}

int receive_lookup(struct LookupMessage *message, DHT_NODE *node, int sockfd) {
  LOG(LOG_LEVEL_DEBUG, "Entering receive_lookup, Current ID: %d, Hash: %d", node->current.id, message->hash_id);
  struct Destination destination;

  if (is_responsible(node->current.id, node->predecessor.id, message->hash_id)) {
    LOG(LOG_LEVEL_INFO, "Current node %s is responsible for the resource %d",
        node->current.port, message->hash_id);

    // Redefine destination to root node
    destination.node_port = message->node_port;
    destination.node_ip = message->node_ip;

    // Node is responsible, send information back to origin node
    message->message_type = 1;
    message->node_id = node->current.id;
    message->node_port = atoi(node->current.port);
    message->node_ip = inet_addr(node->current.ip);

    if (send_message(message, destination, sockfd) < 0) {
      perror("send message responsible");
      return -1;
    }
    return 0;
  }

  if (is_responsible(node->successor.id, node->current.id, message->hash_id)) {
    LOG(LOG_LEVEL_INFO, "Successor node %d is responsible for the resource",
        node->successor.id);

    // Redefine destination to root node
    destination.node_port = message->node_port;
    destination.node_ip = message->node_ip;

    // Node is responsible, send information back to origin node
    message->message_type = 1;
    message->node_id = node->successor.id;
    message->node_port = atoi(node->successor.port);
    message->node_ip = inet_addr(node->successor.ip);

    if (send_message(message, destination, sockfd) < 0) {
      perror("send message responsible");
      return -1;
    }
    return 0;
  } 

  LOG(LOG_LEVEL_INFO,
      "NodePort %s with id %u is NOT responsible for the resource %u, "
      "sending to %u with NodePort %s",
      node->current.port, node->current.id, message->hash_id,
      node->successor.id, node->successor.port);

  destination.node_ip = inet_addr(node->successor.ip);
  destination.node_port = atoi(node->successor.port);
  if (send_message(message, destination, sockfd) < 0) {
    perror("send message not responsible");
    return -1;
  }
  return 0;
}
