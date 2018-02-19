#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "serial.h"
#include "analog.h"

#define MAC_SIZE	6
#define IPV4_SIZE	4

int main(void)
{
    init_printf();
    ad_init(0);
    int val;
    while(1) {
        val = (((ad_sample() * 0.00488) - 0.5) * (-100)); // Test du capteur de temperature
        printf("Valeur : %d\n",val); 
    }  
    
    return 0;
}
