#include <stdio.h>
#include <pthread.h>
#include <libthrd.h>
#include "capteurs.h"
#include <stdlib.h>
#include <string.h>

pthread_mutex_t sem;

int traitement_udp(unsigned char *message, int size) {

    param_udp_t *arguments = malloc(sizeof(param_udp_t));
    arguments->message = (unsigned char *)strdup((const char *)message);
    arguments->size = size;
    if(lanceThread(traitement_message, (void *)arguments, sizeof(arguments))){
        perror("traitement_udp");
        return 1;
    }
    
    return 0;
}

void traitement_message(void *arg) {

    param_udp_t *arguments = (param_udp_t *)arg;
    char id, x, y, z, temp;
    char nom_fichier1[32], nom_fichier2[32];
    FILE *fp1, *fp2;
    sscanf((char *)arguments->message, "%c%c%c%c%c", &id, &x, &y, &z, &temp);
    printf("ID : %d\nx : %d\ny : %d\nz : %d\nTemp : %d\n", id, x, y, z, temp);
    printf("----------------\n");

    //Fichiers sous la forme : ID_temp et ID_accel

    P(sem);
    fp1 = fopen("capteurs.txt","a");
    //fp2 = fopen("capteurs.txt","a");
    //TODO : Ajouter la date dans le fichier ?
    fprintf(fp1, "%d,%d,%d,%d,%d\n", id, x, y, z, temp);
    fclose(fp1);
    V(sem);
    free(arguments->message);
    //free(arguments); Cause un segfault
    
}
