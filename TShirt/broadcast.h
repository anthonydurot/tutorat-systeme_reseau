// Définition des structures

#pragma pack(1)
typedef struct dataUDP {

    uint8_t 	id_tshirt;
    uint8_t 	accel_x;
    uint8_t 	accel_y;
    uint8_t 	accel_z;
    uint8_t 	temp;

} DataUDP;
#pragma pack()

#pragma pack(1)
typedef struct trameUDP {

    uint16_t 	port_source;
    uint16_t 	port_destination;
    uint16_t 	longueur;
    uint8_t 	checksum;
    DataUDP	    data;

} TrameUDP;
#pragma pack()

#pragma pack(1)
typedef struct trameIP {

    uint8_t		version_longueur_entete;
    uint8_t		TOS;
    uint16_t 	longueur_totale;
    uint16_t 	identificateur;
    uint16_t 	flags_offset;
    uint8_t		TTL;
    uint8_t		protocole;
    uint16_t 	checksum;
    uint8_t 	adr_source[4];
    uint8_t		adr_destination[4];
    TrameUDP 	data;

} TrameIP;
#pragma pack()
