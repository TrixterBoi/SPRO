#define F_CPU 16000000UL
#define BAUD 9600UL
#define CIRCUMFERENCE 20 // cm
#define SLICES 8 // number of slices (holes and solid parts) on the encoder wheel
#define SNAPPINESS 0.8 // factor setting how aggressively/smoofly the speed is updated
#define SMOOTHING 0.05
#define DELTA_PWM ((target_speed-measured_speed)*SNAPPINESS)
​
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <math.h>
​
#include "usart.h"
​
float opto();
​
int duration = 25; // sec
int path_distance = 200; // cm
float total_revolutions = 0;
​
bool interrupt = false;
​
long loops = 0;
​
float measured_speed;
float target_speed;
float elapsed_time = 0;
float current_distance = 0;
float init_rev_sec;
float revolution_sec;
float prev_sec;
​
int main(){
    uart_init();   // open the communication to the microcontroller
    io_redirect(); // redirect input and output to the communication
​
    DDRD |= 0x60;
    TCCR0A |= 0xA3;
    TCCR0B |= 0x05;
    TCCR1A = 0x00;
    DDRB &= ~0x01;
    PORTB |= 0x01;
    TCCR1B = (1 << ICNC1) | (1 << ICES1) | (1 << CS12) | (1 << CS10);
​
    OCR0A = 255;
​
    prev_sec = opto();
​
    interrupt = false;
​
    while(1){
        init_rev_sec = (1-SMOOTHING)*opto() + SMOOTHING*prev_sec; // moving average smoothing
        revolution_sec = init_rev_sec/(1-pow(SMOOTHING, (loops+1))); // new readings weigh more
        printf("\n sec per rev: %.2f", revolution_sec);
        
        // if(revolution_sec != prev_sec){
            measured_speed = CIRCUMFERENCE/revolution_sec;
        
            current_distance += CIRCUMFERENCE/SLICES;
​
            target_speed = (path_distance-current_distance)/(duration-elapsed_time);
        // }
​
        printf("     Measured speed: %.2f", measured_speed);
        printf("     Target speed: %.2f", target_speed);
        printf("     Current distance: %.2f", current_distance);
        printf("     Elapsed time: %.2f", elapsed_time);
        printf("     OCR0A: %d", OCR0A);
        printf("     Elapsed time: %.2f", elapsed_time);
​
        if((OCR0A + DELTA_PWM < 255) && (OCR0A + DELTA_PWM > 0)){
            OCR0A += DELTA_PWM;
            printf("     adjusted");
        }
       
        prev_sec = revolution_sec;
        interrupt = false;
        loops++;
​
    }
​
}
​
float opto(){
    unsigned int opto_time;
    float opto_sec;
    while(!(TIFR1 & (1 << ICF1))){
  
    }
    interrupt = true;
    TCNT1 = 0;
    TIFR1 = 0b00100000;
    opto_time = ICR1;
    elapsed_time += opto_time*0.000064;
    opto_sec = ((float)opto_time)*0.000064*SLICES; // convert time into seconds
    return opto_sec;
}