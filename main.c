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
volatile int changes = 0;

#define BIT_SET(a, b) ((a) |= (1ULL << (b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b)))) 

volatile unsigned char buttonTickResult_PB1 = 0;

typedef enum{
    Click_status_Pressed,
    Click_status_Released,
    Click_status_Notsure,
} Click_status;

Click_status clicked(unsigned char ch){
    // Create a mask with the 5 lower bits set to 1 (0b00011111)
    unsigned char mask = 0b00011111;

    //00011111 5 senaste
    if((ch & mask) == mask) return Click_status_Pressed; // 5 sista är satta
    if((ch & mask) == 0) return Click_status_Released; // 5 sista är nollade
    return  Click_status_Notsure;
}

#define BUTTON_IS_CLICKED(PINB,BUTTON_PIN) !BIT_CHECK(PINB,BUTTON_PIN)
 
volatile unsigned int t = 0;


volatile int n = 0;

ISR(TIMER2_OVF_vect)
{
    TCNT2 = 5; // Timer Preloading
  // Handle The Timer Interrupt
  //...
    bool sample = BUTTON_IS_CLICKED(PINB,BUTTON_PIN);
    // if(sample)
    //     printf("P");
    // else 
    //     printf("N");

    // 1011111  -> 8 st millisekunder
    buttonTickResult_PB1 = buttonTickResult_PB1 << 1 | sample;
}



void timer0_init()
{
  TCCR2A = 0;           // Init Timer2A
  TCCR2B = 0;           // Init Timer2B
  TCCR2B |= 0B00000111;  // Prescaler = 1024
  TCNT2 = 5;        // Timer Preloading
  TIMSK2 |= 0B00000001;  // Enable Timer Overflow Interrupt
}



int main(void){
    init_serial();
    timer0_init();
    sei();

    BIT_SET(DDRB,LED_PIN); //OUTPUT MODE

    //Sätt till INPUT_PULLUP
    BIT_CLEAR(DDRB,BUTTON_PIN); // INPUT MODE
    BIT_SET(PORTB,BUTTON_PIN); 

    // DATA DIRECTION REGISTER = avgör mode
    // om output så skickar vi  1 eller 0 på motsvarande pinne på PORT
    // om input så läser vi  1 eller 0 på motsvarande pinne på PIN
    bool isOn = true;
    bool canRelease = false;
    int old = changes;
    while(1){
        // Om kanppen är tryct så LYS
        // annars lys inte
        // PIND PINB
        Click_status status = clicked(buttonTickResult_PB1);
        if(status == Click_status_Pressed) canRelease = true;
        if(canRelease && status == Click_status_Released) {
            isOn = !isOn;
            changes++;
            canRelease = false;
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
