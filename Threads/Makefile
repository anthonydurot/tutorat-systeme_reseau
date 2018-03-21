#
# Makefile pour generer la bibliotheque des threads
#

all: libthrd.a

#
# La cible de nettoyage
#

clean:
	rm -f core *.o libthrd.a

#
# Les cibles pour la bibliotheque
#

libthrd.o: libthrd.c libthrd.h
	gcc -o libthrd.o -c libthrd.c

libthrd.a: libthrd.o
	rm -f $@
	$(CLIB) $@ $+
