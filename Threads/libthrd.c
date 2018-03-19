/**
 * \file libthrd.c
 * \brief Bibliothèque pour threads
 * \author Antoine.D
 * \author Anthony.D
 * \version 0.1
 * \date 13 mars 2018
 *
 * Bibliothèque offrant les fonctions permettant la gestion de threads
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include "libthrd.h"
#include "../Serveur/http.h"

/**
 * \fn static int lanceThread(void (*fonction)(void*), void* arg, int taille)
 * \brief Fonction de lancement de thread
 *
 * \param fonction Pointeur de fonction de la fonction wrapper
 * \param arg Pointeur générique représentant la donnée à passer au thread
 * \param taille Taille de la valeur pointée par arg
 * \return 0 si aucune erreur, -1 sinon.
 */

int lanceThread(void (*fonction)(void*), void* arg, int taille) {

	thread_param_t* arguments = (thread_param_t*)malloc(sizeof(thread_param_t));

	if(arguments == NULL){
        perror("lanceThread.malloc");
        return -1;
    }

    arguments->arg = arg;
    arguments->taille = taille;
	fonction((void*)arguments);

	return 0;

}

/**
 * \fn lanceThreadWEB(void* arg)
 * \brief Fonction wrapper du thread pour la gestion d'un client
 *
 * \param arg Pointeur sur thread_param_t coercé génériquement
 * \return void
 */

void lanceThreadWEB(void* arg) {

    pthread_t tid;
    pthread_attr_t attr;

    if(pthread_attr_init(&attr) != 0){
        perror("lanceThread.pthread_attr_init");
    }

    if(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0){
        perror("lanceThread.pthread_attr_setdetachstate");
    }

    if(pthread_create(&tid, &attr, gestionClient, arg) != 0){
       perror("lanceThread.pthread_create");
    }

}

void* gestionClient(void* s) {

    int socket = *((int *)(((thread_param_t *)s)->arg));
    /* Obtient une structure de fichier */
    FILE *dialogue = fdopen(socket, "a+");
    if(dialogue == NULL){ perror("gestionClient.fdopen"); exit(EXIT_FAILURE); }
    http_info_t req;

    if(traiter_requete(dialogue, &req)) {
        errno = ENOENT;
        perror("gestionClient.traiter_requete");
        fclose(dialogue);
        free(((thread_param_t *)s)->arg);
        free(s);
        return NULL;
    }

    printf("######### Code : %d\n", req.code);
    if(req.donnees) printf("######### Donnees : %s\n", req.donnees);

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

    printf("------------------------------------------------------------\n");
    fclose(dialogue);
    free(((thread_param_t *)s)->arg);
    free(s);
    free_http_info(&req);

    return NULL;

}
