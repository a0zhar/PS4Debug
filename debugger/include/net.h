#ifndef _NET_H
#define _NET_H

#include <ps4.h>
#include "errno.h"

#define NET_MAX_LENGTH  8192

#define SO_USELOOPBACK 0x0040     // bypass hardware when possible 
#define SO_LINGER      0x0080     // linger on close if data present 
#define SO_NOSIGPIPE   0x0800     // no SIGPIPE from EPIPE 
#define SO_SNDBUF      0x1001     // send buffer size 
#define SO_RCVBUF      0x1002     // receive buffer size 
#define SO_USELOOPBACK 0x0040     // bypass hardware when possible 
#define SO_LINGER      0x0080     // linger on close if data present 
#define SO_NOSIGPIPE   0x0800     // no SIGPIPE from EPIPE 
#define SO_SNDBUF      0x1001     // send buffer size 
#define SO_RCVBUF      0x1002     // receive buffer size 
#define SO_DEBUG       0x00000001 // turn on debugging info recording 
#define SO_ACCEPTCONN  0x00000002 // socket has had listen() 
#define SO_REUSEADDR   0x00000004 // allow local address reuse 
#define SO_KEEPALIVE   0x00000008 // keep connections alive 
#define SO_DONTROUTE   0x00000010 // just use interface addresses 
#define SO_BROADCAST   0x00000020 // permit sending of broadcast msgs 


// NOTES: I would like to move away from the stupid sony wrapper functions
// They do not always return what I expect and I want to use straight syscalls

#define FD_SETSIZE 1024

typedef unsigned long fd_mask;
typedef struct fd_set_t {
    fd_mask fds_bits[FD_SETSIZE / 8 / sizeof(long)];
} fd_set;

void FD_ZERO(fd_set *set);
void FD_SET(int d, fd_set *set);
void FD_CLR(int d, fd_set *set);
int FD_ISSET(int d, fd_set *set);

int net_select(int fd, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
int net_send_data(int fd, void *data, int length);
int net_recv_data(int fd, void *data, int length, int force);
int net_send_status(int fd, uint32_t status);

#endif
