#define MON_ID 3
#define SEUIL 1
#define IP_ARDUINO "10.189.28.15"//"192.168.0.27" //"172.26.79.205"
#define PORT "4000"

typedef struct param_udp {
    unsigned char *message;
    int size;
} param_udp_t;

int traitement_udp(unsigned char *message, int size);
void traitement_message(void * arg);
int ajouter_id_list(int id);
void compare(unsigned char x, unsigned char y, unsigned char z);
