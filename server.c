#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if LINUX || CYGWIN || OSX
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#elif MINGW
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#if MINGW
void win32_init_code()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        exit(EXIT_FAILURE);
    }
}
#endif

#define MYPORT "3490"
#define BACKLOG 10

//int main(int argc, char * argv[])
int main()
{

#if MINGW
    win32_init_code();
#endif

    struct addrinfo * res;
    struct addrinfo hints;
    int status;
#if MINGW
    SOCKET sockfd;
#else
    int sockfd;
#endif

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // 127.0.0.1
    // localhost
    // NULL 
    char * node = "0.0.0.0"; // needed for connections from outside localhost

    if ((status = getaddrinfo(node, MYPORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo() error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    // TODO: loop over res

#if MINGW
    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == INVALID_SOCKET) {
        printf("%d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
#else
    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }
#endif

    int yes = 1;
    if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, (void *) &yes,sizeof yes) == -1) {
        perror("setsockopt() error");
        exit(EXIT_FAILURE);
    }

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("bind() error");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, BACKLOG) < 0) {
        perror("listen() error");
        exit(EXIT_FAILURE);
    }

    // TODO: spawn new thread for each client that connects

    struct sockaddr_storage their_addr;
    socklen_t addrlen = sizeof(struct sockaddr_storage);
    int new_socket;
    if ((new_socket = accept(sockfd, (struct sockaddr *) &their_addr, &addrlen)) < 0) {
        perror("accept() error");
        exit(EXIT_FAILURE);
    }

    void * addr;

    if (their_addr.ss_family == AF_INET) {
        struct sockaddr_in * ipv4 = (struct sockaddr_in *) &their_addr;
        addr = &(ipv4->sin_addr);
    } else {
        struct sockaddr_in6 * ipv6 = (struct sockaddr_in6 *) &their_addr;
        addr = &(ipv6->sin6_addr);
    }

#if MINGW
    struct in_addr * ia_ptr = addr;
    if (res->ai_family == AF_INET)
        printf("New connection: %s\n", inet_ntoa(*ia_ptr));
    else if (res->ai_family == AF_INET6)
        printf("cannot use inet_ntoa for IPv6 addresses\n");
#else
    char ipstr[INET6_ADDRSTRLEN];
    inet_ntop(their_addr.ss_family, addr, ipstr, sizeof(ipstr));
    printf("New connection: %s\n", ipstr);
#endif

    char buf[1024];

    int recv_rv;
    while ( (recv_rv = recv(new_socket, buf, sizeof(buf), 0)) != 0) {
        if (recv_rv < 0) {
            perror("recv() error");
            exit(EXIT_FAILURE);
        }
        printf("%s\n", buf);
        fflush(stdout);
    }

    printf("client disconnected\n");
    fflush(stdout);

#if MINGW
    closesocket(new_socket);
#else
    close(new_socket);
#endif

#if MINGW
    closesocket(sockfd);
#else
    close(sockfd);
#endif

    freeaddrinfo(res);

#if MINGW
    WSACleanup();
#endif

    return 0;
}
