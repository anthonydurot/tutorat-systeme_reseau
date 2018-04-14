/** Fichier serveur.c **/

/***********************************************************/
/** Serveur pour le serveur                               **/
/***********************************************************/

/** Fichiers d'inclusion **/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <libcom.h>
#include <libthrd.h>
#include <signal.h>
#include "http.h"
#include "capteurs.h"
#include "serveur.h"

/** Constantes **/

/** Variables publiques **/

int nombre_thread_tcp;
int nombre_thread_udp;
int http_port;
struct sigaction action;
list_ID_t list_ID;

void hand(int sig) {

    if(sig == SIGINT) {
        DEBUG_PRINT(("SIGINT\n"));
        while(nombre_thread_tcp != 0) {}
        DEBUG_PRINT(("%d threads tcp\n", nombre_thread_tcp));
        while(nombre_thread_udp != 1) {}
        DEBUG_PRINT(("%d threads udp\n", nombre_thread_udp));
        //TODO Trouver un moyen de kill le serveurMessages, garder son tid ?
        DEBUG_PRINT(("Threads terminés\n"));
        exit(SIGINT);
    }
}

/** Variables statiques **/

/** Fonctions propres au serveur **/

void gestionClient(void *s) {

    int socket = *((int *)s);
    /* Obtient une structure de fichier */
    FILE *dialogue = fdopen(socket, "a+");
    if(dialogue == NULL){ perror("gestionClient.fdopen"); exit(EXIT_FAILURE); }
    http_info_t req;

    if(traiter_requete(dialogue, &req)) {
        errno = ENOENT;
        perror("gestionClient.traiter_requete");
        fclose(dialogue);
        P(MUTEX_THREAD);
        nombre_thread_tcp--;
        V(MUTEX_THREAD);
        return;
    }

    DEBUG_PRINT(("######### Code : %d\n", req.code));
    if(req.donnees) DEBUG_PRINT(("######### Donnees : %s\n", req.donnees));

    if(req.code == FORBIDDEN) {
        envoyer_interdit(dialogue, &req);
    }

    else if(req.code == FOUND) {
        envoyer_localisation(dialogue, &req);
    }

    else {

        if(req.type == IS_DIR) {
            html_dir(dialogue, &req);
        }

        else {
            if(ecriture_reponse(dialogue, &req)) {
                errno = ENOENT;
                perror("gestionClient.ecriture_reponse");
            }
        }
    }

    fclose(dialogue);
    free_http_info(&req);
    P(MUTEX_THREAD);
    nombre_thread_tcp--;
    V(MUTEX_THREAD);

    return;

}

int traiter_options(int argc, char **argv) {

    int ch, port_n;
	int option_presente = 0;

    for(int i = 0; i < argc; i++) {
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

void nouveauClient(int dialogue) {

    if(lanceThread(gestionClient, (void *)&dialogue, sizeof(int))) {
        perror("nouveauClient.lanceThread");
        exit(-1);
    }
    P(MUTEX_THREAD);
    nombre_thread_tcp++;
    V(MUTEX_THREAD);

}

void _serveurMessages(void *arg) {

    (void)arg;
    serveurMessages("4000", traitement_udp);
    P(MUTEX_THREAD);
    nombre_thread_tcp--;
    V(MUTEX_THREAD);

}

void maj_list() {

    FILE *fp = fopen("www/data/list_ID", "a+");
    char buffer[32];
    int id;
    while(fgets(buffer, 32, fp) != NULL) {
        sscanf(buffer, "%d", &id);
        list_ID.list[++list_ID.last] = id;
    }
    fclose(fp);

}

/** Procedure principale **/

int main(int argc, char **argv) {

    int s;
    action.sa_handler = hand;
    sigaction(SIGINT, &action, NULL);
    nombre_thread_tcp = 0;
    nombre_thread_udp = 0;
    list_ID.last = -1;
    maj_list();
    /* Lecture des arguments de la commande */
    http_port = traiter_options(argc, argv);
    char port_s[6];
    sprintf(port_s, "%d", http_port);

    /* Initialisation du serveur */
    if((s = initialisationServeur(port_s)) < 0 ) {
        fprintf(stderr, "Initialisation du serveur impossible, êtes vous root ?\n");
        exit(-1);
    }

    /* Lancement du serveur de messages UDP */
    if(lanceThread(_serveurMessages, NULL, 0)) {
        perror("nouveauClient.lanceThread");
        exit(-1);
    }
    P(MUTEX_THREAD);
    nombre_thread_udp++;
    V(MUTEX_THREAD);

    /* Lancement de la boucle d'ecoute */
    if(boucleServeur(s, nouveauClient) <= 0) {
        fprintf(stderr, "Connexion avec le client impossible\n");
        exit(-1);
    }

}
