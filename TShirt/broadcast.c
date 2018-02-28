#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "serial.h"
#include "analog.h"
#include "broadcast.h"

#define ID_TSHIRT    3

// Define pour IP
#define VERSION 	 0x04
#define L_ENTETE     0x05
#define TOS			 0x00
#define ID			 0x0000
#define FLAGS        0x02
#define OFFSET       0x0000
#define TTL          0x40
#define PROTOCOLE    0x11	
#define IP_SOURCE    "172.26.79.205"
#define IP_DEST      "172.26.79.204"

// Define pour UDP
#define PORT_SOURCE  4269
#define PORT_DEST    80

// Define pour SLIP
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

	trame->c0 = (uint16_t)TOS;
	uint16_t vl = (uint16_t)L_ENTETE | (uint16_t)VERSION<<4;
	trame->c0 = (trame->c0) | vl<<8;
	trame->c1 = (uint16_t)sizeof(TrameIP);
	trame->c2 = (uint16_t)ID;
	trame->c3 = (uint16_t)OFFSET;
	trame->c3 = (trame->c3) | (uint16_t)FLAGS<<13;
	trame->c4 = (uint16_t)PROTOCOLE | (uint16_t)TTL<<8;
	trame->c5 = 0x0000; // Futur checksum
	
	char* ips = strdup(IP_SOURCE); 
	char* ipd = strdup(IP_DEST);
	char *token1, *token2;
	uint8_t adr_source[4], adr_destination[4], i = 0;

	while((token1 = strtok_r(ips, ".", &ips)) && (token2 = strtok_r(ipd, ".", &ipd)))
	{
		adr_source[i] = (uint8_t)atoi(token1);
		adr_destination[i] = (uint8_t)atoi(token2);
		i++;
	}
	
	trame->c6 = (uint16_t)adr_source[1] | (uint16_t)adr_source[0]<<8;
	trame->c7 = (uint16_t)adr_source[3] | (uint16_t)adr_source[2]<<8;
	trame->c8 = (uint16_t)adr_destination[1] | (uint16_t)adr_destination[0]<<8;
	trame->c9 = (uint16_t)adr_destination[3] | (uint16_t)adr_destination[2]<<8;
	
	free(ips);
	free(ipd);
	
	// Ici on calculera le checksum
	
    calcul_checksum_ip(trame);

	// Little Endian to Big Endian 
	trame->c0 = swap_uint16(trame->c0);
	trame->c1 = swap_uint16(trame->c1);
	trame->c2 = swap_uint16(trame->c2);
	trame->c3 = swap_uint16(trame->c3);
	trame->c4 = swap_uint16(trame->c4);
	trame->c5 = swap_uint16(trame->c5);
	trame->c6 = swap_uint16(trame->c6);
	trame->c7 = swap_uint16(trame->c7);
	trame->c8 = swap_uint16(trame->c8);
	trame->c9 = swap_uint16(trame->c9);
	
	TrameUDP trameU;
	forger_trameUDP(&trameU, v_capteurs);
	trame->data = trameU;

}

void calcul_checksum_ip(TrameIP* trame){
    uint32_t somme = 0;
    uint16_t carry = 0;
    uint16_t* ptr = (uint16_t*)trame;
    uint8_t i = 0;
    for(i=0;i<((sizeof(TrameIP)-sizeof(TrameUDP))/sizeof(uint16_t));i++){
            if(i!=5){
                somme += *ptr;
            }
            ptr++;
    }
    carry = (uint16_t)(somme & 0xffff0000)>>16;
    while(carry != 0){
        somme &= 0x0000ffff;
        somme += carry;
        carry = (uint16_t)(somme & 0xffff000)>>16;
    }
    trame->c5 = ~(uint16_t)somme;
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

    //init_printf();
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
