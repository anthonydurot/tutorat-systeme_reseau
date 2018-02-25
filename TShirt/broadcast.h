// Définition des structures

#pragma pack(push, 1)

typedef struct dataUDP {

    uint8_t 	id_tshirt;
    uint8_t 	accel_x;
    uint8_t 	accel_y;
    uint8_t 	accel_z;
    uint8_t 	temp;

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
