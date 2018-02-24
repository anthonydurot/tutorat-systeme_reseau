#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "serial.h"
#include "analog.h"
#include "broadcast.h"

#define MAC_SIZE	 6
#define IPV4_SIZE	 4

#define IP_SOURCE    "172.26.79.205"
#define IP_DEST      "172.26.79.204"

#define PORT_SOURCE  4269
#define PORT_DEST    80

#define ID_TSHIRT    3

#define END			 0xC0	
#define ESC			 0xDB
#define ESC_END		 0xDC	
#define ESC_ESC      0xDD

uint16_t swap_uint16(uint16_t val) 
{
    return (val << 8) | (val >> 8 );
}

void forger_trameUDP(TrameUDP* trame, uint8_t* v_capteurs) {

	DataUDP data;
	
	trame->port_source = swap_uint16((uint16_t)PORT_SOURCE);
	trame->port_destination = swap_uint16((uint16_t)PORT_DEST);
	trame->longueur = swap_uint16((uint16_t)sizeof(TrameUDP));
	trame->checksum = swap_uint16(0x0000);
	
	data.id_tshirt = (uint8_t)ID_TSHIRT;
	data.accel_x = v_capteurs[0];
	data.accel_y = v_capteurs[1];
	data.accel_z = v_capteurs[2];
	data.temp = v_capteurs[3];
	
	trame->data = data;

}

void forger_trameIP(TrameIP* trame, uint8_t* v_capteurs) {

	trame->version_longueur_entete = 0x45;
	trame->TOS = 0x00;
	trame->longueur_totale = swap_uint16((uint16_t)sizeof(TrameIP));
	trame->identificateur = swap_uint16(0x0000);
	trame->flags_offset = swap_uint16(0x4000);
	trame->TTL = 0x40;
	trame->protocole = 0x11;
	trame->checksum = swap_uint16(0x0000);
	
	char* ips = strdup(IP_SOURCE); 
	char* ipd = strdup(IP_DEST);
	char *token1, *token2;
	uint8_t i = 0;

	while((token1 = strtok_r(ips, ".", &ips)) && (token2 = strtok_r(ipd, ".", &ipd)))
	{
		trame->adr_source[i] = (uint8_t)atoi(token1);
		trame->adr_destination[i] = (uint8_t)atoi(token2);
		i++;
	}
	
	TrameUDP trameU;
	forger_trameUDP(&trameU, v_capteurs);
	trame->data = trameU;	

}

void envoyer_trame(TrameIP* trame) {
	
	uint8_t* buffer = (uint8_t*)trame;
	uint8_t i;
	send_serial(END);
	for(i = 0; i < sizeof(TrameIP); buffer++,i++) {
		if(*buffer == END) {
			send_serial(ESC);
			send_serial(ESC_END);
		}
		else if(*buffer == ESC) {
			send_serial(ESC);
			send_serial(ESC_ESC);			
		}
		else {
			send_serial(*buffer);
		}
	}
	send_serial(END);
}


int main(void) {

    init_printf();
    init_serial(9600);
    uint8_t v_capteurs[4];
    TrameIP trame;
    v_capteurs[0] = 26;
    v_capteurs[1] = 13;
    v_capteurs[2] = 76;
    v_capteurs[3] = 75;
    forger_trameIP(&trame, v_capteurs);
    envoyer_trame(&trame);

/*
   
    while(1) {
    
        for(int i = 0; i <= 3; i++) {
            ad_init(i);
            v_capteurs[i] = ad_sample();
        }
        
        forger_trame(&trame, v_capteurs);
        envoyer_trame(&trame);
    }
    
*/
    
    return 0;
}
