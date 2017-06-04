/* 
 * $Id: irc.h,v 1.10 2004/09/27 09:21:29 mmr Exp $
 */

/* Includes */
#include <stdio.h>
#include <stdlib.h> /* malloc, free */
#include <string.h> /* strtok, strncmp */
#include <sys/types.h>  /* regex */
#include <regex.h>  /* reg* */
#include <ctype.h>  /* toupper */

/* Constants */
#define b1n_QUIT_MESSAGE  "See ya later alligator!"

/* Includes */
#include "sock.h"


/* IRC Line (line received from Server that starts with ':') */
typedef struct b1n_ircline {
  char  nick[32];
  char  user[32];
  char  host[32];
  char  cmd[32];  /* IRC Server Command (PRIVMSG, JOIN, PART, etc) */
  char  cmdp[32]; /* Command Parameter */
  char  msg[BUFSIZ];  /* Message */
} b1n_ircline;

/* Declaring IRC Structure/Object */
typedef struct b1n_irc {
  /* Vars */
  char server[32];  /* IRC Server */
  unsigned short port;  /* IRC Server Port */
  b1n_sock sock;    /* Socket Object */

  /* Methods */
  int   (* b1n_ircConnect)(struct b1n_irc*);  /* Connect to Server */
  int   (* b1n_ircIdentify)(struct b1n_irc*, const char*, const char*, const char*, const char*);  /* Identify Bot */
  void  (* b1n_ircCheckCmds)(struct b1n_irc*);  /* Check for IRC Commands */
  void  (* b1n_ircCheckCmd)(struct b1n_irc*, const char*);  /* Check one command (not PING) */
  void  (* b1n_ircDisconnect)(struct b1n_irc*);  /* Disconnect from Server */

  void  (* b1n_ircChanJoin)(struct b1n_irc*, const char*);  /* Join Channel */
  void  (* b1n_ircChanPart)(struct b1n_irc*, const char*);  /* Part Channel */
  void  (* b1n_ircMsgChan)(struct b1n_irc*, const char*, const char*);
  void  (* b1n_ircMsgPerson)(struct b1n_irc*, const char*, const char*);

    /* Treat commands got from an ircline */
  void  (* b1n_ircGotPRIVMSG)(struct b1n_irc*, const b1n_ircline);
  void  (* b1n_ircGotNICK)(struct b1n_irc*, const b1n_ircline);
  void  (* b1n_ircGotJOIN)(struct b1n_irc*, const b1n_ircline);
  void  (* b1n_ircGotPART)(struct b1n_irc*, const b1n_ircline);
  void  (* b1n_ircGotQUIT)(struct b1n_irc*, const b1n_ircline);
  void  (* b1n_ircGotMODE)(struct b1n_irc*, const b1n_ircline);
  void  (* b1n_ircGotTOPIC)(struct b1n_irc*, const b1n_ircline);

  void  (* b1n_ircGotCTCP)(struct b1n_irc*, const b1n_ircline, const char*, const char*);
  void  (* b1n_ircGotBOTEVENT)(struct b1n_irc*, const b1n_ircline, const char*, const char*);

    /* Replies */
  void  (* b1n_ircCTCPReply)(struct b1n_irc*, const char*, const char*);
  void  (* b1n_ircReplyPing)(struct b1n_irc*, const char*);
  void  (* b1n_ircReplyCTCPFinger)(struct b1n_irc*, const char*);
  void  (* b1n_ircReplyCTCPTime)(struct b1n_irc*, const char*);
  void  (* b1n_ircReplyCTCPUserinfo)(struct b1n_irc*, const char*);
  void  (* b1n_ircReplyCTCPVersion)(struct b1n_irc*, const char*);
  void  (* b1n_ircReplyCTCPClientinfo)(struct b1n_irc*, const char*, const char*);
  void  (* b1n_ircReplyCTCPPing)(struct b1n_irc*, const char*, const char*);
  void  (* b1n_ircReplyCTCPAction)(struct b1n_irc*, const char*, const char*);
  void  (* b1n_ircReplyCTCPError)(struct b1n_irc*, const char*, const char*);
} b1n_irc;

/* Prototypes */
  /* IRC Object */
b1n_irc b1n_ircNew(void); /* Constructor */
int   b1n_ircConfigure(b1n_irc*);
int   b1n_ircConnect(b1n_irc*);
int   b1n_ircIdentify(b1n_irc*, const char*, const char*, const char*, const char*);
void  b1n_ircCheckCmds(b1n_irc*);
void  b1n_ircCheckCmd(b1n_irc*, const char*);
void  b1n_ircDisconnect(b1n_irc*);

void  b1n_ircChanJoin(b1n_irc*, const char*);
void  b1n_ircChanPart(b1n_irc*, const char*);
void  b1n_ircMsgChan(b1n_irc*, const char*, const char*);
void  b1n_ircMsgPerson(b1n_irc*, const char*, const char*);

  /* IRC Commands found at ircline->cmd */ 
void  b1n_ircGotPRIVMSG(b1n_irc*, const b1n_ircline);
void  b1n_ircGotNICK(b1n_irc*, const b1n_ircline);
void  b1n_ircGotJOIN(b1n_irc*, const b1n_ircline);
void  b1n_ircGotPART(b1n_irc*, const b1n_ircline);
void  b1n_ircGotQUIT(b1n_irc*, const b1n_ircline);
void  b1n_ircGotMODE(b1n_irc*, const b1n_ircline);
void  b1n_ircGotTOPIC(b1n_irc*, const b1n_ircline);

void  b1n_ircGotBOTEVENT(b1n_irc*, const b1n_ircline, const char*, const char*);
void  b1n_ircGotCTCP(b1n_irc*, const b1n_ircline, const char*, const char*);
void  b1n_ircGotCTCP(b1n_irc*, const b1n_ircline, const char*, const char*);

  /* Replies */
void  b1n_ircCTCPReply(b1n_irc*, const char*, const char*);
void  b1n_ircReplyPing(b1n_irc*, const char*);
void  b1n_ircReplyCTCPFinger(b1n_irc*, const char*);
void  b1n_ircReplyCTCPTime(b1n_irc*, const char*);
void  b1n_ircReplyCTCPUserinfo(b1n_irc*, const char*);
void  b1n_ircReplyCTCPVersion(b1n_irc*, const char*);
void  b1n_ircReplyCTCPClientinfo(b1n_irc*, const char*, const char*);
void  b1n_ircReplyCTCPPing(b1n_irc*, const char*, const char*);
void  b1n_ircReplyCTCPAction(b1n_irc*, const char*, const char*);
void  b1n_ircReplyCTCPError(b1n_irc*, const char*, const char*);
