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
  float seconds;
  float distance;
  int rpm;
  TCCR1A = 0x00;
  DDRB &= ~0x01;
  PORTB |= 0x01;
  TCCR1B = 0XC5;
  int count = 1;
  double currentspeed;
  double tireCircumference = 2 * 3.14 * 3.25;
  DDRD |= 0x60;
  TCCR0A |= 0xA3;
  TCCR0B |= 0x05;
  float opto_seconds;
  float timepassed;
  int totalrotations = 0;
  

  TCCR1B = (1 << ICNC1) | (1 << ICES1) | (1 << CS12) | (1 << CS10);


  OCR0A = 255;

    while(1)
  {

    if((TIFR1 & (1 << 5)) == (1 << 5)){

      TCNT1 = 0;
      TIFR1 = 0b00100000;
      counter = ICR1;

    }

    opto_seconds = ((counter * 0.000064)*8); // converts counter value to seconds for 1 full rotation

    currentspeed = tireCircumference/opto_seconds;
    
    totalrotations = totalrotations + 0.125;

    timepassed = totalrotations*opto_seconds;
    
    printf("\nICR1: %u", ICR1);
    printf("     1 rotation: %f", opto_seconds);
    printf("     Speed(cm/s): %f.", currentspeed);
    printf("     Time Passed: %i",timepassed);
    printf("     Total Rotations: %i", totalrotations);
  }      
}