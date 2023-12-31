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
  char *content;
} HttpRequest;

const int REQUEST_SIZE = 64;

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

void http_response_handler(int new_fd, int status_code, const char *content) {
  // set msg
  char msg[1024];

  int bytes_sent;

  if (status_code == 200 && content != NULL) {
    char header[1024];
    sprintf(msg,
            "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
            "%lu\r\n\r\n%s",
            strlen(content), content);
  } else if (status_code == 400) {
    sprintf(msg, "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n");
  } else if (status_code == 404) {
    sprintf(msg, "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n");
  } else if (status_code == 413) {
    sprintf(msg, "HTTP/1.1 413 Payload Too Large\r\nContent-Length: 0\r\n\r\n");
  } else if (status_code == 501) {
    sprintf(msg, "HTTP/1.1 501 Not Implemented\r\nContent-Length: 0\r\n\r\n");
  } else if (status_code == 201) {
    sprintf(msg, "HTTP/1.1 201 Created\r\nContent-Length: 0\r\n\r\n");
  } else if (status_code == 204) {
    sprintf(msg, "HTTP/1.1 204 No Content\r\nContent-Length: 0\r\n\r\n");
  } else {
    // Default response or for other status codes
    sprintf(msg, "HTTP/1.1 %d\r\nContent-Length: 0\r\n\r\n", status_code);
  }

  // if (status_code == 400) {
  //   sprintf(msg, "400 Bad Request\r\n");
  // } else if (status_code == 413) {
  //   sprintf(msg, "413 Buffer Overflow\r\n");
  // }

  // send msg
  int len = strlen(msg);

  bytes_sent = send(new_fd, msg, len, 0);
  if (bytes_sent == -1) {
    perror("sending");
  } else {
    printf("Response sent.\n");
  }
}

// function to handle https receiving and return msg
char *receive_http_request(int new_fd) {
  // buffer for http requests
  char request_buffer[4096 * 2];
  memset(request_buffer, 0, sizeof(request_buffer));

  int request_length = 0;
  int header_end_found = 0;

  while (!header_end_found) {
    char buf[1024];
    memset(buf, 0, sizeof buf);

    int result = recv(new_fd, buf, sizeof(buf) - 1, 0);

    if (result == -1) {
      perror("recv");
      break;
    } else if (result == 0) {
      printf("Client closed the connection\n");
      break;
    }

    // write data to bigger request buf and check if it is full
    if (request_length + result < sizeof(request_buffer) - 1) {
      memcpy(request_buffer + request_length, buf, result);
      request_length += result;
      request_buffer[request_length] = '\0';
      printf("Received HTTP Request:%s\n", request_buffer);
    } else {
      // buffer is full
      http_response_handler(new_fd, 413, NULL);
      printf("BUFFER IS FULL!\n");
      return NULL;
    }

    // check end of received string for http end
    if (strstr(request_buffer, "\r\n\r\n") != NULL) {
      header_end_found = 1;
    } else {
      printf("End not found, waiting for the rest of the HTTP.\n");
    }
  }

  // return response
  char *dynamic_buffer = malloc(request_length + 1);
  if (dynamic_buffer == NULL) {
    perror("dynbuf malloc");
    return NULL;
  }

  // copy request into dynamic_buffer
  memcpy(dynamic_buffer, request_buffer, request_length);
  dynamic_buffer[request_length] = '\0';
  return dynamic_buffer;
}

int parse_http_request(const char *http_request, HttpRequest *request) {
  // using sscanf to get data out of the request
  int result = sscanf(http_request, "%63s %63s %63s\r\n", request->method,
                      request->resource, request->version);

  printf("Method:%s\n", request->method);
  printf("Ressource:%s\n", request->resource);
  printf("Version:%s\n", request->version);
  printf("Result:%d\n", result);
  // check if the headline is correct
  if (result != 3) {
    return 400;
  }

  const char *headers_start = strstr(http_request, "\r\n") + 2;
  const char *content_length_str = "Content-Length: ";
  const char *content_length_pos = strstr(http_request, content_length_str);

  if (content_length_pos) {
    int content_length;
    sscanf(content_length_pos, "Content-Length: %d", &content_length);

    const char *body_start = strstr(headers_start, "\r\n\r\n") + 4;
    if (body_start) {
      strncpy(request->body, body_start, content_length);
    }
  }

  // convert filepath
  if (request->resource[0] == '/') {
    memmove(request->resource, request->resource + 1,
            strlen(request->resource));
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
    FILE *fptr;
    fptr = fopen(request->resource, "r");

    if (fptr != NULL) {
      // read file content
      fseek(fptr, 0, SEEK_END);
      long fsize = ftell(fptr);
      fseek(fptr, 0, SEEK_SET);

      free(request->content);

      // pass content into HttpRequest struct
      request->content = malloc(fsize + 1);
      if (request->content == NULL) {
        fclose(fptr);
        return 500;
      }

      fread(request->content, 1, fsize, fptr);
      fclose(fptr);

      if (fsize > 0 && request->content[fsize - 1] == '\n') {
        request->content[fsize - 1] = '\0';
      } else {
        request->content[fsize] = '\0';
      }

      return 200;
    }
    return 404;
  } else if (strcmp(request->method, "PUT") == 0) {
    int existed = 0;

    FILE *fptr = fopen(request->resource, "r");

    if (fptr) {
      existed = 1;
      fclose(fptr);
    }

    fptr = fopen(request->resource, "w");

    if (fptr) {
      fprintf(fptr, "%s", request->body);
      fclose(fptr);

      if (existed) {
        return 204;
      } else {
        return 201;
      }
    }
  } else if (strcmp(request->method, "DELETE") == 0) {
    int existed = 0;

    FILE *fptr = fopen(request->resource, "r");

    if (fptr) {
      existed = 1;
      fclose(fptr);
    }

    if (fptr) {
      remove(request->resource);
      if (existed) {
        return 204;
      } else {
        return 404;
      }
    }
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
    while (1) {
      // get the full http_request
      char *http_request = receive_http_request(new_fd);

      if (http_request == NULL)
        break;

      if (http_request != NULL) {
        // create request struct instance to fill it with data of http_request
        HttpRequest request;

        request.method = malloc(REQUEST_SIZE);
        if (request.method == NULL) {
          perror("request.method malloc");
          break;
        }

        request.resource = malloc(REQUEST_SIZE);
        if (request.resource == NULL) {
          free(request.method);
          perror("request.resource malloc");
          break;
        }

        request.version = malloc(REQUEST_SIZE);
        if (request.version == NULL) {
          free(request.method);
          free(request.resource);
          perror("request.version malloc");
          break;
        }

        request.content = malloc(REQUEST_SIZE);
        if (request.content == NULL) {
          free(request.method);
          free(request.resource);
          free(request.version);
          perror("request.version malloc");
          break;
        }

        request.body = malloc(REQUEST_SIZE);
        if (request.body == NULL) {
          free(request.method);
          free(request.resource);
          free(request.content);
          free(request.version);
          perror("request.version malloc");
          break;
        }

        memset(request.method, 0, REQUEST_SIZE);
        memset(request.resource, 0, REQUEST_SIZE);
        memset(request.version, 0, REQUEST_SIZE);
        memset(request.content, 0, REQUEST_SIZE);
        memset(request.body, 0, REQUEST_SIZE);

        int status_code = parse_http_request(http_request, &request);

        free(request.method);
        free(request.resource);
        free(request.version);
        free(request.body);

        free(http_request);

        http_response_handler(new_fd, status_code, request.content);

        free(request.content);

        // if (strcmp(request.version, "HTTP/1.0") == 0 || status_code != 200) {
        //  break; // Close the connection for HTTP/1.0 or if there's an error
        //}
      }
    }
    close(new_fd);
    printf("Connection closed, waiting for new connection\n");
  }
}
