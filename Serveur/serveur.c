/** Fichier serveur.c **/

/***********************************************************/
/** Serveur pour le serveur                               **/
/***********************************************************/

/** Fichiers d'inclusion **/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include "../Communication/libcom.h"

/** Constantes **/

/** Variables publiques **/

int http_port;

/** Variables statiques **/

/** Fonctions propres au serveur **/

int traiter_options(int argc, char **argv) {

    int ch, port_n;
	uint8_t option_presente = 0;

	for(uint8_t i = 0; i < argc; i++) {
		if(*argv[i] == '-') { option_presente = 1; }
	}

	if(!option_presente) {
        fprintf(stderr,"Usage du programme : %s [-p port] [--port port]\n", argv[0]);
        exit(-1);
	}

    static struct option long_options[] =
    {
        {"port", required_argument, NULL, 'p'},
        {NULL, 0, NULL, 0}
    };

    while ((ch = getopt_long(argc, argv, "p:", long_options, NULL)) != -1)
    {
        switch (ch)
        {
            case 'p':
                port_n = atoi(optarg);
                if(port_n <= 0 || port_n > 65535) {
                    fprintf(stderr,"Le port %d n'est pas valide\n", port_n);
                    exit(-1);
                }
                break;
            case ':':
                fprintf(stderr, "%s: le parametre '-%c' requiert un argument\n", argv[0], optopt);
                exit(-1);
            case '?':
            	fprintf(stderr, "Usage du programme: %s [-p port] [--port port]\n", argv[0]);
            	exit(-1);
            default:
                fprintf(stderr, "%s: l'option `-%c' est inconnue\n", argv[0], optopt);
                exit(-1);
        }
    }

    return port_n;

}

/** Procedure principale **/

int main(int argc,char **argv) {

    int s;
    /* Lecture des arguments de la commande */
    http_port = traiter_options(argc,argv);
    char port_s[6];
    sprintf(port_s, "%d", http_port);

    /* Initialisation du serveur */
    if((s = initialisationServeur(port_s)) < 0 ) {
        fprintf(stderr,"Initialisation du serveur impossible, êtes vous root ?\n");
        exit(-1);
    }

    /* Lancement de la boucle d'ecoute */
    if(boucleServeur(s,TCP_connexion) <= 0) {
        fprintf(stderr, "Connexion avec le client impossible\n");
        exit(-1);
    }

}
