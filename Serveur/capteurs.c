#include <stdio.h>

int traitement_udp(unsigned char *message, int size) {

    char id, x, y, z, temp;
    
    sscanf((char *)message, "%c%c%c%c%c", &id, &x, &y, &z, &temp);
    printf("ID : %c\nx : %c\ny : %c\nz : %c\nTemp : %c\n",id,x,y,z,temp);
    printf("----------------\n");
    
    return 0;

}

