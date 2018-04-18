#include <stdio.h>
#include <libthrd.h>
#include <libcom.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include <math.h>
#include "capteurs.h"
#include "serveur.h"

/**
 * \fn int traitement_udp(unsigned char *message, int size)
 * \brief Fonction passée en paramètre de serveurMessages() pour traiter les paquets UDP entrant.
 *
 * \param message Donnée UDP.
 * \param size Taille de la donnée UDP.
 *
 * \return 0 si aucune erreur.
 */
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

/**
 * \fn int ajouter_id_list(int id)
 * \brief Fonction qui ajoute un groupe (ID) à liste contigue list_ID globale.
 *
 * \param id ID à ajouter à la liste.
 *
 * \return 0 si aucune erreur, 1 si liste pleine.
 */
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

/**
 * \fn void traitement_message(void *arg)
 * \brief Fonction passée en paramètre de lanceThread() pour traiter la donnée UDP.
 *
 * \param arg Pointeur générique transportant une donnée de type param_udp_t.
 *
 * \return void
 */
void traitement_message(void *arg) {

    param_udp_t *arguments = (param_udp_t *)arg;
    unsigned char id, x, y, z, temp;
    char nom_fichier1[32], nom_fichier2[32];
    FILE *fp1, *fp2, *fp3;
    sscanf((char *)arguments->message, "%c%c%c%c%c", &id, &x, &y, &z, &temp);
    DEBUG_PRINT(("ID : %d\nx : %d\ny : %d\nz : %d\nTemp : %d\n", (uint8_t)id, (uint8_t)x, (uint8_t)y, (uint8_t)z, (uint8_t)temp));
    compare(x,y,z);
    DEBUG_PRINT(("----------------\n"));
    sprintf(nom_fichier1, "www/data/TID_%d", id);
    sprintf(nom_fichier2, "www/data/AID_%d", id);
    P((int)id);
    fp1 = fopen(nom_fichier1, "a");
    fp2 = fopen(nom_fichier2, "a");
    if(ajouter_id_list(id)) {
        fp3 = fopen("www/data/list_ID", "a");
        fprintf(fp3, "%d\n", (uint8_t)id);
        fclose(fp3);
    }
    //TODO : Ajouter la date dans le fichier ?
    fprintf(fp1, "%d\n", (uint8_t)temp);
    fprintf(fp2, "%d,%d,%d\n", (uint8_t)x, (uint8_t)y, (uint8_t)z);
    fclose(fp1);
    fclose(fp2);
    V((int)id);
    free(arguments->message);
    P(MUTEX_THREAD);
    nombre_thread_udp--;
    V(MUTEX_THREAD);

}


void compare(unsigned char x, unsigned char y, unsigned char z){
    if((uint8_t)(sqrt(pow((double)x,(double)2.0)+pow((double)y,(double)2.0)+pow((double)z,(double)2.0))) > SEUIL){
        DEBUG_PRINT(("Chute !\n"));
        envoiMessageUnicast(PORT, IP_ARDUINO, (unsigned char *)MESSAGE_CHUTE, sizeof(MESSAGE_CHUTE));
    }
    return;
}
