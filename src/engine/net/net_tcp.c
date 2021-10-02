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
#include <net/net_tcp.h>
#include <libc/stdlib.h>
#include <libc/string.h>

struct tcp_socket {
    uint64_t sock;
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
struct tcp_socket *
tcp_open(char const *ip, char const *port)
{
    char tmp[INET_ADDRSTRLEN];
    int status;
    struct addrinfo hints;
    struct addrinfo *res, *p;
    struct tcp_socket *sock;

    dbg_assert(ip != NULL, "IP must not be NULL");
    dbg_assert(port != NULL, "PORT must not be NULL");

    if ((sock = mem_calloc(sizeof(struct tcp_socket))) == NULL)
        return NULL;
    memcpy(sock->ip, ip, strlen(ip) + 1);
    memcpy(sock->port, port, strlen(port) + 1);

    memset(&hints, 0x0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if ((status = getaddrinfo(ip, port, &hints, &res)) != 0) {
        ELOG(("Error: getaddrinfo(): %s", gai_strerror(status)));
        goto e0;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        sock->sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock->sock == INVALID_SOCKET)
            continue;

        if (connect(sock->sock, p->ai_addr, (int)p->ai_addrlen) == -1) {
            ELOG(("Error: connect(): %d", WSAGetLastError()));
            closesocket(sock->sock);
            continue;
        }

        break;
    }

    if (p == NULL) {
        ELOG(("Client: Connection refused"));
        goto e1;
    }

    inet_ntop(AF_INET, get_in_addr(p->ai_addr), tmp, INET_ADDRSTRLEN);
    ILOG(("Client: Connected to %s", tmp));

    freeaddrinfo(res);
    return sock;

e1: freeaddrinfo(res);
e0: mem_free(sock);
    return NULL;
}

struct tcp_socket *
tcp_listen(char const *ip, char const *port)
{
    int status;
    struct addrinfo hints;
    struct addrinfo *res, *p;
    struct tcp_socket *sock;

    dbg_assert(port != NULL, "PORT must not be NULL");

    if ((sock = mem_calloc(sizeof(struct tcp_socket))) == NULL)
        return NULL;
    if (ip)
        memcpy(sock->ip, ip, strlen(ip) + 1);
    memcpy(sock->port, port, strlen(port) + 1);

    memset(&hints, 0x0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
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

    if (listen(sock->sock, 10) == -1) {
        ELOG(("Server: Failed to listen on %s:%s", sock->ip, sock->port));
        goto e1;
    }

    ILOG(("Server: Listen on %s:%s", sock->ip, sock->port));

    freeaddrinfo(res);
    return sock;

e1: freeaddrinfo(res);
e0: mem_free(sock);
    return NULL;
}

void
tcp_close(struct tcp_socket *sock)
{
    closesocket(sock->sock);
    mem_free(sock);
}

void
tcp_setblocking(struct tcp_socket *sock, int blocking)
{
    int ret;
    u_long arg = !blocking;

    ret = ioctlsocket(sock->sock, FIONBIO, &arg);
    if (ret != NO_ERROR)
        ELOG(("Error: tcp_setblocking() failed: %d", ret));
}

int
tcp_send(struct tcp_socket *sock, void *buf, usize len)
{
    int ret = send(sock->sock, buf, (int)len, 0);
    if (ret != len)
        WLOG(("Partially sent packet: %d of %d bytes", ret, (int)len));
    return ret;
}

int
tcp_send_s(struct tcp_socket *sock, void *buf, usize len)
{
    int n = 0;
    int total = 0;

    while (total < len) {
        n = send(sock->sock, (char*)buf + total, (int)len - total, 0);
        if (n == -1)
            break;
        total += n;
    }

    return n == -1;
}

int
tcp_recv(struct tcp_socket *sock, void *buf, usize len)
{
    int ret = recv(sock->sock, buf, (int)len, 0);
    if (ret == 0) {
        WLOG(("Connection has been gracefully closed"));
    }

    return ret;
}

