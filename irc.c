/* 
 * $Id: irc.c,v 1.16 2006/01/12 22:32:39 mmr Exp $
 */

/* Includes */
#include "irc.h"
#include "regex.h"
#include "misc.h"

/* Constructor */
b1n_irc
b1n_ircNew(void)
{
  b1n_irc this;

  /* Configuring Socket Object */
  this.sock = b1n_sockNew();

  /* Configuring function pointers */
  this.b1n_ircConnect   = b1n_ircConnect;
  this.b1n_ircIdentify  = b1n_ircIdentify;
  this.b1n_ircCheckCmd  = b1n_ircCheckCmd;
  this.b1n_ircCheckCmds = b1n_ircCheckCmds;
  this.b1n_ircDisconnect  = b1n_ircDisconnect;

  /* IRC Commands */
  this.b1n_ircMsgChan = b1n_ircMsgChan;
  this.b1n_ircMsgPerson = b1n_ircMsgPerson;
  this.b1n_ircChanJoin  = b1n_ircChanJoin;
  this.b1n_ircChanPart  = b1n_ircChanPart;

    /* Treat commands found in IRC lines */
  this.b1n_ircGotPRIVMSG  = b1n_ircGotPRIVMSG;
  this.b1n_ircGotNICK = b1n_ircGotNICK;
  this.b1n_ircGotJOIN = b1n_ircGotJOIN;
  this.b1n_ircGotPART = b1n_ircGotPART;
  this.b1n_ircGotQUIT = b1n_ircGotQUIT;
  this.b1n_ircGotMODE = b1n_ircGotMODE;
  this.b1n_ircGotTOPIC  = b1n_ircGotTOPIC;
  this.b1n_ircGotBOTEVENT = b1n_ircGotBOTEVENT;
  this.b1n_ircGotCTCP = b1n_ircGotCTCP;

    /* Replies */
  this.b1n_ircCTCPReply = b1n_ircCTCPReply;
  this.b1n_ircReplyPing = b1n_ircReplyPing;
  this.b1n_ircReplyCTCPFinger = b1n_ircReplyCTCPFinger;
  this.b1n_ircReplyCTCPTime = b1n_ircReplyCTCPTime;
  this.b1n_ircReplyCTCPUserinfo = b1n_ircReplyCTCPUserinfo;
  this.b1n_ircReplyCTCPVersion = b1n_ircReplyCTCPVersion;
  this.b1n_ircReplyCTCPClientinfo = b1n_ircReplyCTCPClientinfo;
  this.b1n_ircReplyCTCPPing = b1n_ircReplyCTCPPing;
  this.b1n_ircReplyCTCPAction = b1n_ircReplyCTCPAction;
  this.b1n_ircReplyCTCPError = b1n_ircReplyCTCPError;

  /* Returning Irc Object */
  return this;
}

int
b1n_ircConnect(b1n_irc* this)
{
  if(this->sock.b1n_sockConnect(&this->sock, this->server, this->port)){
    return b1n_TRUE;
  }
  return b1n_FALSE;
}

int
b1n_ircIdentify(b1n_irc* this, const char* auser, const char* aname, const char* anick, const char* apass)
{
  char user[BUFSIZ];
  char nick[BUFSIZ];
  char pass[BUFSIZ];

  /* Setting data */
  snprintf(user, sizeof(user), "USER %s 8 * :%s\r\n", auser, aname);
  snprintf(nick, sizeof(nick), "NICK %s\r\n", anick);
  snprintf(pass, sizeof(pass), "NICKSERV IDENTIFY %s\r\n", apass);

  /* Writing to socket */
  this->sock.b1n_sockWrite(&this->sock, user);
  this->sock.b1n_sockWrite(&this->sock, nick);
  this->sock.b1n_sockWrite(&this->sock, pass);

  return b1n_TRUE;
}

void
b1n_ircChanJoin(b1n_irc* this, const char* chan)
{
  char msg[BUFSIZ];
  snprintf(msg, sizeof(msg), "JOIN #%s\r\n", chan);
  this->sock.b1n_sockWrite(&this->sock, msg);
}

void
b1n_ircChanPart(b1n_irc* this, const char* chan)
{
  char msg[BUFSIZ];
  snprintf(msg, sizeof(msg), "PART #%s", chan);
  this->sock.b1n_sockWrite(&this->sock, msg);
}

void
b1n_ircMsgChan(b1n_irc* this, const char* chan, const char* msg)
{
  char buf[BUFSIZ];
  snprintf(buf, sizeof(buf), "PRIVMSG %s :%s\r\n", chan, msg);
  this->sock.b1n_sockWrite(&this->sock, buf);
}

void
b1n_ircMsgPerson(b1n_irc* this, const char* nick, const char* msg)
{
  char buf[BUFSIZ];
  snprintf(buf, sizeof(buf), "PRIVMSG %s :%s\r\n", nick, msg);
  this->sock.b1n_sockWrite(&this->sock, buf);
}

void
b1n_ircCheckCmds(b1n_irc* this)
{
  char  buf[BUFSIZ];
  char* aux;
  int   ret;

  /* Checking for commands */
  while((ret = this->sock.b1n_sockRead(&this->sock, buf)) > 0){
    for(aux = strtok(buf, "\n"); aux != NULL; aux = strtok(NULL, "\n")){
      if(aux[0] == ':'){
        /* IRC Protocol Parser */
        this->b1n_ircCheckCmd(this, buf);
      }
      else {
        /* PING Reply */
        aux = strtok(aux, " ");
        if(strcmp(aux, "PING") == 0){
          aux+=6; /* 6 = PING(4) + ' '(1) + :(1) = 4+1+1 = 6, now aux = server */
          aux[strlen(aux)] = '\0'; /* stripping \n out */
          this->b1n_ircReplyPing(this, aux);
        }
      }
    }
  }
}

void
b1n_ircCheckCmd(b1n_irc* this, const char* buf)
{
  /* IRC Line */
  char*   pattern;
  size_t  nmatch = 7;
  regmatch_t  m[nmatch];
  b1n_ircline il;

  pattern = "^:([^!]+)!([^@]+)@([^[:space:]]+)[[:space:]]([^[:space:]]+)[[:space:]]([^[:space:]]+)[[:space:]]:?([^\r]+)";
  if(b1n_regMatch(buf, pattern, nmatch, m) &&
    b1n_substr(buf, il.nick, sizeof(il.nick), m[1].rm_so, m[1].rm_eo) &&
    b1n_substr(buf, il.user, sizeof(il.user), m[2].rm_so, m[2].rm_eo) &&
    b1n_substr(buf, il.host, sizeof(il.host), m[3].rm_so, m[3].rm_eo) &&
    b1n_substr(buf, il.cmd,  sizeof(il.cmd),  m[4].rm_so, m[4].rm_eo) &&
    b1n_substr(buf, il.cmdp, sizeof(il.cmdp), m[5].rm_so, m[5].rm_eo) &&
    b1n_substr(buf, il.msg,  sizeof(il.msg),  m[6].rm_so, m[6].rm_eo))
  {
    if(strcmp(il.cmd, "PRIVMSG") == 0){
      this->b1n_ircGotPRIVMSG(this, il);
    }
    else if(strcmp(il.cmd, "NICK") == 0){
      this->b1n_ircGotNICK(this, il);
    }
    else if(strcmp(il.cmd, "JOIN") == 0){
      this->b1n_ircGotJOIN(this, il);
    }
    else if(strcmp(il.cmd, "PART") == 0 || strcmp(il.cmd, "QUIT")){
      this->b1n_ircGotQUIT(this, il);
    }
    else if(strcmp(il.cmd, "MODE") == 0){
      this->b1n_ircGotMODE(this, il);
    }
    else if(strcmp(il.cmd, "TOPIC") == 0){
      this->b1n_ircGotTOPIC(this, il);
    }
  }
}

void
b1n_ircDisconnect(b1n_irc* this)
{
  char msg[BUFSIZ];

  /* Setting data */
  snprintf(msg, BUFSIZ, "QUIT :%s\r\n", b1n_QUIT_MESSAGE);

  /* Disconnecting From Server */
  this->sock.b1n_sockWrite(&this->sock, msg);

  /* Closing Socket */
  this->sock.b1n_sockClose(&this->sock);

  exit(b1n_OK);
}

void
b1n_ircReplyPing(b1n_irc* this, const char* server)
{
  char msg[BUFSIZ];

  /* Setting data */
  snprintf(msg, BUFSIZ, "PONG %s\r\n", server);

  /* Writing to socket */
  this->sock.b1n_sockWrite(&this->sock, msg);
}

void
b1n_ircGotPRIVMSG(b1n_irc* this, const b1n_ircline il)
{
  size_t  nmatch = 3;
  regmatch_t  m[nmatch];

  /* CTCP */
  char  ctcp_cmd[32], ctcp_par[32];
  char* ctcp_pattern;

  /* Bot Cmds */
  char  bot_cmd[32], bot_par[32];
  char* bot_pattern;

  /* Preparing Patterns */
  ctcp_pattern = "^\001([A-Z]+)[[:space:]]?(.*)\001$";
  bot_pattern  = "^[[:space:]]*![[:space:]]*([[:alpha:]]+)[[:space:]]*(.*)$";

  /* Checking if its a CTCP Message */
  if(b1n_regMatch(il.msg, ctcp_pattern, nmatch, m) &&
    b1n_substr(il.msg, ctcp_cmd, sizeof(ctcp_cmd), m[1].rm_so, m[1].rm_eo) &&
    b1n_substr(il.msg, ctcp_par, sizeof(ctcp_par), m[2].rm_so, m[2].rm_eo))
  {
    this->b1n_ircGotCTCP(this, il, ctcp_cmd, ctcp_par);
  }
  /* Testing if its an attempt to trigger a bot event */
  else if(b1n_regMatch(il.msg, bot_pattern, nmatch, m) &&
    b1n_substr(il.msg, bot_cmd, sizeof(bot_cmd), m[1].rm_so, m[1].rm_eo) &&
    b1n_substr(il.msg, bot_par, sizeof(bot_par), m[2].rm_so, m[2].rm_eo))
  {
    this->b1n_ircGotBOTEVENT(this, il, bot_cmd, bot_par);
  }
}

/* Treat BOT Events/Commands */
void
b1n_ircGotBOTEVENT(b1n_irc* this, const b1n_ircline il, const char* bot_cmd, const char* bot_par)
{
  char msg[BUFSIZ];

  if(strcmp(il.nick, b1n_PROGADMIN) == 0){
    if(strcmp(bot_cmd, "s") == 0){
      snprintf(msg, sizeof(msg), "%s: Wassup?", bot_par);
      this->b1n_ircMsgChan(this, il.cmdp, msg);
    }
    else if(strcmp(bot_cmd, "google") == 0){
    }
    else {
    }
  }
}

/* Treat CTCP */
void
b1n_ircGotCTCP(b1n_irc* this, b1n_ircline il, const char* ctcp_cmd, const char* ctcp_par)
{
  char* nick = il.nick;

  if(strcmp(ctcp_cmd, "FINGER") == 0){
    this->b1n_ircReplyCTCPFinger(this, nick);
  }
  else if(strcmp(ctcp_cmd, "TIME") == 0){
    this->b1n_ircReplyCTCPTime(this, nick);
  }
  else if(strcmp(ctcp_cmd, "USERINFO") == 0){
    this->b1n_ircReplyCTCPUserinfo(this, nick);
  }
  else if(strcmp(ctcp_cmd, "VERSION") == 0){
    this->b1n_ircReplyCTCPVersion(this, nick);
  }
  else if(strcmp(ctcp_cmd, "CLIENTINFO") == 0){
    this->b1n_ircReplyCTCPClientinfo(this, nick, ctcp_par);
  }
  else if(strcmp(ctcp_cmd, "PING") == 0){
    this->b1n_ircReplyCTCPPing(this, nick, ctcp_par);
  }
  else if(strcmp(ctcp_cmd, "ACTION") == 0){
    this->b1n_ircReplyCTCPAction(this, nick, ctcp_par);
  }
  else {
    this->b1n_ircReplyCTCPError(this, nick, ctcp_par);
  }
}

void
b1n_ircGotNICK(b1n_irc* this, const b1n_ircline il)
{
}

void
b1n_ircGotJOIN(b1n_irc* this, const b1n_ircline il)
{
}

void
b1n_ircGotPART(b1n_irc* this, const b1n_ircline il)
{
}

void
b1n_ircGotQUIT(b1n_irc* this, const b1n_ircline il)
{
}

void
b1n_ircGotMODE(b1n_irc* this, const b1n_ircline il)
{
}

void
b1n_ircGotTOPIC(b1n_irc* this, const b1n_ircline il)
{
}

/* CTCP Replies */
  /* CTCP Reply Abstraction */
void
b1n_ircCTCPReply(b1n_irc* this, const char* nick, const char* reply)
{
  char msg[BUFSIZ];

  snprintf(msg, sizeof(msg),
    "NOTICE %s :\001%s\001\r\n", nick, reply);

  this->sock.b1n_sockWrite(&this->sock, msg);
}

  /* Reply Finger */
void
b1n_ircReplyCTCPFinger(b1n_irc* this, const char* nick)
{
  char msg[BUFSIZ];

  snprintf(msg, sizeof(msg),
    "FINGER Not implemented.");

  this->b1n_ircCTCPReply(this, nick, msg);
}

  /* Reply Time */
void
b1n_ircReplyCTCPTime(b1n_irc* this, const char* nick)
{
  char msg[BUFSIZ];
  char buf[BUFSIZ];
  time_t t;

  time(&t);
  strftime(buf,  BUFSIZ, "%c", localtime(&t));

  snprintf(msg, sizeof(msg), "TIME %s.", buf);

  this->b1n_ircCTCPReply(this, nick, msg);
}

  /* Reply UserInfo */
void
b1n_ircReplyCTCPUserinfo(b1n_irc* this, const char* nick)
{
  char msg[BUFSIZ];

  snprintf(msg, sizeof(msg),
    "USERINFO %s", b1n_PROGDESC);

  this->b1n_ircCTCPReply(this, nick, msg);
}

  /* Reply Version */
void
b1n_ircReplyCTCPVersion(b1n_irc* this, const char* nick)
{
  char msg[BUFSIZ];

  snprintf(msg, sizeof(msg),
    "VERSION %s %s By %s - %s",
    b1n_PROGNAME,   b1n_PROGVERSION,
    b1n_PROGAUTHOR, b1n_PROGENV);

  this->b1n_ircCTCPReply(this, nick, msg);
}

  /* Reply ClientInfo */
void
b1n_ircReplyCTCPClientinfo(b1n_irc* this, const char* nick, const char* par)
{
  char msg[BUFSIZ];
  int len = strlen(par);
  int i;
  char upar[len]; /* upper case par */

  if(strlen(par) == 0){
    snprintf(msg, sizeof(msg),
      "CLIENTINFO You can request help of the commands "
      "CLIENTINFO ERRMSG FINGER PING USERINFO VERSION by givinng "
      "an argument to CLIENTINFO.");
  }
  else {
    for(i=0; i<=len; i++){
      upar[i] = toupper(par[i]);
    }
    if(strcmp(upar, "CLIENTINFO") == 0){
      snprintf(msg, sizeof(msg),
        "CLIENTINFO CLIENTINFO with 0 arguments gives a list of known "
        "client keywords. With 1 argument, a description of the client "
        "query keyword is returned.");
    }
    else if(strcmp(upar, "ERRMSG") == 0){
      snprintf(msg, sizeof(msg),
        "CLIENTINFO ERRMSG returns error messages.");
    }
    else if(strcmp(upar, "FINGER") == 0){
      snprintf(msg, sizeof(msg),
        "CLIENTINFO FINGER shows real name, login name and idle time of user");
    }
    else if(strcmp(upar, "PING") == 0){
      snprintf(msg, sizeof(msg),
        "CLIENTINFO PING returns the argument it receives.");
    }
    else if(strcmp(upar, "USERINFO") == 0){
      snprintf(msg, sizeof(msg),
        "CLIENTINFO VERSION returns user settable information.");
    }
    else if(strcmp(upar, "VERSION") == 0){
      snprintf(msg, sizeof(msg),
        "CLIENTINFO VERSION shows client type, version and environemnt.");
    }
    else {
      snprintf(msg, sizeof(msg),
        "ERRMSG CLIENTINFO: %s is not a valid command.", upar);
    }
  }
  this->b1n_ircCTCPReply(this, nick, msg);
}

  /* Reply Ping */
void
b1n_ircReplyCTCPPing(b1n_irc* this, const char* nick, const char* par)
{
  char msg[BUFSIZ];

  snprintf(msg, sizeof(msg), "PING %s", par); 

  this->b1n_ircCTCPReply(this, nick, msg);
}

  /* Reply Action */
void
b1n_ircReplyCTCPAction(b1n_irc* this, const char* nick, const char* par)
{
}

  /* Error (CTCP Command not implemented/does not exist) */
void
b1n_ircReplyCTCPError(b1n_irc* this, const char* nick, const char* par)
{
  char msg[BUFSIZ];

  snprintf(msg, sizeof(msg), 
    "ERRMSG %s is not a valid command. "
    "Please, see CLIENTINFO for valid CTCP commands.", par);

  this->b1n_ircCTCPReply(this, nick, msg);
}
