#define F_CPU 16000000UL
#define BAUD 9600
#define NUMBER_STRING 1001

#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include "usart.h"

// Arrays

char readButton[100];
char readNumber[100];

// Variables

unsigned int counter;
float inputseconds1 = 25;
float inputdistance1 = 100;
float inputseconds2 = 55;
float inputdistance2 = 300;
float opto_seconds;
float opto_time;
float targetspeed1;
float targetspeed2;
float speeddiff1;
float speeddiff2;
float distancecovered1;
float distancecovered2;
float distanceleft1;
float distanceleft2;
float previoustime;
float timeleft1;
float timeleft2;
float timepassed1;
float timepassed2;
float control;
float totalrotations1 = 0;
float totalrotations2 = 0;
int rpm;
int count = 0;
int speedcheck;
int check = 0;
double tireCircumference = 2 * 3.14 * 3.25;
float currentspeed;
double targetspeed;
double currenttime;
double voltage;


// PID constants

double kp = 1;
// double kd = 1;
double ki = 1;

// PID function variables

double error, lastError, setSpeed, cumError, rateError;

// Functions

float computePid();
float time();

int main(void)
{
  {
    // Registers

    TCCR1A = 0x00;
    DDRB &= ~0x01;
    PORTB |= 0x01;
    TCCR1B = 0XC5;
    DDRD |= 0x60;
    TCCR0A |= 0xA3;
    TCCR0B |= 0x05;
    TCCR1B = (1 << ICNC1) | (1 << ICES1);

    // Motor speed
    TCCR1B |= (1 << CS12) | (1 << CS10);
    OCR0A = 255;

    uart_init();//initialize communication with PC - debugging
    io_redirect();//redirect printf function to uart, so text will be shown on PC
    
    int motor = motorcontrol();


  }
}

int motorcontrol(void)
{ 
  count = 0;

  while(1)
  {
    
    count++;

    opto_time = time();
    
    if(opto_time != previoustime)
    {
      currentspeed = tireCircumference/opto_time;
      
      if(check == 0)
      {
        totalrotations1 = totalrotations1 + 8;

        timepassed1 = timepassed1 + (opto_time/8);
              
        distancecovered1 = totalrotations1 * (tireCircumference/8);

        distanceleft1 = inputdistance1 - distancecovered1;

        timeleft1 = inputseconds1 - timepassed1;

        targetspeed1 = distanceleft1/timeleft1;

        setSpeed = targetspeed1;

        // printf("\nICR1: %u", counter);
        // printf("     Time Passed: %f",timepassed1);
        printf("\nSpeed(cm/s): %f.", currentspeed);
        printf("     Target speed: %f",targetspeed1);
        printf("     Total Rotations: %i", totalrotations1);
        printf("     Distance covered: %f", distancecovered1);
        // printf("     Distance left: %f", distanceleft1);


        if( distancecovered1 == inputdistance1 | distancecovered1 > inputdistance1)
        {
          check = 1;
        }
      }

      if(check == 1)
      {
        totalrotations2++;

        timepassed2 = timepassed2 + (opto_time/8);
              
        distancecovered2 = totalrotations2 * (tireCircumference/8);

        distanceleft2 = inputdistance2 - distancecovered2;

        timeleft2 = inputseconds2 - timepassed2;

        targetspeed2 = distanceleft2/timeleft2;

        setSpeed = targetspeed2;

        // printf("\nICR1: %u", counter);
        printf("\nSpeed(cm/s): %f.", currentspeed);
        printf("     Target speed: %f",targetspeed2);
        // printf("     Time Passed: %f",timepassed2);
        // printf("     Total Rotations: %i", totalrotations2);
        printf("     Distance covered: %f", distancecovered2);
        printf("     Distance left: %f", distanceleft2);
      }
    }
    else
    {
      currentspeed = currentspeed - 5;
    }
    previoustime = opto_seconds;
  }

  // Below, there is stuff that should be in the while loop -
  // first executing the computePid function, then checking and assigning the value to OCR0A

  while(1)
  {
    control = computePid(currentspeed);

    // Checking if the control value is between 0 and 255 to avoid a fatal error
    if (control > 0 && control < 255)
    {
      control = (int)control;
      OCR0A = control;
    }
  }
}

float time(void)
{ 
  while(1)
  
  {
    while(!((TIFR1 & (1 << 5))))
    {

    }
    
    TCNT1 = 0;
    TIFR1 = 0b00100000;
    counter = ICR1;
    opto_seconds = (((float)counter * 0.000064)*8); // converts counter value to seconds for 1 full rotation
    return opto_seconds;
  }      
}

float computePid(currentspeed)
{

  // There is no time involved in calculating the derivative and cumulative error
  // because of the equal delay between every function execution

  error = setSpeed - currentspeed;
  cumError += error;
  if (lastError == 0){
      rateError = 0;
  } else {
      rateError = error - lastError;
  }
  double out = kp*error + ki*cumError;  // + kd*rateError;  --  will not use derivative as it is too aggressive  
  lastError = error;
  return out;
}

unsigned int read_adc(void)
{
  ADMUX = ADMUX | 0X40;
  ADCSRB = ADCSRB & (0XF8);
  ADCSRA = ADCSRA | 0XE7;
  unsigned int adclow = ADCL;
  return (adclow + ((ADCH & 0x03) << 8));
}