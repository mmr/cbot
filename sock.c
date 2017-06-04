/*
 * $Id: sock.c,v 1.13 2006/01/12 22:32:39 mmr Exp $
 */

#include "sock.h"
#include "misc.h"

b1n_sock
b1n_sockNew(void)
{
  b1n_sock this;

#ifdef DEBUG  
  printf("Creating Socket\n");
#endif

  /* Zeroing byte counters */
  this.bytes_received = 0;
  this.bytes_sent = 0;

  /* Setting function pointers */ 
  this.b1n_sockConnect  = b1n_sockConnect;
  this.b1n_sockWrite  = b1n_sockWrite;
  this.b1n_sockRead   = b1n_sockRead;
  this.b1n_sockClose  = b1n_sockClose;

  /* Creating socket file descriptor */
  this.fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  /* Returning socket object */
  return this;
}

int
b1n_sockConnect(b1n_sock* this, const char* server, unsigned short port)
{
  struct hostent      *host;
  struct sockaddr_in  addr;

  /* Resolving Host Name */
  if((host = gethostbyname(server)) != NULL){
    /* Cleaning addr */
    memset(&addr, 0, sizeof(addr));

    /* Setting connection structure */
      /* Family */
    addr.sin_family = host->h_addrtype;

      /* Port */
    addr.sin_port   = htons(port);

      /* IP Address */
      /* TODO: get a random ip instead always [0] */
    memcpy(&addr.sin_addr, host->h_addr_list[0], sizeof(addr.sin_addr));

#ifdef DEBUG
    printf("Connecting to %s %s:%d\n", 
      server, inet_ntoa(*((struct in_addr *) host->h_addr_list[0])), port);
#endif

    /* Connecting */
    /* connect returns -1 on error */
    return
      connect(this->fd, (struct sockaddr*) &addr, sizeof(addr)) != -1;
  }
  return b1n_FALSE;
}

int
b1n_sockWrite(b1n_sock* this, char* str)
{
  int   ret;
  int   len = strlen(str);

  /* send returns -1 on error */
  /* -1 is due to \0 (IRC Commands are not terminated with \0) */
  if((ret = send(this->fd, str, len-1, 0)) != -1){
#ifdef DEBUG    
    printf("WRITE (%d): %s", ret, str);
#endif
    this->bytes_sent += ret;
    return b1n_TRUE;
  }
  return b1n_FALSE;
}

int
b1n_sockRead(b1n_sock* this, char* str)
{
  int  ret = 0;

  memset(str, 0, BUFSIZ);
  if((ret = recvfrom(this->fd, str, BUFSIZ, 0, NULL, NULL)) != -1){
#ifdef DEBUG
    printf("READ (%d): %s", ret, str);
#endif
    this->bytes_received += ret;
  }
  return ret;
}

int
b1n_sockClose(b1n_sock* this)
{
  /* close returns -1 on error */
  return close(this->fd) != -1;
}
