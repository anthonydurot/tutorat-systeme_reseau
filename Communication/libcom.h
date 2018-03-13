/** fichier libcom.h **/

/******************************************************************/
/** Ce fichier decrit les structures et les constantes utilisees **/
/** par les fonctions de gestion des sockets                     **/
/******************************************************************/

/**** Constantes ****/

#define MAX_UDP_MESSAGE 500
#define MAX_TCP_CONNEXION 500

/**** Fonctions ****/


/*** TCP ***/


int initialisationServeur(char *);

/* Cette foncton prend en paramètre le port sur lequel il faut écouter et retourne la socket de lecture. Il vous est demandé d'activer l'option de réutilisation d'adresse sur la socket d'écoute (fonction setsockopt) */

int boucleServeur(int, int (*)(int));

/* Cette fonction effectue l'écoute sur la socket passée en premier argument et lors d'une connexion, exécute la fonction passée en second argument. Cette fonction passée en argument doit être une fonction qui prend une socket en unique paramètre. Lors d'une connexion de clien, la fonction boucleServeur lance donc cette fonction avec la socket de dialogue en paramètre. */


/**** UDP ***/


void serveurMessages(char *, int (*)(unsigned char*, int));

/* Cette fonction prend en paramètre le service sur lequel lancer un serveur UDP et la fonction de traitement des messages recus */

int envoiMessage(char*, unsigned char*, int);

/* Il s'agit de la fonction complémentaire, permettant d'envoyer un message UDP en diffusion totale. Le premier argument est le service UDP ciblé, le second est le paquet à envoyer et enfin le dernier la taille du paquet. N'oubliez pas d'activer l'option permettant une diffusion totale (fonctionsetsockopt). Notez que le descripteur de socket est celui créé, en variable globale, par la fonction précédente */

int envoiMessageUnicast(char*, char*, unsigned char*);

/* Cette fonction envoie un message UDP. Le premier argument est le service UDP ciblé, le second est le nom de la machine de destination, le troisième est le paquet à envoyer et enfin le dernier est la taille du paquet */

