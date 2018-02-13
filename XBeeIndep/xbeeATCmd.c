/** fichier xbeeATCmd.c **/

/*****************************************************************/
/** Commandes pour configurer les modules XBee.                 **/
/*****************************************************************/

/** Fichiers d'inclusion **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/** Constantes **/

#define TAILLE_TAMPON	128

/** Fonctions **/

void xbeeReponse(int ds)
{
    register int i;
    char d[TAILLE_TAMPON];
    sync();
    for(i=0;i<TAILLE_TAMPON;i++){
        if(read(ds,d+i,1)!=1){ perror("xbeeReponse.read"); exit(-1); }
        if(d[i]==0x0d) break;
    }
    int size=i;
    for(i=0;i<size;i++)
        if(d[i]!=0x0d) fprintf(stdout,"%c",d[i]);
    if(size>0) fprintf(stdout," (");
    for(i=0;i<size;i++){
        fprintf(stdout,"%.2x",d[i]);
        if(i<size-1) fprintf(stdout," ");
    }
    if(size>0) fprintf(stdout,")\n");
}

void xbeeDefaut(int ds)
{
    #ifdef DEBUG
    printf("{xbeeDefaut}\n");
    #endif
    char* d="ATRE\r";
    write(ds,d,strlen(d));
    xbeeReponse(ds);
}

void xbeeSauver(int ds)
{
    #ifdef DEBUG
    printf("{xbeeSauver}\n");
    #endif
    char* d="ATWR\r";
    write(ds,d,strlen(d));
    xbeeReponse(ds);
}

void xbeeSortir(int ds)
{
    #ifdef DEBUG
    printf("{xbeeSortir}\n");
    #endif
    char *cmd="ATCN\r";
    write(ds,cmd,strlen(cmd));
    xbeeReponse(ds);
}

void xbeeConfigureVitesse(int ds,unsigned char vitesse)
{
    #ifdef DEBUG
    printf("{xbeeConfigureVitesse %d}\n",vitesse);
    #endif
    if(vitesse<0 || vitesse>7) return;
    char cmd[TAILLE_TAMPON];
    sprintf(cmd,"ATBD %x\r",vitesse);
    write(ds,cmd,strlen(cmd));
    xbeeReponse(ds);
}

void xbeeRecupereVitesse(int ds)
{
    #ifdef DEBUG
    printf("{xbeeRecupereVitesse}\n");
    #endif
    char *cmd="ATBD\r";
    write(ds,cmd,strlen(cmd));
    xbeeReponse(ds);
}

/* Parametre canal entre 0x0B et 0x1A */
void xbeeConfigureCanal(int ds,char canal)
{
    #ifdef DEBUG
    printf("{xbeeConfigureCanal %02x} : debut\n",canal);
    #endif
    char cmd[TAILLE_TAMPON];
    sprintf(cmd,"ATCH %x\r",canal);
    sync();
    write(ds,cmd,strlen(cmd));
    xbeeReponse(ds);
    #ifdef DEBUG
    printf("{xbeeConfigureCanal} : fin\n");
    #endif
}

void xbeeRecupereCanal(int ds)
{
    #ifdef DEBUG
    printf("{xbeeRecupereCanal} : debut\n");
    #endif
    char *cmd="ATCH\r";
    sync();
    write(ds,cmd,strlen(cmd));
    xbeeReponse(ds);
    #ifdef DEBUG
    printf("{xbeeRecupereCanal} : fin\n");
    #endif
}

void xbeeModeCommande(int ds)
{
    #ifdef DEBUG
    printf("{xbeeModeCommande}\n");
    #endif
    char *cmd="+++";
    sleep(1);
    sync();
    write(ds,cmd,strlen(cmd));
    xbeeReponse(ds);
}
