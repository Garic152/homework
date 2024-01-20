#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/sha.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "DHT.h"
#include "data.h"
#include "http.h"
#include "structures.h"
#include "util.h"

#define MAX_RESOURCES 100
#define MAX_ENTRIES 10
#define BUFLEN 512
#define EPOLL_MAX_EVENTS 10

int currentLogLevel = LOG_LEVEL_DEBUG;

// Temporarely define UDP socket globally
int udp_socket = -1;

typedef struct {
  in_addr_t ip;
  uint32_t resource;
  int port;
} DHT_Entry;

typedef struct {
  DHT_Entry entries[10];
  int current;
} DHT_History;

struct tuple resources[MAX_RESOURCES] = {
    {"/static/foo", "Foo", sizeof "Foo" - 1},
    {"/static/bar", "Bar", sizeof "Bar" - 1},
    {"/static/baz", "Baz", sizeof "Baz" - 1}};

void initialize_node(DHT_NODE *node) {
  // Read predecessor info from evnironment variables
  node->predecessor.id = atoi(getenv("PRED_ID"));
  node->predecessor.ip = getenv("PRED_IP");
  node->predecessor.port = getenv("PRED_PORT");

  // Read successor info from environment variables
  node->successor.id = atoi(getenv("SUCC_ID"));
  node->successor.ip = getenv("SUCC_IP");
  node->successor.port = getenv("SUCC_PORT");
}

void initialize_dht_history(DHT_History *history) {
  memset(history, 0, sizeof(DHT_History));
}

void addEntry(DHT_History *history, in_addr_t *ip, uint32_t *resource,
              int *port) {
  history->entries[history->current].ip = *ip;
  history->entries[history->current].resource = *resource;
  history->entries[history->current].port = *port;

  history->current = (history->current + 1) % MAX_ENTRIES;
}

int findEntry(DHT_History *history, uint32_t *resource, DHT_Entry *result) {
  for (int i = 0; i < MAX_ENTRIES; i++) {
    if (history->entries[i].resource == *resource) {
      *result = history->entries[i];
      return 1;
    }
  }
  return 0;
}

/**
 * Sends an HTTP reply to the client based on the received request.
 *
 * @param conn      The file descriptor of the client connection socket.
 * @param request   A pointer to the struct containing the parsed request
 * information.
 */
void send_reply(int conn, struct request *request, DHT_NODE *node,
                DHT_History *history) {

  LOG(LOG_LEVEL_DEBUG, "Entering send_reply for connection %d\n", conn);

  // Create a buffer to hold the HTTP reply
  char buffer[HTTP_MAX_SIZE];
  char *reply = buffer;

  fprintf(stderr, "Handling %s request for %s (%lu byte payload)\n",
          request->method, request->uri, request->payload_length);

  if (is_responsible(node->current.id, node->predecessor.id, request->hash)) {
    LOG(LOG_LEVEL_INFO, "Node is responsible for the request\n");

    if (strcmp(request->method, "GET") == 0) {
      LOG(LOG_LEVEL_DEBUG, "Handling GET request for URI: %s\n", request->uri);
      // Find the resource with the given URI in the 'resources' array.
      size_t resource_length;
      const char *resource =
          get(request->uri, resources, MAX_RESOURCES, &resource_length);

      if (resource) {
        sprintf(reply, "HTTP/1.1 200 OK\r\nContent-Length: %lu\r\n\r\n%.*s",
                resource_length, (int)resource_length, resource);
      } else {
        reply = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
      }
    } else if (strcmp(request->method, "PUT") == 0) {
      LOG(LOG_LEVEL_DEBUG, "Handling PUT request for URI: %s\n", request->uri);
      // Try to set the requested resource with the given payload in the
      // 'resources' array.
      if (set(request->uri, request->payload, request->payload_length,
              resources, MAX_RESOURCES)) {
        reply = "HTTP/1.1 204 No Content\r\n\r\n";
      } else {
        reply = "HTTP/1.1 201 Created\r\nContent-Length: 0\r\n\r\n";
      }
    } else if (strcmp(request->method, "DELETE") == 0) {
      // Try to delete the requested resource from the 'resources' array
      if (delete (request->uri, resources, MAX_RESOURCES)) {
        reply = "HTTP/1.1 204 No Content\r\n\r\n";
      } else {
        reply = "HTTP/1.1 404 Not Found\r\n\r\n";
      }
    } else {
      reply = "HTTP/1.1 501 Method Not Supported\r\n\r\n";
    }
  } else {
    LOG(LOG_LEVEL_INFO,
        "Node is not responsible, performing lookup or sending 503\n");
    DHT_Entry entry;
    if (findEntry(history, &request->hash, &entry)) {
      LOG(LOG_LEVEL_INFO, "Lookup successful for hash: %u\n", request->hash);

      // Handle IP adress
      char ip_str[32];
      inet_ntop(AF_INET, &(entry.ip), ip_str, 32);

      sprintf(reply,
              "HTTP/1.1 303 See "
              "Other\r\nLocation:http://%s:%d%s\r\nContent-Length: 0\r\n\r\n",
              ip_str, entry.port, request->uri);
    } else {
      LOG(LOG_LEVEL_WARN,
          "Sending 503 Service Unavailable for hash and performing lookup: "
          "%u\n",
          request->hash);

      // Copy lookup information into message struct
      struct LookupMessage message = {.message_type = 0,
                                      .hash_id = request->hash,
                                      .node_id = node->current.id,
                                      .node_ip = inet_addr(node->current.ip),
                                      .node_port = atoi(node->current.port)};

      // Copy destination node into Destination struct
      struct Destination destination = {
          .node_ip = inet_addr(node->successor.ip),
          .node_port = atoi(node->successor.port)};

      sprintf(reply, "HTTP/1.1 503 Service Unavailable\r\nRetry-After: "
                     "5\r\nContent-Length: 0\r\n\r\n");

      if ((send_lookup(&message, destination, udp_socket)) < 0) {
        LOG(LOG_LEVEL_ERROR, "Failed to send or receive on lookup\n");
        perror("Send lookup");
      } else {
        // Handle IP adress
        char ip_str[32];
        inet_ntop(AF_INET, &(message.node_ip), ip_str, 32);

        addEntry(history, &message.node_ip, &message.hash_id,
                 &message.node_port);
      }
    }
  }

  LOG(LOG_LEVEL_INFO, "Sending reply: %s\n", reply);
  // Send the reply back to the client
  if (send(conn, reply, strlen(reply), 0) == -1) {
    perror("send");
    LOG(LOG_LEVEL_ERROR, "Failed to send reply on connection %d\n", conn);
    close(conn);
  }
  LOG(LOG_LEVEL_DEBUG, "Exiting send_reply for connection %d\n", conn);
}

/**
 * Processes an incoming packet from the client.
 *
 * @param conn The socket descriptor representing the connection to the client.
 * @param buffer A pointer to the incoming packet's buffer.
 * @param n The size of the incoming packet.
 *
 * @return Returns the number of bytes processed from the packet.
 *         If the packet is successfully processed and a reply is sent, the
 * return value indicates the number of bytes processed. If the packet is
 * malformed or an error occurs during processing, the return value is -1.
 *
 */
size_t process_packet(int conn, char *buffer, size_t n, DHT_NODE node,
                      DHT_History *history) {
  struct request request = {
      .method = NULL, .uri = NULL, .payload = NULL, .payload_length = -1};
  ssize_t bytes_processed = parse_request(buffer, n, &request);

  if (bytes_processed > 0) {
    send_reply(conn, &request, &node, history);

    // Check the "Connection" header in the request to determine if the
    // connection should be kept alive or closed.
    const string connection_header = get_header(&request, "Connection");
    if (connection_header && strcmp(connection_header, "close")) {
      return -1;
    }
  } else if (bytes_processed == -1) {
    // If the request is malformed or an error occurs during processing, send a
    // 400 Bad Request response to the client.
    const string bad_request = "HTTP/1.1 400 Bad Request\r\n\r\n";
    send(conn, bad_request, strlen(bad_request), 0);
    printf("Received malformed request, terminating connection.\n");
    close(conn);
    return -1;
  }

  return bytes_processed;
}

/**
 * Sets up the connection state for a new socket connection.
 *
 * @param state A pointer to the connection_state structure to be initialized.
 * @param sock The socket descriptor representing the new connection.
 *
 */
static void connection_setup(struct connection_state *state, int sock) {
  // Set the socket descriptor for the new connection in the connection_state
  // structure.
  state->sock = sock;

  // Set the 'end' pointer of the state to the beginning of the buffer.
  state->end = state->buffer;

  // Clear the buffer by filling it with zeros to avoid any stale data.
  memset(state->buffer, 0, HTTP_MAX_SIZE);
}

/**
 * Discards the front of a buffer
 *
 * @param buffer A pointer to the buffer to be modified.
 * @param discard The number of bytes to drop from the front of the buffer.
 * @param keep The number of bytes that should be kept after the discarded
 * bytes.
 *
 * @return Returns a pointer to the first unused byte in the buffer after the
 * discard.
 * @example buffer_discard(ABCDEF0000, 4, 2):
 *          ABCDEF0000 ->  EFCDEF0000 -> EF00000000, returns pointer to first 0.
 */
char *buffer_discard(char *buffer, size_t discard, size_t keep) {
  memmove(buffer, buffer + discard, keep);
  memset(buffer + keep, 0, discard); // invalidate buffer
  return buffer + keep;
}

/**
 * Handles incoming connections and processes data received over the socket.
 *
 * @param state A pointer to the connection_state structure containing the
 * connection state.
 * @return Returns true if the connection and data processing were successful,
 * false otherwise. If an error occurs while receiving data from the socket, the
 * function exits the program.
 */
bool handle_connection(struct connection_state *state, DHT_NODE *node,
                       DHT_History *history) {
  // Calculate the pointer to the end of the buffer to avoid buffer overflow
  const char *buffer_end = state->buffer + HTTP_MAX_SIZE;

  // Check if an error occurred while receiving data from the socket
  ssize_t bytes_read =
      recv(state->sock, state->end, buffer_end - state->end, 0);
  if (bytes_read == -1) {
    perror("recv");
    close(state->sock);
    exit(EXIT_FAILURE);
  } else if (bytes_read == 0) {
    return false;
  }

  char *window_start = state->buffer;
  char *window_end = state->end + bytes_read;

  ssize_t bytes_processed = 0;
  while ((bytes_processed =
              process_packet(state->sock, window_start,
                             window_end - window_start, *node, history)) > 0) {
    window_start += bytes_processed;
  }
  if (bytes_processed == -1) {
    return false;
  }

  state->end = buffer_discard(state->buffer, window_start - state->buffer,
                              window_end - window_start);
  return true;
}

/**
 * Derives a sockaddr_in structure from the provided host and port information.
 *
 * @param host The host (IP address or hostname) to be resolved into a network
 * address.
 * @param port The port number to be converted into network byte order.
 *
 * @return A sockaddr_in structure representing the network address derived from
 * the host and port.
 */
static struct sockaddr_in derive_sockaddr(const char *host, const char *port) {
  struct addrinfo hints = {
      .ai_family = AF_INET,
  };
  struct addrinfo *result_info;

  // Resolve the host (IP address or hostname) into a list of possible
  // addresses.
  int returncode = getaddrinfo(host, port, &hints, &result_info);
  if (returncode) {
    fprintf(stderr, "Error parsing host/port");
    exit(EXIT_FAILURE);
  }

  // Copy the sockaddr_in structure from the first address in the list
  struct sockaddr_in result = *((struct sockaddr_in *)result_info->ai_addr);

  // Free the allocated memory for the result_info
  freeaddrinfo(result_info);
  return result;
}

/**
 * Sets up a TCP server socket and binds it to the provided sockaddr_in address.
 *
 * @param addr The sockaddr_in structure representing the IP address and port of
 * the server.
 *
 * @return The file descriptor of the created TCP server socket.
 */
static int setup_server_socket(struct sockaddr_in addr, int is_udp) {
  LOG(LOG_LEVEL_DEBUG, "IS_UDP: %d\n", is_udp);
  LOG(LOG_LEVEL_INFO, "IS_UDP: %d\n", is_udp);
  LOG(LOG_LEVEL_WARN, "IS_UDP: %d\n", is_udp);
  LOG(LOG_LEVEL_ERROR, "IS_UDP: %d\n", is_udp);
  const int enable = 1;
  const int backlog = 1;

  int sock_type = is_udp ? SOCK_DGRAM : SOCK_STREAM;

  // Create a socket
  int sock = socket(AF_INET, sock_type, 0);
  if (sock == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  if (!is_udp) {
    // Avoid dead lock on connections that are dropped after poll returns but
    // before accept is called
    if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1) {
      perror("fcntl");
      exit(EXIT_FAILURE);
    }
  }

  // Set the SO_REUSEADDR socket option to allow reuse of local addresses
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) ==
      -1) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  // Bind socket to the provided address
  if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    close(sock);
    exit(EXIT_FAILURE);
  }

  uint namelen = sizeof(addr);
  if (getsockname(sock, (struct sockaddr *)&addr, &namelen) < 0) {
    perror("getsockname()");
    exit(3);
  }

  printf("Port %d is assigned to %d\n", ntohs(addr.sin_port), sock);

  if (!is_udp) {
    // Start listening on the socket with maximum backlog of 1 pending
    // connection
    if (listen(sock, backlog)) {
      perror("listen");
      exit(EXIT_FAILURE);
    }
  }

  return sock;
}

void parse_arguments(int argc, char *argv[], DHT_NODE *node) {
  node->current.ip = argv[1];
  node->current.port = argv[2];
  if (argc == 4) {
    node->current.id = atoi(argv[3]);
  }
}

/**
 *  The program expects 5; otherwise, it returns EXIT_FAILURE.
 *
 *  Call as:
 *
 *  ./build/webserver self.ip self.port self.id
 */
int main(int argc, char **argv) {
  if (argc < 3) {
    return EXIT_FAILURE;
  }

  // Initialize node information
  const int MAX_NODES = 1;
  DHT_NODE nodes[MAX_NODES];

  if (argc == 3) {
    parse_arguments(argc, argv, &nodes[0]);
  } else if (argc == 4) {
    initialize_node(&nodes[0]);
    parse_arguments(argc, argv, &nodes[0]);
  }

  // Initialize dht_lookup_table
  DHT_History history;
  initialize_dht_history(&history);

  // Initialize epoll()
  struct epoll_event ev, events[EPOLL_MAX_EVENTS];

  int epoll_fd = epoll_create1(0);
  if (epoll_fd == 1) {
    perror("epoll");
    return 1;
  }

  int *server_sockets = malloc(MAX_NODES * 2 * sizeof(int));

  if (!server_sockets) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < MAX_NODES; i++) {
    struct sockaddr_in addr = derive_sockaddr(argv[1], nodes[i].current.port);

    // Set up a TCP server socket.
    int tcp_server_socket = setup_server_socket(addr, 0);

    // Set up a UDP server socket.
    int udp_server_socket = setup_server_socket(addr, 1);
    // Temporarely pass the UDP socket into global var
    udp_socket = udp_server_socket;

    server_sockets[i * 2] = tcp_server_socket;
    server_sockets[i * 2 + 1] = udp_server_socket;

    ev.events = EPOLLIN;

    ev.data.fd = tcp_server_socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, tcp_server_socket, &ev) == -1) {
      perror("epoll_ctl: tcp_server_socket");
      exit(EXIT_FAILURE);
    }

    ev.data.fd = udp_server_socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, udp_server_socket, &ev) == -1) {
      perror("epoll_ctl: udp_server_socket");
      exit(EXIT_FAILURE);
    }
  }

  struct connection_state state = {0};

  while (true) {

    // Use epoll() to wait for events on the monitored sockets.
    int n = epoll_wait(epoll_fd, events, EPOLL_MAX_EVENTS, -1);
    if (n == -1) {
      perror("epoll wait");
      exit(EXIT_FAILURE);
    }

    // Process events on the monitored sockets.
    for (int i = 0; i < n; i++) {
      int current_fd = events[i].data.fd;

      bool is_tcp_socket = false;
      bool is_udp_socket = false;

      for (int j = 0; j < MAX_NODES * 2; j++) {
        if (server_sockets[j] == current_fd) {
          if (j % 2 == 0) {
            is_tcp_socket = true;
          } else {
            is_udp_socket = true;
          }
          break;
        }
      }

      // Check if current connection is tcp
      if (is_tcp_socket) {
        // If the event is on the server_socket, accept a new connection from a
        // client.
        int connection = accept(current_fd, NULL, NULL);
        if (connection == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
          close(current_fd);
          perror("accept");
          exit(EXIT_FAILURE);
        } // else {
        connection_setup(&state, connection);
        // Close connection
        ev.events = EPOLLIN;
        ev.data.fd = connection;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection, &ev) == -1) {
          perror("epoll_ctl: connection");
          close(connection);
        }
        // else handle data from udp
      } else if (is_udp_socket) {
        char buffer[BUFLEN];
        struct sockaddr_in sender_addr;
        socklen_t sender_addr_len = sizeof(sender_addr);

        int received_bytes =
            recvfrom(current_fd, buffer, BUFLEN, 0,
                     (struct sockaddr *)&sender_addr, &sender_addr_len);
        if (received_bytes == -1) {
          perror("recfrom");
          LOG(LOG_LEVEL_DEBUG,
              "Didnt receive udp data, now closing socket: %u\n",
              events[i].data.fd);
          close(events[i].data.fd);
        } else if (received_bytes == sizeof(struct LookupMessage)) {
          LOG(LOG_LEVEL_DEBUG, "Received UDP data on sock: %u\n",
              events[i].data.fd);
          struct LookupMessage *message = (struct LookupMessage *)buffer;

          // Convert received message from network byte order to host byte order
          message->message_type = ntohl(message->message_type);
          message->hash_id = ntohl(message->hash_id);
          message->node_id = ntohl(message->node_id);
          message->node_port = ntohs(message->node_port);

          // Handle IP adress
          char ipStr[32];
          inet_ntop(AF_INET, &(message->node_ip), ipStr, 32);

          if (receive_lookup(message, &nodes[0], events[i].data.fd) < 0) {
            perror("receive_lookup");
            close(events[i].data.fd);
            LOG(LOG_LEVEL_ERROR, "receive lookup failed");
          };
        } else {
          LOG(LOG_LEVEL_WARN, "RECEIVED WRONG UDP PACKAGE!");
        }
      } else {
        int conn_fd = events[i].data.fd;

        state.sock = conn_fd;

        // Call the 'handle_connection' function to process the incoming data on
        // the socket.
        bool cont = handle_connection(&state, &nodes[0], &history);
        if (!cont) { // get ready for a new connection
          if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, conn_fd, NULL) == -1) {
            perror("epoll_ctl: remove connection");
          }
          close(conn_fd);
        }
      }
    }
  }

  return EXIT_SUCCESS;
}
