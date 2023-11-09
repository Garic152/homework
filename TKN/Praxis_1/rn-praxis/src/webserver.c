#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char *argv[]) {
  struct addrinfo hints, *res, *p;
  int status;
  char ipstr[INET6_ADDRSTRLEN];

  if(argc != 2) {
    printf("We don't do that here boi.");
    return 1;
  }

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if((status = getaddrinfo(NULL, "3490", &hints, &res)) != 0) {
    fprintf(stderr, "gettaddrinfo error: %s\n", gai_strerror(status));
    return 2;
  }

  printf("IP adresses for %s:\n\n", argv[1]);

  for(p = res; p != NULL; p = p->ai_next) {
    void *addr;
    char *ipver;

    int *socktype;
    
    if (p->ai_family == AF_INET) {
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
      addr = &(ipv4->sin_addr);
      ipver = "IPv4";
    } else {
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
      addr = &(ipv6->sin6_addr);
      ipver = "IPv6";
    }
    

    //print canon name
    if (p->ai_socktype != 0) {
      socktype = &(p->ai_socktype);
    } else {
      printf("No socktype specified");
    }

    //conversion to string
    inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
    printf("%s: %s\n", ipver, ipstr);


    printf("Sock Type: %s\n", socktype);
    
  }

  freeaddrinfo(res);
}
