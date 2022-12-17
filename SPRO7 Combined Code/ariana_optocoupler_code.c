#define F_CPU 16000000UL //needs to be defined for the delay functions to work.
#define BAUD 9600
#define NUMBER_STRING 1001

#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h> //here the delay functions are found
#include "usart.h"

int main(void)
{

uart_init();   // open the communication to the microcontroller
io_redirect(); // redirect input and output to the communication

  unsigned int counter;
  
  int index = 0;
  float seconds;
  float distance;
  int rpm;
  int SUM[10];

  TCCR1A = 0x00;
  DDRB &= ~0x01;
  PORTB |= 0x01;
  TCCR1B = 0XC5;
  int count = 1;
  double speed;
  double tireCircumference = 2 * 3.14 * 6.5;
  DDRD |= 0x60;
  TCCR0A |= 0xA3;
  TCCR0B |= 0x05;
  
  TCCR1B = (1 << ICNC1) | (1 << ICES1) | (1 << CS12) | (1 << CS10);

  OCR0A = 255;

  while (0)

  {

    TIFR1 = (1 << ICF1);                   // resets ICF1
    TCNT1 = 0;                             // Resets TCNT1

    counter = ICR1;                        // copies ICR1 into a local variable

    seconds = ((counter * 0.000064)*8); // converts counter value to minutes for 1 full rotation

    rpm = 60/seconds;

    _delay_ms(100);
 
    printf("The rpm is %f\n.", rpm);

  }

}