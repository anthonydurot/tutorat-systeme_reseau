#include <stdio.h>
#include <libthrd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "capteurs.h"
#include "serveur.h"

int traitement_udp(unsigned char *message, int size) {

    param_udp_t *arguments = malloc(sizeof(param_udp_t));
    arguments->message = (unsigned char *)strdup((const char *)message);
    arguments->size = size;
    if(lanceThread(traitement_message, (void *)arguments, sizeof(arguments))) {
        perror("traitement_udp");
        return 1;
    }
    P(MUTEX_THREAD);
    nombre_thread_udp++;
    V(MUTEX_THREAD);

    return 0;

}

int ajouter_id_list(int id) {

    int i;
    if(list_ID.last == -1) {
        list_ID.list[0] = id;
        list_ID.last++;
    }
    else {
        for(i = 0; i <= list_ID.last; i++) {
            if(list_ID.list[i] == id) {return 0;}
        }
        list_ID.list[++list_ID.last] = id;
    }

    return 1;

}

void traitement_message(void *arg) {

    param_udp_t *arguments = (param_udp_t *)arg;
    char id, x, y, z, temp;
    char nom_fichier1[32], nom_fichier2[32];
    FILE *fp1, *fp2, *fp3;
    sscanf((char *)arguments->message, "%c%c%c%c%c", &id, &x, &y, &z, &temp);
    DEBUG_PRINT(("ID : %d\nx : %d\ny : %d\nz : %d\nTemp : %d\n", (int)id, (int)x, (int)y, (int)z, (int)temp));
    DEBUG_PRINT(("----------------\n"));

    //Fichiers sous la forme : ID_temp et ID_accel

    sprintf(nom_fichier1, "www/data/TID_%d", (int)id);
    sprintf(nom_fichier2, "www/data/AID_%d", (int)id);
    P(MUTEX_FICHIER);
    fp1 = fopen(nom_fichier1, "a");
    fp2 = fopen(nom_fichier2, "a");
    if(ajouter_id_list(id)) {
        fp3 = fopen("www/data/list_ID", "a");
        fprintf(fp3, "%d\n", (int)id);
        fclose(fp3);
    }
    //TODO : Ajouter la date dans le fichier ?
    fprintf(fp1, "%d\n", (int)temp);
    fprintf(fp2, "%d,%d,%d\n", (int)x, (int)y, (int)z);
    fclose(fp1);
    fclose(fp2);
    V(MUTEX_FICHIER);
    free(arguments->message);
    P(MUTEX_THREAD);
    nombre_thread_udp--;
    V(MUTEX_THREAD);

}
