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

extern int http_port;

/**** Constantes ****/

#define SERVER_NAME     "Choctaw HTTP Server"
#define MAX_BUFFER      1024
#define DEFAULT_PAGE 	"valeurs.html"
#define WEB_DIR         "./www"
#define NOT_FOUND_PAGE  "404.html"
#define IS_FILE         0
#define IS_DIR          1
#define OK              200
#define NOT_FOUND       404
#define FOUND           302
#define FORBIDDEN       403

/**** Structures ****/

typedef struct http_info_s {
    char *methode;
    char *cible;
    char *version;
    int type; // Fichier (0) ou dossier (1)
    char *serveur;
    char *contenu_type;
    char *donnees;
    char *date;
    int contenu_taille;
    int code;
} http_info_t;

/***** Prototypes ****/

char *ip_machine(void);
char *analyser_format(char *format);
char *date_actuelle(void);
int traiter_requete(FILE *socket, http_info_t *req);
int reponse_header(FILE *socket, http_info_t *req);
int ecriture_reponse(FILE *socket, http_info_t *req);
void free_http_info(http_info_t *r);
int envoyer_localisation(FILE *socket, http_info_t *req);
int envoyer_interdit(FILE *socket, http_info_t *req);
int in_the_list(char *format, const char *list[]);
void icon_format(char *buf, char *icon, int type);
void readable_fs(double size, char *buf);
int html_dir(FILE *socket, http_info_t *req);

#endif
