/** fichier http.c **/

/*****************************************/
/** Ce fichier contient des fonctions  **/
/**  concernant les requetes HTTP      **/
/****************************************/

/**** Fichiers d'inclusion ****/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <stddef.h>
#include <errno.h>
#include <stdint.h>
#include <dirent.h>
#include <libcom.h>
#include "http.h"

/**** Constantes ****/

/**** Variables globales *****/

/**** Fonctions de gestion des requetes HTTP ****/


// Requetes HTTP

/**
 * \fn char *analyser_format(char *format)
 * \brief Fonction qui renvoie le format normalisé HTTP en fonction de l'extention passée en paramètre.
 *
 * \param format Extention de fichier à traiter.
 * 
 * \return Chaine de caractère allouée correspondant au format normalisé HTTP.
 */
char *analyser_format(char *format) {

	/*
	/ On pourrait également analyser le fichier ciblé s'il existe pour savoir si c'est du texte ou du binaire. Une facon simple est de répéré la présence de \n et la non présence de \n
	/ Fichier de config avec les MIME pris en charge ?
	*/

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

/**
 * \fn char *date_actuelle(void)
 * \brief Fonction qui renvoie la date actuelle.
 *
 * 
 * \return Date actuelle.
 */
char *date_actuelle(void) {

	time_t timestamp;
	char buffer[256];

	time(&timestamp);
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %X GMT", localtime(&timestamp));

	return strdup(buffer);

}

/**
 * \fn int traiter_requete(FILE *socket, http_info_t *req)
 * \brief Fonction de traitement d'une requête HTTP.
 *
 * \param socket Socket TCP ouverte en tant que fichier contenant la requête HTTP.
 * \param req Structure dans laquelle les informations de la requête seront stockées.
 * 
 * \return 0 si aucune érreur.
 */
int traiter_requete(FILE *socket, http_info_t *req) {

    char buffer[MAX_BUFFER];
    char methode[16];
    char cible[128];
    char version[16];
    char chemin[128];
    char chemin_lock[128];
    char donnees[512];
    char *temp = NULL;
    char *format = NULL;
    int content_length = 0;
    int code;
    struct stat fstat;

    if(fgets(buffer, MAX_BUFFER, socket) == NULL) return 1;
    if(sscanf(buffer, "%s %s %s", methode, cible, version) != 3) return 1;
    printf("%s", buffer);
    if(strcmp(cible, "/") == 0) sprintf(cible, "/%s", DEFAULT_PAGE);

    // On vérifie la présence du fichier

    sprintf(chemin, "%s%s", WEB_DIR, cible);

    if(stat(chemin, &fstat) != 0) {
        code = FOUND; // redirection erreur 404
        sprintf(chemin,"/404.html");
        req->type = IS_FILE;
    }

    else {

        if(S_ISREG(fstat.st_mode)) {
            req->type = IS_FILE;
            code = OK;
        }

        else if(S_ISDIR(fstat.st_mode)) { // Si ce n'est pas un fichier c'est peut être un dossier
            req->type = IS_DIR;
            code = OK;
            strcpy(chemin_lock, chemin);
            strcat(chemin_lock, ".lock");
            if(cible[strlen(cible)-1] != '/') {
                strcat(cible, "/");
                strcpy(chemin, cible);
                code = FOUND;
            }
            else if(!stat(chemin_lock, &fstat)) {
                strcpy(chemin, cible);
                code = FORBIDDEN;
            }
        }

        else { // Sinon, redirection erreur 404
            code = FOUND;
            sprintf(chemin,"/404.html");
            req->type = IS_FILE;
        }
    }

    // Analyse du type de contenu

    if(((temp = strrchr(cible, '.')) != NULL) && (req->type != IS_DIR)) {
    /* Recupère le format de fichier sans le point */
        format = strdup(temp+1);
    }

    else if (req->type == IS_DIR) {
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
    req->donnees = NULL;
    free(format);

    while(fgets(buffer, MAX_BUFFER, socket) != NULL) {
        if(strcmp(buffer, "\r\n") == 0) break;
        if((temp = strstr(buffer, "Content-Length:")) != NULL) {
            content_length = atoi(temp+16);
        }
        printf("%s", buffer);
        fflush(stdout);
    }

    printf("######### Taille du contenu : %d\n", content_length);

    if(content_length > 0) {
        for(int i = 0; i < content_length; i++) {
            donnees[i] = fgetc(socket);;
        }
    donnees[content_length] = '\0';
    req->donnees = strdup(donnees);
    }

    return 0;

}
/**
 * \fn int reponse_header(FILE *socket, http_info_t *req)
 * \brief Fonction qui envoie le header de la réponse au client.
 *
 * \param socket Socket ouverte en fichier de la connexion TCP avec le client.
 * \param req Structure comportant les informations de la requête du client.
 * 
 * \return 0 si aucune érreur.
 */
int reponse_header(FILE *socket, http_info_t *req) {

    fprintf(socket, "HTTP/1.1 %d\r\n", req->code);
    fprintf(socket, "Server: %s\r\n", req->serveur);
    fprintf(socket, "Date: %s\r\n", req->date);
    fprintf(socket, "Content-type: %s\r\n", req->contenu_type);
    fprintf(socket, "Content-length: %d\r\n", req->contenu_taille);
    fprintf(socket, "\r\n");
    fflush(socket);

    return 0;

}

/**
 * \fn int ecriture_reponse(FILE *socket, http_info_t *req)
 * \brief Fonction de réponse envoyant bit à bit le fichier demandé par le client.
 *
 * \param socket Socket ouverte en fichier de la connexion TCP avec le client.
 * \param req Structure comportant les informations de la requête du client.
 * 
 * \return 0 si aucune érreur.
 */
int ecriture_reponse(FILE *socket, http_info_t *req) {

    int toSend = open(req->cible, O_RDONLY);
    int bytes;
    char buffer[MAX_BUFFER];
    int s = fileno(socket);
    struct stat fstat;
    stat(req->cible, &fstat);
    req->contenu_taille = fstat.st_size;

    reponse_header(socket, req);

    while((bytes = read(toSend, buffer, MAX_BUFFER)) > 0) {
        write(s, buffer, bytes);
    }

    close(toSend);

    return 0;

}


/**
 * \fn void free_http_info(http_info_t *r)
 * \brief Fonction qui libère l'espace mémoire alloué lors du traitement de la requête du client.
 *
 * \param r Structure requête à désalouer.
 * 
 * \return 0 si aucune érreur.
 */
void free_http_info(http_info_t *r) {

    free(r->methode);
    free(r->cible);
    free(r->version);
    free(r->serveur);
    free(r->contenu_type);
    free(r->date);
    if(r->donnees) free(r->donnees);

}


/**
 * \fn envoyer_localisation(FILE *socket, http_info_t *req)
 * \brief Fonction qui envoie la localisation au client.
 *
 * \param socket Socket ouverte en fichier de la connexion TCP avec le client.
 * \param req Structure comportant les informations de la requête du client.
 * 
 * \return 0 si aucune érreur.
 */
int envoyer_localisation(FILE *socket, http_info_t *req) {

    fprintf(socket, "HTTP/1.1 %d\r\n", req->code);
    fprintf(socket, "Server: %s\r\n", req->serveur);
    fprintf(socket, "Content-type: %s\r\n", req->contenu_type);
    fprintf(socket, "Location: %s\r\n", req->cible);
    fprintf(socket, "\r\n");
    fflush(socket);

    return 0;

}

/**
 * \fn envoyer_interdit(FILE *socket, http_info_t *req)
 * \brief Fonction qui envoie le message d'erreur 403 Forbidden au client.
 *
 * \param socket Socket ouverte en fichier de la connexion TCP avec le client.
 * \param req Structure comportant les informations de la requête du client.
 * 
 * \return 0 si aucune erreur.
 */
int envoyer_interdit(FILE *socket, http_info_t *req) {

    FILE *toSend = tmpfile();
    int toSend_fd = fileno(toSend);
    int s = fileno(socket);
    struct stat filestat;
    char *ip_serveur = ip_machine();
    char buffer[MAX_BUFFER];
    int bytes;

    fprintf(toSend,

        "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n\
        <html><head>\n\
        <title>403 Forbidden</title>\n\
        </head><body>\n\
        <h1>Forbidden</h1>\n\
        <p>You don't have permission to access %s\n\
        on this server.</p>\n\
        <hr>\n\
        <address>%s at %s Port %d</address>\n\
        </body></html>\n", req->cible, req->serveur, ip_serveur, http_port);

    free(ip_serveur);
    fflush(toSend);
    fseek(toSend, 0, SEEK_SET);
    fstat(toSend_fd, &filestat);
    req->contenu_taille = filestat.st_size;

    reponse_header(socket, req);

    while((bytes = read(toSend_fd, buffer, MAX_BUFFER)) > 0) {
        write(s, buffer, bytes);
    }

    fclose(toSend);

    return 0;

}

// Fonctions pour les répertoires

int in_the_list(char *format, const char *list[]) {

	char c = *list[0];
	int i = 0;

	while(c != 0) {
		if(!strcmp(format, list[i])) { return 1;}
		c = *list[++i];
	}

	return 0;

}

void icon_format(char *buf, char *icon, int type) {

	char *format;
	const char *image[] = {"jpg", "png", "gif", "bmp", "jpeg", "psd", "tif", "tiff", "ico", "\0"};
	const char *movie[] = {"avi", "mp4", "mkv", "wmv", "mov", "flv", "mpg", "\0"};
	const char *sound[] = {"mp3", "flac", "wav", "ogg", "alac", "aiff", "\0"};
	const char *text[] = {"pdf", "txt", "doc", "docx", "md", "tex", "html", "odt", "\0"};
	const char *compressed[] = {"zip", "rar", "tar", "gz", "7z", "\0"};

	if(type) {
		sprintf(icon,"/icons/folder.gif");
	}
	else {
		if((format = strrchr(buf, '.')) != NULL) {format++;}
		if(format == NULL) {sprintf(icon, "/icons/unknown.gif");}
		else if(in_the_list(format, image)) {sprintf(icon, "/icons/image.gif");}
		else if(in_the_list(format, movie)) {sprintf(icon, "/icons/movie.gif");}
		else if(in_the_list(format, sound)) {sprintf(icon, "/icons/sound.gif");}
		else if(in_the_list(format, text)) {sprintf(icon, "/icons/text.gif");}
		else if(in_the_list(format, compressed)) {sprintf(icon, "/icons/compressed.gif");}
		else {
			sprintf(icon, "/icons/unknown.gif");
		}
	}
}

void readable_fs(double size, char *buf) {

    int i = 0;
    const char *units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    while (size > 1000) {
        size /= 1000;
        i++;
    }
    sprintf(buf, "%.*f %s", i, size, units[i]);

}

int html_dir(FILE *socket, http_info_t *req) {

    struct dirent *lecture;
    struct stat attrib;
    char parent[256];
    char courant[256];
    char file_path[256];
    char path[256];
    char f_name[256];
    char date[32];
    char size[32];
    char icon[32];
    char buffer[MAX_BUFFER];
    char *tab[16];
    char *temp;
    FILE *tmp = tmpfile();
    DIR *rep;
    int cpt = 0;
    int s = fileno(socket);
    int tmp_fd = fileno(tmp);
    int bytes;

    sprintf(path, "%s", req->cible);
    rep = opendir(req->cible);
    temp = strtok(path, "/");
    
    while(temp != NULL) {
        tab[cpt++] = temp;
        temp = strtok(NULL, "/");
    }
    
    *parent = 0;
    *courant = 0;

    for(int i = 2; i < cpt-1; i++) {
        strcat(parent, "/");
        strcat(parent, tab[i]);
    }

    strcat(parent, "/");
    strcat(courant, parent);
    strcat(courant, tab[cpt-1]);
    strcat(courant, "/");

    fprintf(tmp, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\n<html>\n\t<head>\n");
    fprintf(tmp, "\t\t<title>Index of %s</title>\n", courant);
    fprintf(tmp, "\t</head>\n\t<body>\n\t\t<h1>Index of %s</h1>\n\t\t<table>\n\t\t\t<tr>\n",courant);
    fprintf(tmp,
    "\t\t\t\t<th valign=\"top\"><img src=\"/icons/blank.gif\"></th>\n\
    \t\t\t<th>Name</th>\n\
    \t\t\t<th>Last modified</th>\n\
    \t\t\t<th>Size</th>\n\
    \t\t\t<th>Description</th>\n\
    \t\t</tr>\n");
    fprintf(tmp, "\t\t\t<tr><th colspan=\"5\"><hr></th></tr>\n");
    fprintf(tmp,
    "\t\t\t<tr>\n\
    \t\t\t<td valign=\"top\"><img src=\"/icons/back.gif\"></td>\n\
    \t\t\t<td><a href=\"%s\">Parent Directory</a></td>\n\
    \t\t\t<td>&nbsp;</td><td align=\"right\">  - </td>\n\
    \t\t\t<td>&nbsp;</td>\n\
    \t\t</tr>\n", parent);

    while ((lecture = readdir(rep))) {

        if((strcmp(lecture->d_name, ".") != 0) && (strcmp(lecture->d_name, "..") != 0)) {

            sprintf(file_path, "%s%s", req->cible, lecture->d_name);
            stat(file_path, &attrib);

            if(!S_ISDIR(attrib.st_mode)) {
                readable_fs(attrib.st_size, size);
                icon_format(lecture->d_name, icon, 0);
                strcpy(f_name, lecture->d_name);
            }
            else {
                strcpy(size, "-");
                icon_format(lecture->d_name, icon, 1);
                strcpy(f_name, lecture->d_name);
                strcat(f_name, "/");
            }

            strftime(date, 32, "%Y-%m-%d %H:%M", localtime(&(attrib.st_ctime)));

            fprintf(tmp,
            "\t\t\t<tr>\n\
            \t<td valign=\"top\"><img src=\"%s\"></td>\n\
            \t<td><a href=\"%s\">%s</a></td>\n\
            \t<td align=\"right\">%s</td>\n\
            \t<td align=\"right\">%s</td>\n\
            \t<td>&nbsp;</td>\n\t\t\t</tr>\n", icon, f_name, lecture->d_name, date, size);

        }
	}

	fprintf(tmp,
	"\t\t\t<tr><th colspan=\"5\"><hr></th></tr>\n\
	\t</table>\n\
	</body>\n</html>\n");

    fflush(tmp);
    fseek(tmp, 0, SEEK_SET);

    fstat(tmp_fd, &attrib);
    req->contenu_taille = attrib.st_size;

    reponse_header(socket, req);

    while((bytes = read(tmp_fd, buffer, MAX_BUFFER)) > 0) {
        write(s, buffer, bytes);
    }

    fclose(tmp);
    closedir(rep);

	return 0;

}
