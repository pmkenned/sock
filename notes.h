#if NOTES

#include <netinet/in.h>

/* pg 14 */

htons();
htonl();
ntohs();
ntohl();

/* pg 15 */

struct addrinfo {
    int ai_flags;               /* AI_PASSIVE, AI_CANONNAME, etc. */
    int ai_family;              /* AF_INET, AF_INET6, AF_UNSPEC */
    int ai_socktype;            /* SOCK_STREAM, SOCK_DGRAM */
    int ai_protocol;            /* use 0 for "any" */
    size_t ai_addrlen;          /* size of ai_addr in bytes */
    struct sockaddr *ai_addr;   /* struct sockaddr_in or _in6 */
    char *ai_canonname;         /* full canonical hostname */
    struct addrinfo *ai_next;   /* linked list, next node */
};

getaddrinfo();

struct sockaddr {
    unsigned short sa_family;   /* address family, AF_xxx */
    char sa_data[14];           /* 14 bytes of protocol address */
};

/* NOTE: a pointer to a struct sockaddr_in can be cast to a pointer to a struct sockaddr and vice-versa */

/* (IPv4 only--see struct sockaddr_in6 for IPv6) */
struct sockaddr_in {
    short int sin_family;           /* Address family, AF_INET (must be Network byte order) */
    unsigned short int sin_port;    /* Port number */
    struct in_addr sin_addr;        /* Internet address */
    unsigned char sin_zero[8];      /* Same size as struct sockaddr */
};

/* pg 16 */

struct in_addr {
    uint32_t s_addr; /* that's a 32-bit int (4 bytes) */
};

/* (IPv6 only--see struct sockaddr_in and struct in_addr for IPv4) */
struct sockaddr_in6 {
    u_int16_t sin6_family;      /* address family, AF_INET6 */
    u_int16_t sin6_port;        /* port number, Network Byte Order */
    u_int32_t sin6_flowinfo;    /* IPv6 flow information */
    struct in6_addr sin6_addr;  /* IPv6 address */
    u_int32_t sin6_scope_id;    /* Scope ID */
};

struct in6_addr {
    unsigned char s6_addr[16];  /* IPv6 address */
};

/*  designed to be large enough to hold both IPv4 and IPv6 structures */
struct sockaddr_storage {
    sa_family_t ss_family;      /* address family */

    /* all this is padding, implementation specific, ignore it: */
    char __ss_pad1[_SS_PAD1SIZE];
    int64_t __ss_align;
    char __ss_pad2[_SS_PAD2SIZE];
};

/* pg 17 */

/* converts an IP address in numbers-and-dots notation into either a struct in addr
   or a struct in6_addr */
inet_pton(); /* presentation to network */
inet_ntop(); /* network to presentation */

struct sockaddr_in sa; /* IPv4 */
struct sockaddr_in6 sa6; /* IPv6 */
/* NOTE: do error checking on these */
inet_pton(AF_INET, "10.12.110.57", &(sa.sin_addr)); /* IPv4 */
inet_pton(AF_INET6, "2001:db8:63b3:1::3490", &(sa6.sin6_addr)); /* IPv6 */
/* see pg 17 for examples */

/* see pg 19 for "Jumping from IPv4 to IPv6" */

/* pg 21 */

/* pg 25 */

int s;
struct addrinfo hints, *res;
// do the lookup
// [pretend we already filled out the "hints" struct]
getaddrinfo("www.example.com", "http", &hints, &res);
// again, you should do error-checking on getaddrinfo(), and walk
// the "res" linked list looking for valid entries instead of just
// assuming the first one is good (like many of these examples do).
// See the section on client/server for real examples.
s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

int bind(int sockfd, struct sockaddr *my_addr, int addrlen);

/* pg 27 */

int connect(int sockfd, struct sockaddr *serv_addr, int addrlen);

/* pg 28 */

int listen(int sockfd, int backlog);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

/* pg 29 */

int send(int sockfd, const void *msg, int len, int flags);
int recv(int sockfd, void *buf, int len, int flags);

/* pg 30 */

/* for DGRAM */

int sendto(int sockfd, const void *msg, int len, unsigned int flags, const struct sockaddr *to, socklen_t tolen);
int recvfrom(int sockfd, void *buf, int len, unsigned int flags, struct sockaddr *from, int *fromlen);

/* pg 32 */

int getpeername(int sockfd, struct sockaddr *addr, int *addrlen);
int gethostname(char *hostname, size_t size);

#endif
