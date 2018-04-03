/** fichier serveur.h **/

/*********************************************/
/* Structures necessaires pour le serveur    */
/*********************************************/

/** Constantes **/

/** Structures **/

/** Variables publiques  **/

/** Prototypes **/

/* Fonction permettant de récupérer la valeur de l'unique option -p / --port */

int traiter_options(int argc, char **argv);
void gestionClient(void *s);
void nouveauClient(int dialogue);
void _serveurMessages(void *arg);
