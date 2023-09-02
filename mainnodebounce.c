#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdbool.h>
#include "uart.h"
#include "millis.h"

// https://wokwi.com/projects/365047092561218561

// B (digital pin 8 to 13)
// C (analog input pins)
// D (digital pins 0 to 7)
#define LED_PIN 2
#define BUTTON_PIN 1

#define BIT_SET(a, b) ((a) |= (1ULL << (b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b)))) 




#define BUTTON_IS_CLICKED(PINB,BUTTON_PIN) !BIT_CHECK(PINB,BUTTON_PIN)
volatile int changes = 0;
int main(void){
    init_serial();

    millis_init();
    sei();

    BIT_SET(DDRB,LED_PIN); //OUTPUT MODE

    //Sätt till INPUT_PULLUP
    BIT_CLEAR(DDRB,BUTTON_PIN); // INPUT MODE
    BIT_SET(PORTB,BUTTON_PIN); 

    // DATA DIRECTION REGISTER = avgör mode
    // om output så skickar vi  1 eller 0 på motsvarande pinne på PORT
    // om input så läser vi  1 eller 0 på motsvarande pinne på PIN
    bool isOn = false;
    int old = changes;
    while(1){
        // Om kanppen är tryct så LYS
        // annars lys inte
        // PIND PINB
        if(BUTTON_IS_CLICKED(PINB, BUTTON_PIN)){ // KLICKAD!
            isOn = !isOn;
            changes++;
        }
        if(old != changes){
            printf("%d\n", changes);
            old = changes;

        }
        if(isOn)
            BIT_SET(PORTB, LED_PIN); 
        else
            BIT_CLEAR(PORTB, LED_PIN); 

    }
    return 0;
}
