/* $Id: cbot.c,v 1.14 2004/09/27 09:21:29 mmr Exp $ */

#include "irc.h"
#include "cbot.h"
#include "misc.h"

int
main(int argc, char* argv[])
{
  /* Creating Bot Object */
  b1n_bot this = b1n_botNew();

  if(this.b1n_botConfigure(&this)){     /* Configuring  */
    if(this.b1n_botConnect(&this)){     /* Connecting   */
      if(this.b1n_botIdentify(&this)){  /* Identifying  */
        this.b1n_botSignals(&this);     /* Signals      */
        if(this.b1n_botJoinChannels(&this)){ /* Joining Channels */
          this.b1n_botCheckCmds(&this); /* Endless loop to Check for Commands */
        } else b1n_bail(b1n_FAIL, "Could not Join Channels.");
      } else b1n_bail(b1n_FAIL, "Could not Identify Bot.");
    } else b1n_bail(b1n_FAIL, "Could not Connect.");
  } else b1n_bail(b1n_FAIL, "Could not Configure Bot.");

  /* Should not be reached */
  return b1n_FAIL;
}

/* Constructor */
b1n_bot
b1n_botNew(void)
{
  b1n_bot this;

  /* Configuring IRC Object */
  this.irc = b1n_ircNew();

  /* Configuring function pointers */
  this.b1n_botConfigure  = b1n_botConfigure;
  this.b1n_botConnect  = b1n_botConnect;
  this.b1n_botIdentify = b1n_botIdentify;
  this.b1n_botSignals  = b1n_botSignals;
  this.b1n_botJoinChannels = b1n_botJoinChannels;
  this.b1n_botCheckCmds  = b1n_botCheckCmds;
  this.b1n_botDisconnect = b1n_botDisconnect;
  this.b1n_botTerminate  = b1n_botTerminate;

  /* Returning Bot Object */
  return this;
}

/* Get data from configuration file */
/* TODO: this function needs to be reworked, maybe use an array of void* with ENUM */
int
b1n_botConfigure(b1n_bot* this)
{
  char  buf[BUFSIZ];  /* Buffer for data */
  b1n_chan* chan;     /* Auxiliar Channel */
  char* aux;          /* Auxiliar var */
  FILE* conf;         /* Configuration File */

  /* Opening configuration from file */
  if((conf = fopen(b1n_CONFIG_FILE, "r")) != NULL){
    /* Getting data */
    if(fgets(buf, sizeof(buf), conf) != NULL){
      /* Setting data */
        /* Server */
      snprintf(this->irc.server, sizeof(this->irc.server), "%s", strtok(buf, ":"));

        /* Port */
      this->irc.port = atoi(strtok(NULL, ":"));

        /* Bot User */
      snprintf(this->user, sizeof(this->user), "%s", strtok(NULL, ":"));

        /* Bot Name */
      snprintf(this->name, sizeof(this->name), "%s", strtok(NULL, ":"));

        /* Bot Nick */
      snprintf(this->nick, sizeof(this->nick), "%s", strtok(NULL, ":"));

        /* Bot Pass */
      snprintf(this->pass, sizeof(this->pass), "%s", strtok(NULL, ":"));

        /* Channels */
      aux = strtok(NULL, ":");
      this->chans = chan = (b1n_chan *) malloc(sizeof(b1n_chan));
      for(aux = strtok(aux,  ","); aux;
          aux = strtok(NULL, ","))
      {
        snprintf(chan->name, sizeof(chan->name), "%s", aux);

        chan->next = (b1n_chan *) malloc(sizeof(b1n_chan));
        chan = chan->next;
      }

      return b1n_TRUE;
    } else b1n_bail(b1n_FAIL, "Getting data.");
  } else b1n_bail(b1n_FAIL, "Opening config file.");

  return b1n_FALSE;
}

int
b1n_botConnect(b1n_bot* this)
{
  if(this->irc.b1n_ircConnect(&this->irc)){
    this->conn_start = time(NULL);
    return b1n_TRUE;
  }

  return b1n_FALSE;
}

int
b1n_botIdentify(b1n_bot* this)
{
  return this->irc.b1n_ircIdentify(&this->irc, this->user, this->name, this->nick, this->pass);
}

int
b1n_botJoinChannels(b1n_bot* this)
{
  b1n_chan* chan;

  chan = this->chans;
  while(chan->next != NULL){
    this->irc.b1n_ircChanJoin(&this->irc, chan->name);
    chan = chan->next;
  }
  return b1n_TRUE;
}

void
b1n_botDisconnect(b1n_bot* this)
{
  b1n_chan* chan;
  b1n_chan* next;
  char conn_start[BUFSIZ], conn_end[BUFSIZ];

  /* Freeing Memory */
  chan = this->chans;
  while(chan->next != NULL){
    next = chan->next;
    free(chan);
    chan = next;
  }

  this->conn_end = time(NULL);
  strftime(conn_start,  BUFSIZ, "%c", localtime(&this->conn_start));
  strftime(conn_end,    BUFSIZ, "%c", localtime(&this->conn_end));

  /* Statistics */
#ifdef DEBUG
  printf(
    "Connection Start:\t%s\n"
    "Connection   End:\t%s\n"
    "Bytes Sent:\t\t%llu\n"
    "Bytes Received:\t\t%llu\n",
    conn_start, conn_end,
    this->irc.sock.bytes_sent,
    this->irc.sock.bytes_received);
#endif

  /* Disconnecting from IRC Server */
  this->irc.b1n_ircDisconnect(&this->irc);
}

void
b1n_botCheckCmds(b1n_bot* this)
{
  /* Endless loop to check for IRC Commands */
  this->irc.b1n_ircCheckCmds(&this->irc);
}

void
b1n_bail(int n, const char* msg)
{
  fprintf(stderr, "%s\n", msg);
  exit(n);
}

b1n_bot* g_bot; /* Global var for signals (yes, ugh, I know...) */

void
b1n_botSignals(b1n_bot* this)
{
  /*
   * Ah, well... seems like I can't pass arguments
   * to a function triggered by a signal
   * so, the only way is to use a global var
   */
  g_bot = this;

  signal(SIGHUP,  SIG_IGN);
  signal(SIGINT,  (void *) this->b1n_botTerminate);
  signal(SIGTERM, (void *) this->b1n_botTerminate);
  signal(SIGSTOP, SIG_IGN);
}

void
b1n_botTerminate(void)
{
  g_bot->b1n_botDisconnect(g_bot);
}
