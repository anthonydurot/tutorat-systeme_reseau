#include <avr/io.h>

/** Functions for the analog converter **/

void ad_init(unsigned char channel)   
{   
    ADCSRA|=(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //Clock_ADC = Clock / 128 = 125 000 Hz
    ADMUX|=(1<<REFS0)|(1<<ADLAR); //AVCC with external capacitor at AREF pin
    ADMUX=(ADMUX&0xf0)|channel; //Channel = ADC0, ADC1,...,ADCN
    ADCSRA|=(1<<ADEN);
}   

unsigned int ad_sample(void){
    ADCSRA|=(1<<ADSC); //Start conversion
    while(bit_is_set(ADCSRA, ADSC)); //Attends la fin de la conversion
    return ADCH; //Retourne le registre 8 bits en virant les 2 de poids faible
}
