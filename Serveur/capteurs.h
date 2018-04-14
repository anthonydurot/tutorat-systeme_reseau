#define MON_ID 3

typedef struct param_udp {
    unsigned char *message;
    int size;
} param_udp_t;

int traitement_udp(unsigned char *message, int size);
void traitement_message(void * arg);
int ajouter_id_list(int id);
