#
# Makefile pour generer le serveur
#

OBJS = serveur.o
LIBS = -lpthread
LOCAL_LIBS  = -L ../Communication # -L ../Threads
LOCAL_LIBS += -lcom # -lthrd
CFLAGS     += -I ../Communication # -I ../Threads

#
# Cible generale
#

all: serveur

#
# La cible de nettoyage
#

clean:
	rm -f core *.o serveur

#
# Les cibles pour l'executable
#

serveur: $(OBJS)
	$(CC) $(CFLAGS) -o serveur $(OBJS) $(LOCAL_LIBS) $(LIBS)

serveur.o: serveur.c serveur.h
