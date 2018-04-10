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

#define MUTEX_FICHIER 0
#define MUTEX_THREAD  1

#ifdef DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

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
    void *arg;
    void (*fonction)(void *);
} thread_param_t;

/**** Prototypes ****/

int lanceThread(void (*)(void *), void *, int);

void P(int);

void V(int);

#endif
