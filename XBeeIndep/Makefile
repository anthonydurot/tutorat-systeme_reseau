#
# Makefile de l'utilitaire XBee
#
CC = gcc
CFLAGS = -Wall
OBJS = xbee.o xbeeATCmd.o

#
# Cible generale
#

all: xbee

#
# La cible de nettoyage
#

clean:
	rm -f core *.o xbee

#
# Les cibles pour l'executable
#

xbee: $(OBJS)
	$(CC) $(CFLAGS) -o xbee $(OBJS)

xbeeATCmd.o:	xbeeATCmd.c xbeeATCmd.h
