/** fichier libcom.c **/

/*****************************************/
/** Ce fichier contient des fonctions  **/
/**  concernant les sockets.           **/
/****************************************/

/**** Fichiers d'inclusion ****/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include "../Threads/libthrd.h"

//#include "libcom.h"

/**** Constantes ****/

#define MAX_TAMPON	1024
#define MAX_CONNEXIONS 10

/**** Variables globales *****/

/**** Prototype des fonctions locales *****/

/**** Fonctions de gestion des sockets ****/


char* traiter_options(int argc, char** argv) {

    int ch, port_n;
    char* port_s = (char*)malloc(6*sizeof(char));

    static struct option long_options[] =
    {
        {"port", required_argument, NULL, 'p'},
        {NULL, 0, NULL, 0}
    };

    if(argc == 1) {
        fprintf(stderr,"Usage du programme : %s [-p port] [--port port]\n",argv[0]);
        exit(-1);
    }

    while ((ch = getopt_long(argc, argv, ":p:", long_options, NULL)) != -1)
    {
        switch (ch)
        {
            case 'p':
                port_n = atoi(optarg);
                if(port_n <= 0 || port_n > 65535) {
                    fprintf(stderr,"Le port %d n'est pas valide\n",port_n);
                    exit(-1);
                }
                break;
            case ':':
                fprintf(stderr, "%s : le parametre '-%c' requiert un argument\n",argv[0], optopt);
                exit(-1);
            default:
                fprintf(stderr, "%s : l'option `-%c' est inconnue\n",argv[0], optopt);
                exit(-1);
        }
    }

    sprintf(port_s, "%d", port_n);
    return port_s;

}

int initialisationServeur(char *service) {

    struct addrinfo precisions,*resultat,*origine;
    int statut;
    int s;

    /* Construction de la structure adresse */
    memset(&precisions,0,sizeof precisions);
    precisions.ai_family = AF_UNSPEC;
    precisions.ai_socktype = SOCK_STREAM;
    precisions.ai_flags = AI_PASSIVE;
    statut = getaddrinfo(NULL,service,&precisions,&origine);

    if(statut<0){ perror("initialisationServeur.getaddrinfo"); exit(EXIT_FAILURE); }
    struct addrinfo *p;
    for(p = origine,resultat = origine;p != NULL;p = p->ai_next)
        if(p->ai_family == AF_INET6){ resultat = p; break; }

    /* Creation d'une socket */
    s = socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);

    if(s < 0){ perror("initialisationServeur.socket"); exit(EXIT_FAILURE); }

    /* Options utiles */
    int vrai = 1;
    if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&vrai,sizeof(vrai)) < 0){
        perror("initialisationServeur.setsockopt (REUSEADDR)");
        exit(EXIT_FAILURE);
      }
    if(setsockopt(s,IPPROTO_TCP,TCP_NODELAY,&vrai,sizeof(vrai)) < 0){
        perror("initialisationServeur.setsockopt (NODELAY)");
        exit(EXIT_FAILURE);
      }

    /* Specification de l'adresse de la socket */
    statut = bind(s,resultat->ai_addr,resultat->ai_addrlen);

    if(statut < 0) return -1;

    /* Liberation de la structure d'informations */
    freeaddrinfo(origine);

    /* Taille de la queue d'attente */
    statut = listen(s,MAX_CONNEXIONS);
    if(statut < 0) return -1;

    return s;

}


int boucleServeur(int ecoute,int (*traitement)(int)) { //Apelle un wrapper de LanceThread pour lancer la fonction de gestion clients

    int dialogue;

    while(1) {

        /* Attente d'une connexion */
        if((dialogue = accept(ecoute,NULL,NULL)) < 0) return -1;

        /* Passage de la socket de dialogue a la fonction de traitement */
        if(traitement(dialogue) < 0){ shutdown(ecoute,SHUT_RDWR); return 0;}

    }
}

void* gestionClient(void* s) { //Fonction effective de gestion des clients qui va être threaded
    int socket = *((int*)(((thread_param_t*)s)->arg));
    /* Obtient une structure de fichier */
    FILE *dialogue = fdopen(socket,"a+");
    if(dialogue == NULL){ perror("gestionClient.fdopen"); exit(EXIT_FAILURE); }

    /* Echo */
    char ligne[MAX_TAMPON];
    while(fgets(ligne,MAX_TAMPON,dialogue) != NULL)
    {
        printf(">> %s",ligne);
        fprintf(dialogue,"Bien recu !\n");
        //fprintf(dialogue,"> %s",ligne);
    }
    /* Termine la connexion */
    fclose(dialogue);
    free(((thread_param_t*)s)->arg);
    free(s);
    return 0;
}

int Tcp_connexion(int socket){
    int* arg = (int*)malloc(sizeof(int));
    *arg = socket;
    return lanceThread(gestionClient, (void*)arg, sizeof(int));
}



int main(int argc, char *argv[]) {

    int s;
    /* Lecture des arguments de la commande */
    char* port = traiter_options(argc,argv);
    /* Initialisation du serveur */
    if((s = initialisationServeur(port)) < 0 ) {
        fprintf(stderr,"Initialisation du serveur impossible, êtes vous root ?\n");
        exit(-1);
    }

    /* Lancement de la boucle d'ecoute */
    if(boucleServeur(s,Tcp_connexion) <= 0) {
        fprintf(stderr,"Connexion avec le client impossible\n");
        exit(-1);
    }
    free(port);
}
