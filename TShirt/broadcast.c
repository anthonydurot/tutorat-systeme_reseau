#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "serial.h"
#include "analog.h"
#include "broadcast.h"

#define MAC_SIZE	6
#define IPV4_SIZE	4

#define IP_SOURCE   "172.26.79.205"
#define IP_DEST     "172.26.79.204"

#define PORT_SOURCE 4269
#define PORT_DST    80

int main(void)
{

    init_printf();
    uint8_t v_capteurs[4];
    TrameIP trame;
    init_trame(&trame);
   
    while(1) {
    
        for(int i = 0; i <= 3; i++) {
            ad_init(i);
            v_capteurs[i] = ad_sample();
        }
        
        forger_trame(&trame, v_capteurs);
        envoyer_trame(&trame);
    }
    
    return 0;
}
