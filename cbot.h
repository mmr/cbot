/* 
 * $Id: cbot.h,v 1.7 2004/09/27 09:21:29 mmr Exp $
 */

/* Includes */
#include <stdio.h>
#include <string.h> /* snprintf */
#include <stdlib.h> /* malloc, free, exit */
#include <time.h>   /* time, localtime */
#include <signal.h> /* signal */

/* Constants */
#define b1n_CONFIG_FILE   "cbot.conf"

/* Channel */
typedef struct b1n_chan {
  char name[32];
  struct b1n_chan* next;  
} b1n_chan;

/* IRC Structure/Object */
typedef struct b1n_bot {
  /* Vars */
  char user[32];  /* Bot User */
  char name[32];  /* Bot Name */
  char nick[32];  /* Bot Nick */
  char pass[32];  /* Bot Password */
  b1n_irc   irc;    /* IRC Object */
  b1n_chan* chans;  /* Channels */

  time_t conn_start;  /* Connection Start */
  time_t conn_end;    /* Connection End */

  /* Methods */
  int   (* b1n_botConnect)(struct b1n_bot*);    /* Connect Bot    */
  int   (* b1n_botConfigure)(struct b1n_bot*);  /* Configure Bot  */
  int   (* b1n_botIdentify)(struct b1n_bot*);   /* Identify Bot   */
  void  (* b1n_botSignals)(struct b1n_bot*);    /* Configure Signals */
  int   (* b1n_botJoinChannels)(struct b1n_bot*); /* Join Channels */
  void  (* b1n_botCheckCmds)(struct b1n_bot*);  /* Check IRC Commands */
  void  (* b1n_botDisconnect)(struct b1n_bot*); /* Disconnect from Server */
  void  (* b1n_botTerminate)(void); /* For signals */
} b1n_bot;

/* Prototypes */
b1n_bot b1n_botNew(void); /* Constructor */
int   b1n_botConfigure(b1n_bot*);
int   b1n_botConnect(b1n_bot*);
int   b1n_botIdentify(b1n_bot*);
void  b1n_botSignals(b1n_bot*);
int   b1n_botJoinChannels(b1n_bot*);
void  b1n_botCheckCmds(b1n_bot*);
void  b1n_botDisconnect(b1n_bot*);
void  b1n_botTerminate(void);
void  b1n_bail(int, const char*);
