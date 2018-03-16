/**
 * \file http.h
 * \brief Bibliothèque pour requetes HTTP
 * \author Antoine.D
 * \author Anthony.D
 * \version 0.1
 * \date 13 mars 2018
 *
 * Ce fichier décrit les structures et les constantes utilisées
 * par les fonctions de gestion des requetes HTTP
 *
 */

#ifndef LIBHTTP_H
#define LIBHTTP_H

/**** Constantes ****/

#define SERVER_NAME     "Choctaw HTTP Server"
#define MAX_BUFFER      1024
#define DEFAULT_PAGE 	"index.html"
#define WEB_DIR         "./www"
#define NOT_FOUND_PAGE  "404.html"
#define IS_FILE         0
#define IS_DIR          1
#define OK              200
#define NOT_FOUND       404

/**** Structures ****/

typedef struct http_info_s {
	char *methode;
	char *cible;
	char *version;
	int type; // Fichier ou dossier
	char *serveur;
	char *contenu_type;
	char *date;
	int code;
} http_info_t;

/***** Prototypes ****/

char *analyser_format(char *format);
char *date_actuelle(void);
int traiter_requete(FILE *socket, http_info_t *req);
void free_http_info(http_info_t *r);

#endif
