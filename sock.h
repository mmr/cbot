/* 
 * $Id: sock.h,v 1.8 2006/01/12 22:32:39 mmr Exp $
 */

/* Includes */
#include <stdio.h>
#include <unistd.h>     /* close */
#include <string.h>     /* strlen, snprintf, memset */

#include <sys/types.h>  /* socket, htons, connect, send, recv */
#include <sys/socket.h> /* socket, connect, send, recv */
#include <netinet/in.h> /* IPPROTO_TCP, sockaddr_in */
#include <netdb.h>      /* gethostbyname */
#include <arpa/inet.h>  /* inet_ntoa */

/* Types */
typedef struct b1n_sock {
  /* Vars */
  int fd; /* Socket File Handler */
  unsigned long long bytes_sent; /* Bytes sent */
  unsigned long long bytes_received; /* Bytes sent */

  /* Methods */
  int (* b1n_sockConnect)(struct b1n_sock*, const char*, unsigned short);  /* Connect to Remote Host */
  int (* b1n_sockWrite)(struct b1n_sock*, char*); /* Write to the Socket  */
  int (* b1n_sockRead)(struct  b1n_sock*, char*); /* Read from the Socket */
  int (* b1n_sockClose)(struct b1n_sock*);  /* Close Socket */
} b1n_sock;

/* Prototypes */
b1n_sock b1n_sockNew(void);
int b1n_sockConnect(b1n_sock*, const char*, unsigned short);
int b1n_sockWrite(b1n_sock*, char*);
int b1n_sockRead(b1n_sock*,  char*);
int b1n_sockClose(b1n_sock*);
