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
#include <string.h>
#include "http.h"
#include "capteurs.h"
#include "serveur.h"

/** Constantes **/

/** Variables globales **/

int http_port;
struct sigaction action;

/** Variables publiques **/

int nombre_thread_tcp;
int nombre_thread_udp;
list_ID_t list_ID;

/**
 * \fn void hand(int sig)
 * \brief Fonction permettant de traiter les signaux SIGINT
 *
 * \param sig Type du signal.
 *
 * \return void
 */
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

/** Fonctions propres au serveur **/

/**
 * \fn void gestionClient(void *s)
 * \brief Fonction passé en paramètre de lanceThread() pour traiter les paquets TCP entrant
 *
 * \param s Pointeur générique transportant une socket.
 *
 * \return void
 */
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
    if(req.donnees) {
        if(!strcmp(req.donnees,"getLast")) {
            DEBUG_PRINT(("######### getLast\n"));
        }
        else {
            char *ids = strrchr(req.donnees,'=');
            ids++;
            unsigned char id = atoi(ids);
            unsigned char idChange[2] = {0x02,0x00};
            idChange[1] = id;
            DEBUG_PRINT(("######### Groupe : %d\n", id));
            envoiMessageUnicast(PORT_ARDUINO, IP_ARDUINO, idChange, 2);
        }
    }

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

/**
 * \fn traiter_options(int argc, char **argv)
 * \brief Fonction qui traite les options passées au programme.
 *
 * \param argc Nombre d'arguments (+1) passée au programme.
 * \param argv Liste comprenant le nom du programme et les arguments passés.
 *
 * \return int Le numéro du port sur lequel lancer le serveur TCP.
 */
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

/**
 * \fn nouveauClient(int dialogue)
 * \brief Fonction passée en paramètre de boucleServeur() qui lance un thread pour chaque client.
 *
 * \param dialogue Socket de dialogue du client.
 *
 * \return void
 */
void nouveauClient(int dialogue) {

    if(lanceThread(gestionClient, (void *)&dialogue, sizeof(int))) {
        perror("nouveauClient.lanceThread");
        exit(-1);
    }
    P(MUTEX_THREAD);
    nombre_thread_tcp++;
    V(MUTEX_THREAD);

}

/**
 * \fn _serveurMessages(void *arg)
 * \brief Fonction wrapper passée en paramètre de lanceThread() qui va exécuter le serveur UDP
 *
 * \param arg Pointeur générique ne transportant rien ici.
 *
 * \return void
 */
void _serveurMessages(void *arg) {

    (void)arg;
    serveurMessages(UDP_ECOUTE, traitement_udp);
    P(MUTEX_THREAD);
    nombre_thread_udp--;
    V(MUTEX_THREAD);

}

/**
 * \fn void maj_list(void)
 * \brief Fonction qui met à jour au démarrage du programme la liste globale des IDs contenus dans le fichier de sauvegarde
 *
 * \return void
 */
void maj_list(void) {

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
