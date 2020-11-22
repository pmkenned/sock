#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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

// remove leading and trailing whitespace, modify input string
// return pointer to string
char * trim_lr(char * s)
{
    while (*s == ' ')
        s++;
    char * p = s + strlen(s) - 1;
    while ((p >= s) && (*p == ' ' || *p == '\n'))
        *(p--) = '\0';
    return s;
}

// remove trailing whitespace, modify input string
// return number of removed characters
size_t chomp(char * s)
{
    char * last = s + strlen(s) - 1;
    char * p = last;
    while ((p >= s) && (*p == ' ' || *p == '\n'))
        *(p--) = '\0';
    return last-p;
}

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
#define MSG_CAP 1024

int main(int argc, char * argv[])
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

    // TODO: use parse_options
    char * node = "localhost";
    if (argc > 1) {
        node = argv[1];
    }

    if ((status = getaddrinfo(node, MYPORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo() error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

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
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *) &yes,sizeof yes) == -1) {
        perror("setsockopt() error");
        exit(EXIT_FAILURE);
    }

    printf("Connecting...");
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
#if MINGW
        printf("%d\n", WSAGetLastError());
#endif
        perror("connect() error");
        exit(EXIT_FAILURE);
    }
    printf("Connected!\n");

    printf("type 'help' for help, 'quit' to quit\n");

    char msg[MSG_CAP];
    for (printf("> "), fflush(stdout) ; fgets(msg, MSG_CAP, stdin) != NULL; printf("> "), fflush(stdout)) {
        // TODO: tokenize input
        // TODO: arrow keys
        char * msg_ = trim_lr(msg);
        if (strcmp(msg_, "help") == 0) {
            printf("help not yet implemented\n");
        } else if (strcmp(msg_, "quit") == 0) {
            break;
        }
        ssize_t msg_len = strlen(msg_)+1;
        ssize_t bytes_sent = send(sockfd, msg_, msg_len, 0);
        // TODO: apparently send() might not send all bytes; handle this case
        assert(bytes_sent == msg_len);
        if (bytes_sent < 0) {
            perror("send() error");
            exit(EXIT_FAILURE);
        }
    }

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
