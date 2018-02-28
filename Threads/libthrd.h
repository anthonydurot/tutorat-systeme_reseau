#ifndef LIBTHRD_H
#define LIBTHRD_H
/** fichier libthrd.h **/

/******************************************************************/
/** Ce fichier décrut les structures et les constances utilisées **/
/** par les fonctions de gestion des threads                     **/
/******************************************************************/

/**** Structures ****/

typedef struct thread_param{
    void* arg;
    int taille;
}thread_param_t;

/***** Prototypes ****/

int lanceThread(void* (*)(void*), void*, int);
#endif
