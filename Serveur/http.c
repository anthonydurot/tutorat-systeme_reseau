/** fichier http.c **/

/*****************************************/
/** Ce fichier contient des fonctions  **/
/**  concernant les requetes HTTP      **/
/****************************************/

/**** Fichiers d'inclusion ****/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

/**** Constantes ****/

/**** Variables globales *****/

/**** Fonctions de gestion des requetes HTTP ****/

char *analyser_format(char *format) {

	if(!strcmp(format,"none")) return strdup("application/octet-stream"); // Type par défaut
	if(!strcmp(format,"dir")) return strdup("text/html"); // Une page HTML listant les fichiers sera retournée
	if(!strcmp(format,"txt")) return strdup("text/plain");
	if(!strcmp(format,"html")) return strdup("text/html");
	if(!strcmp(format,"css")) return strdup("text/css");
	if(!strcmp(format,"js")) return strdup("application/javascript");
	if(!strcmp(format,"jpg") || !strcmp(format,"jpeg")) return strdup("image/jpeg");
	if(!strcmp(format,"gif")) return strdup("image/gif");
	if(!strcmp(format,"png")) return strdup("image/png");
	if(!strcmp(format,"pdf")) return strdup("application/pdf");

	return strdup("application/octet-stream");

}

char *date_actuelle(void) {

	time_t timestamp;
	char buffer[256];

	time(&timestamp);
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %X GMT", localtime(&timestamp));

	return strdup(buffer);

}

int traiter_requete(FILE *socket, http_info_t *req) {

	char buffer[MAX_BUFFER];
	char methode[16];
	char cible[128];
	char version[16];
	char chemin[128];
	char *temp = NULL;
	char *format = NULL;
	int code;
	struct stat fstat;

	if(fgets(buffer, MAX_BUFFER, socket) == NULL) return 1;
	if(sscanf(buffer, "%s %s %s",methode, cible, version) != 3) return 1;
	if(strcmp(cible, "/") == 0) sprintf(cible, "/%s", DEFAULT_PAGE);

	// On vérifie la présence du fichier

	sprintf(chemin, "%s%s", WEB_DIR, cible);

  	if(stat(chemin,&fstat) != 0) {
        sprintf(chemin, "%s/%s", WEB_DIR, NOT_FOUND_PAGE);
        strcpy(cible, NOT_FOUND_PAGE);
        req->type = IS_FILE;
        code = NOT_FOUND;
    }
    else {
        if(S_ISREG(fstat.st_mode)){
            req->type = IS_FILE;
            code = OK;
        }
  		else if(S_ISDIR(fstat.st_mode)) { // Si ce n'est pas un fichier c'est peut être un dossier
  			req->type = IS_DIR;
  			code = OK;
  		}

        else { // Sinon, erreur 404
			sprintf(chemin, "%s/%s", WEB_DIR, NOT_FOUND_PAGE);
			strcpy(cible, NOT_FOUND_PAGE);
			req->type = IS_FILE;
			code = NOT_FOUND;
        }
    }

	// Analyse du type de contenu

	if(((temp = strrchr(cible,'.')) != NULL) && (req->type != IS_DIR)) {
        /* Recupère le format de fichier sans le point */
		format = strdup(temp+1);
	}

	else if(req->type == IS_DIR) {
        format = strdup("dir");
	}

	else {
        format = strdup("none");
	}

	// Affectation à la structure

	req->code = code;
	req->methode = strdup(methode);
	req->cible = strdup(chemin);
	req->version = strdup(version);
	req->serveur = strdup(SERVER_NAME);
	req->contenu_type = analyser_format(format);
	req->date = date_actuelle();
	free(format);

	while(fgets(buffer, MAX_BUFFER, socket) != NULL) {
		if(strcmp(buffer, "\n") == 0) break;
		printf("%s", buffer);
	}

	return 0;

}

void free_http_info(http_info_t *r) {

	free(r->methode);
	free(r->cible);
	free(r->version);
	free(r->serveur);
	free(r->contenu_type);
	free(r->date);

}
