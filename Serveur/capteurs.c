#include <stdio.h>
#include <libthrd.h>
#include "capteurs.h"
#include <stdlib.h>
#include <string.h>
#include "serveur.h"

int traitement_udp(unsigned char *message, int size) {

    param_udp_t *arguments = malloc(sizeof(param_udp_t));
    arguments->message = (unsigned char *)strdup((const char *)message);
    arguments->size = size;
    if(lanceThread(traitement_message, (void *)arguments, sizeof(arguments))){
        perror("traitement_udp");
        return 1;
    }
    P(MUTEX_THREAD);
    nombre_thread_udp++;
    V(MUTEX_THREAD);
    return 0;
}

void traitement_message(void *arg) {

    param_udp_t *arguments = (param_udp_t *)arg;
    char id, x, y, z, temp;
    char nom_fichier1[32], nom_fichier2[32];
    FILE *fp1, *fp2;
    sscanf((char *)arguments->message, "%c%c%c%c%c", &id, &x, &y, &z, &temp);
    printf("ID : %d\nx : %d\ny : %d\nz : %d\nTemp : %d\n", (int)id, (int)x, (int)y, (int)z, (int)temp);
    printf("----------------\n");

    //Fichiers sous la forme : ID_temp et ID_accel

    sprintf(nom_fichier1, "data/%d_temp.txt", (int)id);
    sprintf(nom_fichier2, "data/%d_accel.txt", (int)id);
    printf("0");
    P(MUTEX_FICHIER);
    printf("1");
    fp1 = fopen(nom_fichier1,"a");
    fp2 = fopen(nom_fichier2,"a");
    //TODO : Ajouter la date dans le fichier ?
    fprintf(fp1, "%d",temp);
    fprintf(fp2, "%d,%d,%d", x, y, z);
    fclose(fp1);
    fclose(fp2);
    V(MUTEX_FICHIER);
    free(arguments->message);
    P(MUTEX_THREAD);
    nombre_thread_udp--;
    V(MUTEX_THREAD);

}
