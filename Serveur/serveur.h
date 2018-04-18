/** fichier serveur.h **/

/*********************************************/
/* Structures necessaires pour le serveur    */
/*********************************************/

/** Constantes **/

#define MAX_ID     128
#define UDP_ECOUTE "4000"
#define IP_ARDUINO "192.168.0.27"
#define PORT_ARDUINO "5678"

#ifdef DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

/** Structures **/

typedef struct list_ID {
    int list[MAX_ID];
    int last;
} list_ID_t;

/** Variables publiques  **/

extern int nombre_thread_tcp;
extern int nombre_thread_udp;
extern list_ID_t list_ID;

/** Prototypes **/

/* Fonction permettant de récupérer la valeur de l'unique option -p / --port */

int traiter_options(int argc, char **argv);
void gestionClient(void *s);
void nouveauClient(int dialogue);
void _serveurMessages(void *arg);
void maj_list(void);
void hand(int sig);
