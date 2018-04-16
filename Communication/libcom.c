/**
 * \file libcom.c
 * \brief Bibliothèque des fonctions réseaux.
 * \author Antoine D
 * \author Anthony D
 * \version 0.1
 * \date 26 Mars 2018
 *
 * Bibliothèque regroupant l'ensemble des fonctions nécessaires pour la communication au sein du projet.
 *
 */


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

/**
 * \fn char *ip_machine(void)
 * \brief Fonction qui retourne l'IP sur laquelle le serveur est lancée.
 *
 * \return char*
 */
char *ip_machine(void) { //TODO : A modifier, déplacer ?

    char hostname[256];
    char *ip_adr;

    if (!gethostname(hostname, sizeof(hostname))) {

        struct hostent *host = gethostbyname(hostname);
        if (host != NULL) {
            struct in_addr **adr;
            for (adr = (struct in_addr **)host->h_addr_list; *adr; adr++) {
                ip_adr = inet_ntoa(**adr);
            }
        }

        return strdup(ip_adr);

    }

    return NULL;

}
/**
 * \fn int initialisationServeur(char *service)
 * \brief Fonction d'initialisation d'un serveur TCP.
 *
 * \param service Port sur lequel doit écouter le serveur TCP.
 * 
 * \return Descripteur de fichier de la socket si aucune erreur, -1 sinon.
 */

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

/**
 * \fn boucleServeur(int ecoute, void (*traitement)(int))
 * \brief Fonction boucle du serveur gérant les connexions entrantes des clients.
 *
 * \param ecoute Socket d'écoute correspondant à la socket TCP bind.
 * \param traitement Fonction de traitement de la socket de connexion du client.
 * 
 * \return 0 si aucune érreur, -1 sinon.
 */

int boucleServeur(int ecoute, void (*traitement)(int)) {

    int dialogue;

    while(1) {
        /*Attente d'une connexion*/
        if((dialogue = accept(ecoute, NULL, NULL)) < 0) return -1;
        /*Passage de la socket de dialogue a la fonction de traitement*/
        traitement(dialogue);
    }
    return 0;
    
}

/**
 * \fn serveurMessages(char *service)
 * \brief Fonction créant un serveur UDP.
 *
 * \param service Port sur lequel doit écouter le serveur UDP.
 * \param traitement_udp Fonction qui va traiter les requêtes UDP entrantes.
 * 
 */
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
        if(traitement_udp((unsigned char*)message, nboctets)) {
            perror("serveurMessages.traitement_udp");
            exit(-1);            
        }
    }
}

/**
 * \fn envoiMessage(char *service, unsigned char *message, int taille)
 * \brief Fonction d'envoi d'une chaine de caractères en broadcast UDP.
 *
 * \param service Port sur lequel envoyer le message UDP.
 * \param message Chaine de caractères à envoyer.
 * \param taille Taille du message à envoyer en octets.
 * 
 * \return 0 si aucune erreur, -1 sinon.
 */

int envoiMessage(char *service, unsigned char *message, int taille) {

    int broadcast = 1;
    struct sockaddr_in adresseBroadcast;
    socklen_t tailleBroadcast = sizeof(adresseBroadcast);

    setsockopt(socket_udp,SOL_SOCKET,SO_BROADCAST, &broadcast, sizeof(broadcast));
    adresseBroadcast.sin_family = AF_INET;
    adresseBroadcast.sin_port = htons(atoi(service));
    adresseBroadcast.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    sendto(socket_udp, message, taille, 0, (struct sockaddr *)&adresseBroadcast, tailleBroadcast);

    return 0;

}


/**
 * \fn envoiMessageUnicast(char *service, char *machine, unsigned char *message, int taille)
 * \brief Fonction d'envoi d'une chaine de caractères en unicast UDP.
 *
 * \param service Port sur lequel envoyer le message UDP.
 * \param machine Nom d'hôte sur laquelle envoyer le message UDP.
 * \param message Chaine de caractères à envoyer.
 * \param taille Taille du message à envoyer en octets.
 * 
 * \return 0 si aucune erreur, -1 sinon.
 */

int envoiMessageUnicast(char *service, char *machine, unsigned char *message, int taille) {

    int unicast = 0;
    struct addrinfo precisions, *origine;

    memset(&precisions, 0, sizeof(precisions));
    precisions.ai_family = AF_INET;
    precisions.ai_socktype = SOCK_DGRAM;
    setsockopt(socket_udp, SOL_SOCKET, SO_BROADCAST, &unicast, sizeof(unicast));
    if(getaddrinfo(machine, service, &precisions, &origine) != 0) {
        if(origine != NULL) {
            sendto(socket_udp, message, taille, 0, origine->ai_addr, origine->ai_addrlen);
            freeaddrinfo(origine);
            return 0;
        }
    }

    return 1;

}
