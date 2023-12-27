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

#include "data.h"
#include "http.h"
#include "util.h"

#define MAX_RESOURCES 100
#define BUFLEN 512
#define EPOLL_MAX_EVENTS 10

struct tuple resources[MAX_RESOURCES] = {
    {"/static/foo", "Foo", sizeof "Foo" - 1},
    {"/static/bar", "Bar", sizeof "Bar" - 1},
    {"/static/baz", "Baz", sizeof "Baz" - 1}};

typedef struct {
  uint32_t id;
  struct in_addr ip;
  uint16_t port;
  struct tuple resources[50];
} DHT;

typedef struct {
  DHT predecessor;
  DHT current;
  DHT successor;
} DHT_NODE;

/**
 * Sends an HTTP reply to the client based on the received request.
 *
 * @param conn      The file descriptor of the client connection socket.
 * @param request   A pointer to the struct containing the parsed request
 * information.
 */
void send_reply(int conn, struct request *request) {

  // Create a buffer to hold the HTTP reply
  char buffer[HTTP_MAX_SIZE];
  char *reply = buffer;

  fprintf(stderr, "Handling %s request for %s (%lu byte payload)\n",
          request->method, request->uri, request->payload_length);

  if (strcmp(request->method, "GET") == 0) {
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
    // Try to set the requested resource with the given payload in the
    // 'resources' array.
    if (set(request->uri, request->payload, request->payload_length, resources,
            MAX_RESOURCES)) {
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

  // Send the reply back to the client
  if (send(conn, reply, strlen(reply), 0) == -1) {
    perror("send");
    close(conn);
  }
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
size_t process_packet(int conn, char *buffer, size_t n) {
  struct request request = {
      .method = NULL, .uri = NULL, .payload = NULL, .payload_length = -1};
  ssize_t bytes_processed = parse_request(buffer, n, &request);

  if (bytes_processed > 0) {
    send_reply(conn, &request);

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
bool handle_connection(struct connection_state *state) {
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
  while ((bytes_processed = process_packet(state->sock, window_start,
                                           window_end - window_start)) > 0) {
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
  if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("bind");
    close(sock);
    exit(EXIT_FAILURE);
  }

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

/**
 *  The program expects 5; otherwise, it returns EXIT_FAILURE.
 *
 *  Call as:
 *
 *  ./build/webserver self.ip self.port self.id
 */
int main(int argc, char **argv) {
  // if (argc != 4) {
  //   return EXIT_FAILURE;
  // }

  if (argc == 4) {
    const int MAX_NODES = 2;
    DHT_NODE nodes[MAX_NODES];

    // parse env variables into DHT_NODE
    nodes[0].predecessor.id = (uint32_t)atoi(argv[3]);
    inet_pton(AF_INET, getenv("PRED_IP"), &nodes[0].predecessor.ip);
    nodes[0].predecessor.port = (uint32_t)atoi(argv[2]);

    nodes[0].current.id = (uint32_t)atoi(getenv("PRED_ID"));
    inet_pton(AF_INET, getenv("PRED_IP"), &nodes[0].current.ip);
    nodes[0].current.port = (uint16_t)atoi(getenv("PRED_PORT"));

    nodes[0].successor.id = (uint32_t)atoi(argv[3]);
    inet_pton(AF_INET, getenv("PRED_IP"), &nodes[0].successor.ip);
    nodes[0].successor.port = (uint32_t)atoi(argv[2]);

    nodes[1].predecessor.id = (uint32_t)atoi(getenv("PRED_ID"));
    inet_pton(AF_INET, getenv("PRED_IP"), &nodes[1].predecessor.ip);
    nodes[1].predecessor.port = (uint16_t)atoi(getenv("PRED_PORT"));

    nodes[1].current.id = (uint32_t)atoi(argv[3]);
    inet_pton(AF_INET, getenv("PRED_IP"), &nodes[1].current.ip);
    nodes[1].current.port = (uint16_t)atoi(argv[2]);

    nodes[1].successor.id = (uint32_t)atoi(getenv("SUCC_ID"));
    inet_pton(AF_INET, getenv("SUCC_IP"), &nodes[1].successor.ip);
    nodes[1].successor.port = (uint16_t)atoi(getenv("SUCC_PORT"));
  }

  struct sockaddr_in addr = derive_sockaddr(argv[1], argv[2]);

  // Initialize epoll()
  int epoll_fd = epoll_create1(0);
  if (epoll_fd == 1) {
    perror("epoll");
    return 1;
  }

  // Set up a TCP server socket.
  int tcp_server_socket = setup_server_socket(addr, 0);

  // Set up a UDP server socket.
  int udp_server_socket = setup_server_socket(addr, 1);

  struct epoll_event ev;
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

  struct connection_state state = {0};

  struct epoll_event events[EPOLL_MAX_EVENTS];

  while (true) {

    // Use epoll() to wait for events on the monitored sockets.
    int n = epoll_wait(epoll_fd, events, EPOLL_MAX_EVENTS, -1);
    if (n == -1) {
      perror("epoll wait");
      exit(EXIT_FAILURE);
    }

    // Process events on the monitored sockets.
    for (int i = 0; i < n; i++) {
      // Check if current connection is tcp
      if (events[i].data.fd == tcp_server_socket) {
        // If the event is on the server_socket, accept a new connection from a
        // client.
        int connection = accept(tcp_server_socket, NULL, NULL);
        if (connection == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
          close(tcp_server_socket);
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
      } else if (events[i].data.fd == udp_server_socket) {
        char buffer[BUFLEN];
        struct sockaddr_in sender_addr;
        socklen_t sender_addr_len = sizeof(sender_addr);

        int received_bytes =
            recvfrom(udp_server_socket, buffer, BUFLEN, 0,
                     (struct sockaddr *)&sender_addr, &sender_addr_len);
        if (received_bytes == -1) {
          perror("recfrom");
        }
      } else {
        int conn_fd = events[i].data.fd;

        state.sock = conn_fd;

        // Call the 'handle_connection' function to process the incoming data on
        // the socket.
        bool cont = handle_connection(&state);
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
