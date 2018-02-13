/** fichier xbee.c **/

/*****************************************************************/
/** Utilitaire pour configurer les modules XBee.                **/
/*****************************************************************/

/** Fichiers d'inclusion **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "xbeeATCmd.h"

/** Constantes **/

#define SERIALDEV	"/dev/ttyUSB0"
#define BAUDRATE	B9600 

/**** Variables globales *****/

static struct termios sauvegarde;

/** Ouverture d'un port serie **/

int ouvertureSerie(char *periph,int vitesse)
{
struct termios nouveau;
int df=open(periph,O_RDWR|O_NOCTTY);
if(df<0) return -1;

tcgetattr(df,&sauvegarde); /* save current port settings */
bzero(&nouveau,sizeof(nouveau));
nouveau.c_cflag=CLOCAL|CREAD|vitesse|CS8;
nouveau.c_iflag=0;
nouveau.c_oflag=0;
nouveau.c_lflag=0;
nouveau.c_cc[VTIME]=0;
nouveau.c_cc[VMIN]=1;
tcflush(df, TCIFLUSH);
tcsetattr(df,TCSANOW,&nouveau);

return df;
}

/** Fermeture d'un port serie **/

void fermetureSerie(int df)
{
tcsetattr(df,TCSANOW,&sauvegarde);
close(df);
}

/** Programme principal **/

int main(int argc, char *argv[])
{
int ds;
ds=ouvertureSerie(SERIALDEV,BAUDRATE);
if(ds<0){
  fprintf(stderr,"Erreur sur la connexion série.\n");
  exit(-1);
  }

fprintf(stdout,"Configuration actuelle :\n");
fprintf(stdout,"----------------------\n");
xbeeModeCommande(ds);
xbeeRecupereVitesse(ds);
xbeeRecupereCanal(ds);

fprintf(stdout,"\nConfiguration par défaut :\n");
fprintf(stdout,"------------------------\n");
xbeeDefaut(ds);
xbeeRecupereVitesse(ds);
xbeeRecupereCanal(ds);

fprintf(stdout,"\nConfiguration spécifique :\n");
fprintf(stdout,"------------------------\n");
xbeeConfigureVitesse(ds,XBEE_VITESSE_9600);
xbeeConfigureCanal(ds,0x18);
xbeeRecupereVitesse(ds);
xbeeRecupereCanal(ds);
xbeeSauver(ds);
xbeeSortir(ds);

fermetureSerie(ds);
return EXIT_SUCCESS;
}
