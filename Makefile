# $Id: Makefile,v 1.6 2006/01/12 22:32:39 mmr Exp $

OBJS=cbot.o irc.o regex.o sock.o 
CC=cc
DEFS=
#CFLAGS=-O2 -Wall -DDEBUG
CFLAGS=-O2 -Wall
INC=
LIB=

all: cbot

cbot: $(OBJS)
	$(CC) $(CFLAGS) $(DEFS) $(OBJS) $(INC) $(LIB) -o $@

cbot.o: cbot.c
	$(CC) $(CFLAGS) $(DEFS) $(INC) -c cbot.c

distclean: clean
	rm -f *~

clean:
	rm -f $(OBJS) $(HDRS) cbot cbot.core
