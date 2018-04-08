typedef struct param_udp_t{
    unsigned char *message;
    int size;
} param_udp_t;


int traitement_udp(unsigned char *message, int size);
void traitement_message(void * arg);
