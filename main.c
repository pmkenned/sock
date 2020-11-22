#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#elif WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#if WIN32
void win32_init_code()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        exit(1);
    }
}
#endif

int main(int argc, char * argv[])
{

#if WIN32
    win32_init_code();
#endif

    struct addrinfo * res, * res_ptr;
    struct addrinfo hints;
    int status;
#if LINUX
    char ipstr[INET6_ADDRSTRLEN];
#endif

    if (argc != 2) {
        fprintf(stderr, "usage: showip hostname\n");
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    printf("IP Addresses for %s:\n\n", argv[1]);

    for (res_ptr = res; res_ptr != NULL; res_ptr = res_ptr->ai_next) {

        void * addr;
        char * ipver;

        if (res_ptr->ai_family == AF_INET) {
            struct sockaddr_in * ipv4 = (struct sockaddr_in *) res_ptr->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else {
            struct sockaddr_in6 * ipv6 = (struct sockaddr_in6 *) res_ptr->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

#if WIN32
        struct in_addr * ia_ptr = addr;
        if (res_ptr->ai_family == AF_INET)
            printf("  %s: %s\n", ipver, inet_ntoa(*ia_ptr));
        else
            printf("cannot use inet_ntoa for IPv6 addresses\n");
#else
        inet_ntop(res_ptr->ai_family, addr, ipstr, sizeof(ipstr));
        printf("  %s: %s\n", ipver, ipstr);
#endif
    }
    printf("\n");

    freeaddrinfo(res);

#if WIN32
    WSACleanup();
#endif

    return 0;
}
