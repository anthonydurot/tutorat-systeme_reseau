#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <avr/interrupt.h>

#include "serial.h"
#include "analog.h"
#include "broadcast.h"

/* Varaibles globales */

uint8_t rx_complete = 1;
uint8_t ID_tshirt = 3;

/* Fonctions */

uint16_t swap_uint16(uint16_t val) {

    return (val << 8) | (val >> 8);

}

void forger_trameUDP(TrameUDP *trame, uint8_t *v_capteurs) {

	DataUDP data;

	trame->port_source = (uint16_t)PORT_SOURCE;
	trame->port_destination = (uint16_t)PORT_DEST;
	trame->longueur = (uint16_t)sizeof(TrameUDP);
	trame->checksum = 0x0000;

	data.TX.id_tshirt = (uint8_t)ID_tshirt;
	data.TX.accel_x = v_capteurs[0];
	data.TX.accel_y = v_capteurs[1];
	data.TX.accel_z = v_capteurs[2];
	data.TX.temp = v_capteurs[3];

	trame->data = data;

}

void forger_trameIP(TrameIP *trame, uint8_t *v_capteurs) {

	trame->c0 = (uint16_t)TOS;
	uint16_t vl = (uint16_t)L_ENTETE | (uint16_t)VERSION<<4;
	trame->c0 = (trame->c0) | vl<<8;
	trame->c1 = (uint16_t)sizeof(TrameIP);
	trame->c2 = (uint16_t)ID;
	trame->c3 = (uint16_t)OFFSET;
	trame->c3 = (trame->c3) | (uint16_t)FLAGS<<13;
	trame->c4 = (uint16_t)PROTOCOLE | (uint16_t)TTL<<8;
	trame->c5 = 0x0000; // Checksum

    char ips[32];
	strcpy(ips, IP_SOURCE);
	char ipd[32];
	strcpy(ipd, IP_DEST);
	int adr_source[4], adr_destination[4];

	sscanf(ips, "%d.%d.%d.%d", &adr_source[0], &adr_source[1], &adr_source[2], &adr_source[3]);
	sscanf(ipd, "%d.%d.%d.%d", &adr_destination[0], &adr_destination[1], &adr_destination[2], &adr_destination[3]);

	trame->c6 = (uint16_t)adr_source[1] | ((uint16_t)adr_source[0])<<8;
	trame->c7 = (uint16_t)adr_source[3] | ((uint16_t)adr_source[2])<<8;
	trame->c8 = (uint16_t)adr_destination[1] | ((uint16_t)adr_destination[0])<<8;
	trame->c9 = (uint16_t)adr_destination[3] | ((uint16_t)adr_destination[2])<<8;

	TrameUDP trameU;
	forger_trameUDP(&trameU, v_capteurs);
	trame->data = trameU;

    // Calcul des checksum
    calcul_checksum_ip(trame);
    calcul_checksum_udp(trame);

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
    (trame->data).port_source = swap_uint16((trame->data).port_source);
    (trame->data).port_destination = swap_uint16((trame->data).port_destination);
    (trame->data).longueur = swap_uint16((trame->data).longueur);
    (trame->data).checksum = swap_uint16((trame->data).checksum);

}

void calcul_checksum_udp(TrameIP *trame) {

    uint32_t somme;
    uint16_t carry;
    uint16_t *ptr = (uint16_t *)trame;
    uint8_t *ptr2;
    uint8_t i;
    uint8_t dataSize = sizeof((trame->data).data);

    somme = (uint32_t)trame->c6 + (uint32_t)trame->c7 + (uint32_t)trame->c8 + (uint32_t)trame->c9 + (uint32_t)PROTOCOLE + (uint32_t)((trame->data).longueur);
    somme += (uint32_t)((trame->data).port_source) + (uint32_t)((trame->data).port_destination) + (uint32_t)((trame->data).longueur);
    ptr += (sizeof(TrameIP)-sizeof(DataUDP))/sizeof(uint16_t);

    for(i = 0; i < (uint8_t)(dataSize/2); i++) {
        somme += (uint32_t)swap_uint16(*ptr);
        ptr++;
    }

    if(dataSize%2 != 0) {
        ptr2 = ((uint8_t *)ptr);
        somme += ((uint16_t)*ptr2)<<8;
    }

    carry = (uint16_t)((somme & 0xffff0000)>>16);

    while(carry != 0) {
        somme &= 0x0000ffff;
        somme += carry;
        carry = (uint16_t)((somme & 0xffff000)>>16);
    }

    (trame->data).checksum = ~((uint16_t)somme);

}

void calcul_checksum_ip(TrameIP *trame) {

    uint32_t somme = 0;
    uint16_t carry = 0;
    uint16_t *ptr = (uint16_t *)trame;
    uint8_t i;

    for(i = 0; i < ((sizeof(TrameIP)-sizeof(TrameUDP))/sizeof(uint16_t)); i++) {
        if(i != 5) {
            somme += (uint32_t)*ptr;
        }
        ptr++;
    }

    carry = (uint16_t)((somme & 0xffff0000)>>16);

    while(carry != 0){
        somme &= 0x0000ffff;
        somme += carry;
        carry = (uint16_t)((somme & 0xffff000)>>16);
    }

    trame->c5 = ~((uint16_t)somme);

}

void envoyer_trame(TrameIP *trame) {

	uint8_t *buffer = (uint8_t *)trame;
	uint8_t i;
	send_serial(END);
	for(i = 0; i < sizeof(TrameIP); buffer++, i++) {
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

void recevoir_UDP(char *rx_buffer) {

    char c;
    int cpt = 0;

    cli();
    while(1) {
        c = get_serial();
        if(c == END || cpt > 1023) {
            if(cpt != 0) {
                rx_complete = 1;
                break;
            }
        }
        else if(c == ESC) {
            c = get_serial();
            if(c == ESC_END) {
                rx_buffer[cpt] = (char)END;
            }
            else if(c == ESC_ESC) {
                rx_buffer[cpt] = (char)ESC;
            }
        }
        else {
            rx_buffer[cpt] = c;
        }   
        cpt++;
    }
    sei();

}

void traitement_UDP(char *rx_buffer, TrameIP *trame) {

    trame = (TrameIP *)rx_buffer;
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
	(trame->data).port_source = swap_uint16((trame->data).port_source);
    (trame->data).port_destination = swap_uint16((trame->data).port_destination);
    (trame->data).longueur = swap_uint16((trame->data).longueur);
    (trame->data).checksum = swap_uint16((trame->data).checksum);
    
    if((trame->data).data.RX.instruction == 2) {
        ID_tshirt = (trame->data).data.RX.valeur;
    }
    
}

ISR(USART_RX_vect) {

    rx_complete = 0;
    
}

int main(void) {

    init_serial(9600);
    sei();
    uint8_t v_capteurs[4];
    uint8_t i;
    char rx_buffer[1024];
    TrameIP trame;
    TrameIP trame_rx;

    while(1) {
   
        for(i = 0; i <= 3; i++) {
            ad_init(i);
            v_capteurs[i] = ad_sample();
        }

        cli();
        forger_trame(&trame, v_capteurs);
        envoyer_trame(&trame);
        sei();
    
        if(!rx_complete) {
            recevoir_UDP(rx_buffer);
            traitement_UDP(rx_buffer, &trame_rx);
        }
    }
    
    return 0;

}
