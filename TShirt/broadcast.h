// TODO : Standardiser les types / structures

#ifndef BROADCAST_H
#define BROADCAST_H

/**** Constantes ****/

// Define pour IP
#define VERSION 	 0x04
#define L_ENTETE     0x05
#define TOS			 0x00
#define ID			 0x0000
#define FLAGS        0x02
#define OFFSET       0x0000
#define TTL          0x40
#define PROTOCOLE    0x11
#define IP_SOURCE    "192.168.0.27"
#define IP_DEST      "192.168.0.26"

// Define pour UDP
#define PORT_SOURCE  4269
#define PORT_DEST    4000

// Define pour SLIP
#define END			 0xC0
#define ESC			 0xDB
#define ESC_END		 0xDC
#define ESC_ESC      0xDD

/**** Structures ****/

#pragma pack(push, 1)

typedef struct dataUDP_RX {
    uint8_t 	id_tshirt;
    uint8_t     instruction;
    uint8_t     valeur;
} DataUDP_RX;

typedef struct dataUDP_TX {
    uint8_t 	id_tshirt;
    uint8_t 	accel_x;
    uint8_t 	accel_y;
    uint8_t 	accel_z;
    uint8_t 	temp;
} DataUDP_TX;

typedef union dataUDP {
    DataUDP_TX  TX;  
    DataUDP_RX  RX;
} DataUDP;

typedef struct trameUDP {
    uint16_t 	port_source;
    uint16_t 	port_destination;
    uint16_t 	longueur;
    uint16_t 	checksum;
    DataUDP	    data;
} TrameUDP;

typedef struct trameIP { // Sur 16 bits pour faciliter le checksum
    uint16_t 	c0;
    uint16_t 	c1;
    uint16_t 	c2;
    uint16_t 	c3;
    uint16_t 	c4;
    uint16_t 	c5;
    uint16_t 	c6;
    uint16_t 	c7;
    uint16_t 	c8;
    uint16_t 	c9;
    TrameUDP 	data;
} TrameIP;

#pragma pack(pop)

/**** Prototypes ****/

void calcul_checksum_ip(TrameIP *);
void calcul_checksum_udp(TrameIP *);
uint16_t swap_uint16(uint16_t);
void forger_trameUDP(TrameUDP *, uint8_t *);
void forger_trameIP(TrameIP *, uint8_t *);
void envoyer_trame(TrameIP *);

#endif
