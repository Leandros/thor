/*!
 * \file net_tcp.h
 * \author Arvid Gerstmann
 * \date Jan 2017
 * \brief Low-Level TCP Connection routines.
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */

#ifndef NET_TCP_H
#define NET_TCP_H

#include <sys/sys_types.h>

struct tcp_socket;

/*!
 * \brief Open a TCP Connection.
 * \param ip IP Address in '.' (dot) notation, e.g "127.0.0.1"
 * \param port Destination port, should be higher than 1024
 * \return Pointer to TCP socket, or NULL on failure.
 */
struct tcp_socket *
tcp_open(char const *ip, char const *port);

/*!
 * \brief Start listening on TCP IP:PORT
 * \param ip IP Address in '.' (dot) notation, e.g "0.0.0.0" or NULL if host ip
 * \param port Port to listen on
 * \return Pointer to TCP socket, or NULL on failure.
 */
struct tcp_socket *
tcp_listen(char const *ip, char const *port);

/*!
 * \brief Close a TCP Connection.
 * \param sock Pointer to opened TCP socket.
 */
void
tcp_close(struct tcp_socket *sock);

/*!
 * \brief Set a TCP connection to non-blocking or blocking.
 * \param sock Pointer to opened TCP socket.
 * \param blocking Wheter blocking is enabled
 */
void
tcp_setblocking(struct tcp_socket *sock, int blocking);

/*!
 * \brief Send a packet via TCP
 * \param sock Pointer to opened TCP socket.
 * \param buf Pointer to buffer.
 * \param len Size of buffer in bytes.
 * \return Number of bytes sent.
 * \remark Attempts to always send the full data, even if it requires
 *         multiple packets.
 */
int
tcp_send(struct tcp_socket *sock, void *buf, usize len);

/*!
 * \brief Send a packet via TCP
 * \param sock Pointer to opened TCP socket.
 * \param buf Pointer to buffer.
 * \param len Size of buffer in bytes.
 * \return 0 on success, non-zero on failure.
 * \remark Attempts to always send the full data, even if it requires
 *         multiple packets.
 */
int
tcp_send_s(struct tcp_socket *sock, void *buf, usize len);

/*!
 * \brief Receive a TCP packet.
 * \param sock Pointer to opened TCP socket.
 * \param buf Pointer to buffer.
 * \param len Size of buffer in bytes.
 * \return Number of bytes written into \c buf, or 0 if connection was closed,
 *         or -1 on error.
 */
int
tcp_recv(struct tcp_socket *sock, void *buf, usize len);

#endif /* NET_TCP_H */

