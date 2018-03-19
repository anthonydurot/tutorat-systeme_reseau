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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "libcom.h"
#include "../Threads/libthrd.h"

/**** Constantes ****/

/**** Variables globales *****/

int socket_udp;

/**** Prototype des fonctions locales *****/

/**** Fonctions de gestion des sockets ****/


int initialisationServeur(char *service) {

    struct addrinfo precisions, *resultat, *origine;
    int statut;
    int s;

    /*Construction de la structure adresse*/
    memset(&precisions, 0, sizeof precisions);
    precisions.ai_family = AF_UNSPEC;
    precisions.ai_socktype = SOCK_STREAM;
    precisions.ai_flags = AI_PASSIVE;
    statut = getaddrinfo(NULL, service, &precisions, &origine);
    if(statut < 0){ perror("initialisationServeur.getaddrinfo"); exit(EXIT_FAILURE); }
    struct addrinfo *p;
    for(p = origine,resultat = origine;p != NULL;p = p->ai_next)
        if(p->ai_family == AF_INET6){ resultat = p; break; }

    /*Creation d'une socket*/
    s = socket(resultat->ai_family, resultat->ai_socktype, resultat->ai_protocol);
    if(s < 0){ perror("initialisationServeur.socket"); exit(EXIT_FAILURE); }

    /*Options utiles*/
    int vrai = 1;
    if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &vrai, sizeof(vrai)) < 0) {
        perror("initialisationServeur.setsockopt (REUSEADDR)");
        exit(EXIT_FAILURE);
      }
    if(setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &vrai, sizeof(vrai)) < 0) {
        perror("initialisationServeur.setsockopt (NODELAY)");
        exit(EXIT_FAILURE);
      }

    /*Specification de l'adresse de la socket*/
    statut = bind(s, resultat->ai_addr, resultat->ai_addrlen);
    if(statut < 0) return -1;

    /*Liberation de la structure d'informations*/
    freeaddrinfo(origine);

    /*Taille de la queue d'attentei*/
    statut = listen(s, MAX_TCP_CONNEXION);
    if(statut < 0) return -1;

    return s;

}

int boucleServeur(int ecoute, int (*traitement)(int)) {

    int dialogue;

    while(1) {
        /*Attente d'une connexion*/
        if((dialogue = accept(ecoute, NULL, NULL)) < 0) return -1;
        /*Passage de la socket de dialogue a la fonction de traitement*/
        if(traitement(dialogue) < 0){ shutdown(ecoute, SHUT_RDWR); return 0;}
    }

}

int TCP_connexion(int socket) {

    int *arg = (int *)malloc(sizeof(int));
    *arg = socket;

    return lanceThread(lanceThreadWEB, (void *)arg, sizeof(int));

}


void serveurMessages(char *service, int (*traitement_udp)(unsigned char *, int)) {

    struct sockaddr_in adresseServeur;
    socklen_t tailleServeur = sizeof(adresseServeur);
    socket_udp = socket(AF_INET, SOCK_DGRAM, 0);

    if(socket_udp < 0) {
        perror("serveurMessages.socket");
        exit(-1);
    }

    memset((char*)&adresseServeur, 0, sizeof(adresseServeur));
    adresseServeur.sin_family = AF_INET; //IPV4
    adresseServeur.sin_addr.s_addr = htonl(INADDR_ANY); //0.0.0.0
    adresseServeur.sin_port = htons(atoi(service)); //Car port > 255, envoie en short

    if(bind(socket_udp, (struct sockaddr *)&adresseServeur, tailleServeur) == -1) {
        perror("serveurMessages.bind");
        exit(-1);
    }

    while(1) {
        struct sockaddr_in adresseClient;
        socklen_t tailleClient = sizeof(adresseClient);
        int nboctets;
        char message[MAX_UDP_MESSAGE];
        nboctets = recvfrom(socket_udp, message, MAX_TAMPON-1, 0, (struct sockaddr*)&adresseClient, &tailleClient);
        message[nboctets] = '\0';
        printf("Données recues : %s\n", message);
        envoiMessage("5000","coucou", sizeof("coucou"));
        envoiMessageUnicast("80", "plil.fr", "coucou-unicast", sizeof("coucou-unicast"));
        //if(traitement_udp(message, nboctets) < 0) {
        /* TODO : Fonction traitement du message */
        //    break;
        //}
    }
}

int envoiMessage(char *service, unsigned char *message, int taille) {

    int broadcast = 1;
    struct sockaddr_in adresseBroadcast;
    socklen_t tailleBroadcast = sizeof(adresseBroadcast);

    setsockopt(socket_udp,SOL_SOCKET,SO_BROADCAST, &broadcast, sizeof(broadcast));
    adresseBroadcast.sin_family = AF_INET;
    adresseBroadcast.sin_port = htons(atoi(service));
    adresseBroadcast.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    sendto(socket_udp, message, strlen(message), 0, (struct sockaddr *)&adresseBroadcast, tailleBroadcast);

    return 0;

}

int envoiMessageUnicast(char *service, char *machine, unsigned char *message, int taille) {

    int unicast = 0;
    struct sockaddr_in adresseClient;
    struct addrinfo precisions, *resultat, *origine;

    memset(&precisions, 0, sizeof(precisions));
    precisions.ai_family = AF_INET;
    precisions.ai_socktype = SOCK_DGRAM;
    setsockopt(socket_udp, SOL_SOCKET, SO_BROADCAST, &unicast, sizeof(unicast));
    if(getaddrinfo(machine, service, &precisions, &origine) != 0) {
        if(origine != NULL) {
            sendto(socket_udp, message, strlen(message), 0, origine->ai_addr, origine->ai_addrlen);
            freeaddrinfo(origine);
            return 0;
        }
    }

    return 1;

}
