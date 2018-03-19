/**
 * \file libthrd.h
 * \brief Bibliothèque pour threads
 * \author Antoine.D
 * \author Anthony.D
 * \version 0.1
 * \date 13 mars 2018
 *
 * Ce fichier décrit les structures et les constantes utilisées
 * par les fonctions de gestion des threads
 *
 */

#ifndef LIBTHRD_H
#define LIBTHRD_H

/**** Structures ****/

/**
 * \struct thread_param_t
 * \brief Arguments des threads
 *
 * thread_param_t permet de passer plusieurs arguments
 * à un thread qui ne prend qu'un void* en paramètre
 *
 */

typedef struct thread_param {

    void *arg; /*!< Pointeur générique */
    int taille; /*!< Taille de la donnée pointée par arg */

} thread_param_t;

/**** Prototypes ****/

int lanceThread(void (*)(void *), void *, int);
void lanceThreadWEB(void *);
void *gestionClient(void *);

#endif
