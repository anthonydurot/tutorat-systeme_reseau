/** fichier xbeeATCmd.h **/

/*****************************************************************/
/** Declarations publiques pour configurer les modules XBee.    **/
/*****************************************************************/

/** Constantes **/

#define XBEE_VITESSE_1200	0
#define XBEE_VITESSE_2400	1
#define XBEE_VITESSE_4800	2
#define XBEE_VITESSE_9600	3
#define XBEE_VITESSE_19200	4
#define XBEE_VITESSE_38400	5
#define XBEE_VITESSE_57600	6
#define XBEE_VITESSE_115200	7

/** Prototypes **/

void xbeeReponse(int ds);
void xbeeDefaut(int ds);
void xbeeSauver(int ds);
void xbeeSortir(int ds);
void xbeeConfigureVitesse(int ds,unsigned char vitesse);
void xbeeRecupereVitesse(int ds);
void xbeeConfigureCanal(int ds,char canal);
void xbeeRecupereCanal(int ds);
void xbeeModeCommande(int ds);
