#
# Makefile pour generer le serveur
#

OBJS = serveur.o http.o capteurs.o
LIBS = -lpthread -lm
LOCAL_LIBS  = -L ../Communication -L ../Threads
LOCAL_LIBS += -lcom -lthrd
CFLAGS     += -I ../Communication -I ../Threads
DFLAGS = -g -DDEBUG


SRCS = serveur.c http.c capteurs.c
OBJS = $(SRCS:.c=.o)

#
# Cible generale
#

all: serveur
debug: serveurdebug

#
# La cible de nettoyage
#

clean:
	rm -f core *.o serveur

#
# Les cibles pour l'executable
#

serveur: $(OBJS)
	$(CC) $(CFLAGS) -o serveur $^ $(LOCAL_LIBS) $(LIBS)

%.o: %.c %.h
	 $(CC) -c $(CFLAGS) -o $@ $<
