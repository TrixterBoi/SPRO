/*
 * HelloWorld.c
 *
 * Created: 12/9/2022 10:43:27 PM
 * Author : Alin
 */ 


 
#define F_CPU 16000000UL


#include <string.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#include "usart.h"
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#define PI (3.141592653589793)


int TIME, PPR; 
double velocity;
float Ang_V;
float carspeed;


int main(void) {    

    uart_init(); // open the communication to the microcontroller
	io_redirect(); // redirect input and output to the communication


DDRB = 0x00; //output
PORTB = 0x01; //pin 1-i
//Setup timer/counter as pure ticks counter, counting //clock ticks with 1024 prescaling ir. no interrupts
TCCR1A = 0X00;
TCCR1B = 0xC5;
//Input capturee on positive edge ICP1 (PBO)
//Filter is enabled 1024 clock prescaler
DDRB &= ~0x01;//pin1 as input for ICP1 use 
PORTB |= 0x01; //enable pullups
TCNT1=0;
//check ICR1 (which is a register) page.110
//Take a look at registers in big file
int count = 0;

while(1) 
{
    while( (TIFR1 & 1<<5) == (1<<5))
    {
        //Make sure 5th slot is recording data

        int temptime = 0; // delete if acceleration code does not work
        
        TIME = ICR1 * (64.0/100000); //Time diff between holes -> time is recorded
        PPR = TIME * 20; //number of holes in encoder wheel
        
        //delete from here if acceleration code does not work
        if(TIME >> temptime)
        {
            //braking = true;
        }
        if(TIME << temptime)
        {
            //accelerating = true;
        }
        temptime = TIME;
        //delete till here if acceleration code does not work

        //trash from here if optocoupler progress bar does not work with distancetravelled
        int onerotation = 1.570796326795;
        float distance = 31.4159265359;
        count = count++;
        int distancetravelled = onerotation * count; 
        //trash till here if optocoupler progress bar does not work with distancetravelled

        float carspeed = distance/PPR;

        // this is more complicated 
        // Ang_V = 2*PI/PPR;
        // velocity = Ang_V*11.5; //diameter of wheel * pi / rotational period;
        
        TCNT1 = 0;
        TIFR1 |= (1<<5);

        TCNT1 = 0;
        TIFR1 |=(1<<5);
    }

    //10-15 diameter


    printf("The speed of the car is %.2f m/s", carspeed);
}
return 0;       
}  
