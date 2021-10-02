/* Ugly Windows' Winsock includes. */
#pragma warning(push)
#pragma warning(disable:4005)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

/* Required, for some reason. */
int _wcsicmp(const unsigned short *, const unsigned short *);

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma warning(pop)

#include <engine.h>
#include <net/net_udp.h>
#include <libc/stdlib.h>
#include <libc/string.h>

struct udp_socket {
    uint64_t sock;
    size_t ai_addrlen;
    struct sockaddr_storage ai_addr;
    char ip[INET_ADDRSTRLEN];
    char port[5];
};

/* ========================================================================= */
/* Helper Functions                                                          */
/* ========================================================================= */
static void *
get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


/* ========================================================================= */
/* Functions                                                                 */
/* ========================================================================= */
struct udp_socket *
udp_open(char const *ip, char const *port)
{
    int status;
    struct addrinfo hints;
    struct addrinfo *res, *p;
    struct udp_socket *sock;

    dbg_assert(ip != NULL, "IP must not be NULL");
    dbg_assert(port != NULL, "PORT must not be NULL");

    if ((sock = mem_calloc(sizeof(struct udp_socket))) == NULL)
        return NULL;
    memcpy(sock->ip, ip, strlen(ip) + 1);
    memcpy(sock->port, port, strlen(port) + 1);

    memset(&hints, 0x0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    if ((status = getaddrinfo(ip, port, &hints, &res)) != 0) {
        ELOG(("Error: getaddrinfo(): %s", gai_strerror(status)));
        goto e0;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        sock->sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock->sock == INVALID_SOCKET)
            continue;
        break;
    }

    if (p == NULL) {
        ELOG(("Client: Connection refused"));
        goto e1;
    }

    sock->ai_addrlen = p->ai_addrlen;
    memcpy(&sock->ai_addr, p->ai_addr, p->ai_addrlen);

    freeaddrinfo(res);
    return sock;

e1: freeaddrinfo(res);
e0: mem_free(sock);
    return NULL;
}

struct udp_socket *
udp_listen(char const *ip, char const *port)
{
    int status;
    struct addrinfo hints;
    struct addrinfo *res, *p;
    struct udp_socket *sock;

    dbg_assert(port != NULL, "PORT must not be NULL");

    if ((sock = mem_calloc(sizeof(struct udp_socket))) == NULL)
        return NULL;
    if (ip)
        memcpy(sock->ip, ip, strlen(ip) + 1);
    memcpy(sock->port, port, strlen(port) + 1);

    memset(&hints, 0x0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    if (ip == NULL)
        hints.ai_flags = AI_PASSIVE;
    if ((status = getaddrinfo(ip, port, &hints, &res)) != 0) {
        ELOG(("Error: getaddrinfo(): %s", gai_strerror(status)));
        goto e0;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        sock->sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock->sock == INVALID_SOCKET)
            continue;

        if (bind(sock->sock, p->ai_addr, (int)p->ai_addrlen) == -1) {
            ELOG(("Error: bind(): %d", WSAGetLastError()));
            closesocket(sock->sock);
            continue;
        }

        break;
    }

    if (p == NULL) {
        ELOG(("Server: Failed to bind port %s", port));
        goto e1;
    }

    inet_ntop(AF_INET, get_in_addr(p->ai_addr), sock->ip, INET_ADDRSTRLEN);
    ILOG(("Server: Listen on %s:%s", sock->ip, sock->port));

    freeaddrinfo(res);
    return sock;

e1: freeaddrinfo(res);
e0: mem_free(sock);
    return NULL;
}

void
udp_close(struct udp_socket *sock)
{
    closesocket(sock->sock);
    mem_free(sock);
}

void
udp_setblocking(struct udp_socket *sock, int blocking)
{
    int ret;
    u_long arg = !blocking;

    ret = ioctlsocket(sock->sock, FIONBIO, &arg);
    if (ret != NO_ERROR)
        ELOG(("Error: udp_setblocking() failed: %d", ret));
}

int
udp_send(struct udp_socket *sock, void *buf, usize len)
{
    int ret = sendto(sock->sock, buf, (int)len, 0,
            (struct sockaddr *)&sock->ai_addr, (int)sock->ai_addrlen);
    if (ret != len)
        WLOG(("Partially sent packet: %d of %d bytes", ret, (int)len));
    return ret;
}

int
udp_send_s(struct udp_socket *sock, void *buf, usize len)
{
    int n = 0;
    int total = 0;

    while (total < len) {
        n = sendto(sock->sock, (char*)buf + total, (int)len - total, 0,
                (struct sockaddr *)&sock->ai_addr, (int)sock->ai_addrlen);
        if (n == -1)
            break;
        total += n;
    }

    return n == -1;
}

int
udp_recv(struct udp_socket *sock, void *buf, usize len)
{
    int ret = recvfrom(sock->sock, buf, (int)len, 0,
            (struct sockaddr *)&sock->ai_addr, (int *)&sock->ai_addrlen);
    if (ret == 0) {
        WLOG(("Connection has been gracefully closed"));
    }

    return ret;
}

