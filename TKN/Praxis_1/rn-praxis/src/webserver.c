#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MYPORT "1234"
#define BACKLOG 10

typedef struct HttpRequest {
  char *method;
  char *resource;
  char *version;
  char *body;
} HttpRequest;

void sigchld_handler(int s) {
  int saved_errno = errno;

  while (waitpid(-1, NULL, WNOHANG) < 0)
    ;

  errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void http_response_handler(int new_fd, int status_code) {
  // set msg
  char msg[1024];

  sprintf(msg, "%d\r\n", status_code);

  // if (status_code == 400) {
  //   sprintf(msg, "400 Bad Request\r\n");
  // } else if (status_code == 413) {
  //   sprintf(msg, "413 Buffer Overflow\r\n");
  // }

  // send msg
  int len = strlen(msg);
  int bytes_sent = send(new_fd, msg, len, 0);

  if (bytes_sent == -1) {
    perror("sending");
  } else {
    printf("Response sent.\n");
  }
}

// function to handle https receiving and return msg
char *receive_http_request(int new_fd) {
  // buffer for http requests
  char request_buffer[2048];
  int request_length = 0;

  while (1) {
    char buf[1024];
    memset(buf, 0, sizeof buf);

    int result = recv(new_fd, buf, sizeof buf, 0);

    if (result == -1) {
      perror("recv");
      return NULL;
    } else if (result == 0) {
      printf("Client closed the connection\n");
      return NULL;
    }

    // write data to bigger request buf
    strncat(request_buffer, buf, result);
    request_length += result;

    if (strstr(request_buffer, "\r\n\r\n") != NULL) {
      // return response
      char *dynamic_buffer = malloc(request_length + 1);
      if (dynamic_buffer == NULL) {
        perror("malloc");
        return NULL;
      }

      // copy request into dynamic_buffer
      memcpy(dynamic_buffer, request_buffer, request_length);
      dynamic_buffer[request_length] = '\0';

      return dynamic_buffer;
    } else {
      // handle buffer overflow
      http_response_handler(new_fd, 413);
    }
  }
}

int parse_http_request(const char *http_request, HttpRequest *request) {
  // using sscanf to get data out of the request
  int result = sscanf(http_request, "%s %s %s\r\n", request->method,
                      request->resource, request->version);

  printf("Method:%s\n", request->method);
  printf("Ressource:%s\n", request->resource);
  printf("Version:%s\n", request->version);
  printf("Result:%d\n", result);
  // check if the headline is correct
  if (result != 3) {
    return 400;
  }

  // handle payload requests and check content_length
  if (strcmp(request->method, "POST") == 0 ||
      strcmp(request->method, "PUT") == 0) {
    if (!strstr(http_request, "Content-Length:")) {
      return 400;
    }
  }

  // handle GET requests
  if (strcmp(request->method, "GET") == 0) {
    return 404;
  }

  // handle other cases
  return 501;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
    return 1;
  }

  const char *ip = argv[1];
  const char *port = argv[2];

  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  struct addrinfo hints, *servinfo, *p;
  int sockfd, new_fd;
  struct sigaction sa;
  char s[INET6_ADDRSTRLEN];
  int rv;
  int yes = 1;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // loop through results and bind to sockfd
  if ((rv = getaddrinfo(ip, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server socket error");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }
    break;
  }

  freeaddrinfo(servinfo);

  if (p == NULL) {
    fprintf(stderr, "server failed to bind\n");
    exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  printf("Now listening on IP %s and PORT %s\n", ip, port);
  printf("Waiting for new connections...\n");

  while (1) {
    addr_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }

    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),
              s, sizeof s);

    printf("Connection to %s established!\n Waiting for packages...\n", s);

    // get the full http_request
    char *http_request = receive_http_request(new_fd);

    if (http_request != NULL) {
      // create request struct instance to fill it with data of http_request
      HttpRequest request;
      request.method = malloc(100);
      request.resource = malloc(100);
      request.version = malloc(100);
      int status_code = parse_http_request(http_request, &request);

      http_response_handler(new_fd, status_code);
    }

    close(new_fd);
    printf("Connection closed, waiting for new connection\n");
  }
}
