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
#include <string.h>
#include "libthrd.h"

//const int sem;

/*** Prototype local ***/
void *_lanceThread(void *arg);


/**
 * \fn int lanceThread(void (*fonction)(void*), void* arg, int taille)
 * \brief Fonction de lancement de thread.
 *
 * \param fonction Pointeur de fonction de la fonction à executer.
 * \param arg Pointeur générique représentant la donnée à passer au thread.
 * \param taille Taille de la valeur pointée par arg.
 * \return 0 si aucune erreur, -1 sinon.
 */
int lanceThread(void (*fonction)(void *), void *arg, int taille) {

    thread_param_t *arguments = (thread_param_t *)malloc(sizeof(thread_param_t));

    if(arguments == NULL){
        perror("lanceThread.malloc");
        return 1;
    }

    arguments->arg = malloc(taille);
    memcpy(arguments->arg, arg, taille);
    arguments->fonction = fonction;

    pthread_t tid;
    pthread_attr_t attr;

    if(pthread_attr_init(&attr) != 0) {
        perror("lanceThread.pthread_attr_init");
        return 1;
    }

    if(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
        perror("lanceThread.pthread_attr_setdetachstate");
        return 1;
    }

    if(pthread_create(&tid, &attr, _lanceThread, arguments) != 0) {
       perror("lanceThread.pthread_create");
       return 1;
    }


    return 0;

}

/* Fonction invisible pour l'utilisateur => Non documentée dans doxygen */
void *_lanceThread(void *arg) {

    thread_param_t *arguments = (thread_param_t *)arg;
    arguments->fonction(arguments->arg);
    free(arguments->arg);
    free(arguments);

    return NULL;

}

/* TODO : P et V doivent prendre un int */
void P(pthread_mutex_t sem){
    pthread_mutex_lock(&sem);
}

void V(pthread_mutex_t sem){
    pthread_mutex_unlock(&sem);
}
