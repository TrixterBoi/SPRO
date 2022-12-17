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
float inputseconds1;
float inputdistance1;
float inputseconds2;
float inputdistance2;
float opto_seconds;
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
int rpm;
int count = 0;
int totalrotations1 = 0;
int totalrotations2 = 0;
int speedcheck;
int check = 0;
double tireCircumference = 2 * 3.14 * 3.25;
double currentspeed;
double targetspeed;
double currenttime;
double voltage;


// PID constants

double kp = 1;
double kd = 1;
double ki = 1;

// PID function variables

double error, lastError, setSpeed, cumError, rateError, control;

// Registers

TCCR1A = 0x00;
DDRB &= ~0x01;
PORTB |= 0x01;
TCCR1B = 0XC5;
DDRD |= 0x60;
TCCR0A |= 0xA3;
TCCR0B |= 0x05;
TCCR1B = (1 << ICNC1) | (1 << ICES1) | (1 << CS12) | (1 << CS10);

// Motor speed

OCR0A = 255;

// Functions

int main(void)
{
  {
    uart_init();//initialize communication with PC - debugging
    io_redirect();//redirect printf function to uart, so text will be shown on PC
    
    printf("page 0%c%c%c",255,255,255);//init at 9600 baud.
    _delay_ms(20);

    while (1) 
    {
      voltage = read_adc();
      printf("page1.n0.val=%d%c%c%c", voltage, 255,255,255);
      // for(int i = 0; i<7; i++)
      // {
      //         scanf("%c", &readButton[i]);
      //         if(readButton[i] == 0x1A)//some error occurred - retrieve the 0xFF commands and start over
      //         {
      //             scanf("%c", &readButton[i]);
      //             scanf("%c", &readButton[i]);
      //             scanf("%c", &readButton[i]);
      //             continue;
      //         }
      // }
      // if(readButton[1] == 0x02 && readButton[2] == 0x06)
      // {
      //   printf("page2.n1.val=%d%c%c%c",100, 255,255,255);
      // }
      // if(readButton[1] == 0x02 && readButton[2] == 0x07)
      // {
      //   printf("page2.n1.val=%d%c%c%c",200, 255,255,255);
      // }
      // if(readButton[1] == 0x02 && readButton[2] == 0x08)
      // {
      //   printf("page2.n1.val=%d%c%c%c",300, 255,255,255);
      // }
      // if(readButton[1] == 0x02 && readButton[2] == 0x09)
      // {
      //   printf("page2.n2.val=%d%c%c%c",25, 255,255,255);
      // }
      // if(readButton[1] == 0x02 && readButton[2] == 0x0A)
      // {
      //   printf("page2.n2.val=%d%c%c%c",30, 255,255,255);
      // }
      // if(readButton[1] == 0x02 && readButton[2] == 0x0B)
      // {
      //   printf("page2.n2.val=%d%c%c%c",35, 255,255,255);
      // }
      
      if(readButton[1] == 0x02 && readButton[2] == 0x05)
      
      {
        printf("get %s.val%c%c%c","page2.n1",255,255,255);

        for(int i = 0; i<8;i++)
        {
            scanf("%c", &readNumber[i]);
        }

        for(int i = 0 ; i < 8 ; i++)
        {
          inputdistance1 = readNumber[1] | readNumber[2] << 8 | readNumber[3] << 16 | readNumber[4] << 24;
        }

        printf("get %s.val%c%c%c","page2.n2",255,255,255);

        for(int i = 0; i<8;i++)
        {
          scanf("%c", &readNumber[i]);
        }

        for(int i = 0 ; i < 8 ; i++)
        {
          inputseconds1 = readNumber[1] | readNumber[2] << 8 | readNumber[3] << 16 | readNumber[4] << 24;
        }       
      }

      if(readButton[1] == 0x02 && readButton[2] == 0x05)
      
      {
        printf("get %s.val%c%c%c","page3.n1",255,255,255);

        for(int i = 0; i<8;i++)
        {
            scanf("%c", &readNumber[i]);
        }

        for(int i = 0 ; i < 8 ; i++)
        {
          inputdistance2 = readNumber[1] | readNumber[2] << 8 | readNumber[3] << 16 | readNumber[4] << 24;
        }

        printf("get %s.val%c%c%c","page3.n2",255,255,255);

        for(int i = 0; i<8;i++)
        {
          scanf("%c", &readNumber[i]);
        }

        for(int i = 0 ; i < 8 ; i++)
        {
          inputseconds2 = readNumber[1] | readNumber[2] << 8 | readNumber[3] << 16 | readNumber[4] << 24;
        }

        targetspeed1 = inputdistance1/inputseconds1;
        targetspeed2 = inputdistance2/inputseconds2;

        int motor = motorcontrol();

        printf("page4.n0.val=%d%c%c%c", (int)currentspeed, 255,255,255);
        printf("page4.n1.val=%d%c%c%c", (int)distancecovered1, 255,255,255);
        printf("page4.n2.val=%d%c%c%c", (int)distanceleft1, 255,255,255);
        printf("page4.n3.val=%d%c%c%c", (int)speedcheck, 255,255,255);
        printf("page4.n4.val=%d%c%c%c", (int)voltage, 255,255,255);

        printf("page 5%c%c%c",255,255,255);

        printf("page5.n0.val=%d%c%c%c", (int)currentspeed, 255,255,255);
        printf("page5.n1.val=%d%c%c%c", (int)distancecovered2, 255,255,255);
        printf("page5.n2.val=%d%c%c%c", (int)distanceleft2, 255,255,255);
        printf("page5.n3.val=%d%c%c%c", (int)speedcheck, 255,255,255);
        printf("page5.n4.val=%d%c%c%c", (int)voltage, 255,255,255);

      }
    } 
  }
  while (voltage > 6.8);
  {
    OCR0A = 0;
    printf("page 6%c%c%c",255,255,255);//init at 9600 baud.
  }
}

int motorcontrol(void)
{ 
  
  count = 0;

  while(1)
  {
    
    count++;

    opto_seconds = time();
    
    currentspeed = tireCircumference/opto_seconds;
    
    if(check == 0)
    {
      totalrotations1 = totalrotations1 + 0.125;

      timepassed1 = totalrotations1*opto_seconds;
      
      printf("\nICR1: %u", counter);
      printf("     Speed(cm/s): %f.", currentspeed);
      printf("     Time Passed: %i",timepassed1);
      printf("     Total Rotations: %i", totalrotations1);
      
      distancecovered1 = totalrotations1 * (tireCircumference/8);

      distanceleft1 = distancecovered1 - inputdistance1;

      timeleft1 = inputseconds1 - timepassed1;

      targetspeed1 = distanceleft1/timeleft1;

      setSpeed = targetspeed1;

      if( distancecovered1 == inputdistance1 | distancecovered1 > inputdistance1)
      {
        check = 1
      }
    }

    if(check == 1)
    {

      _delay_ms(5000);

      totalrotations2 = totalrotations2 + 0.125;

      timepassed2 = totalrotations2*opto_seconds;
      
      printf("\nICR1: %u", counter);
      printf("     Speed(cm/s): %f.", currentspeed);
      printf("     Time Passed: %i",timepassed2);
      printf("     Total Rotations: %i", totalrotations2);
      
      distancecovered2 = totalrotations2 * (tireCircumference/8);

      distanceleft2 = distancecovered2 - inputdistance2;

      timeleft2 = inputseconds2 - timepassed2;

      targetspeed2 = distanceleft2/timeleft2;

      setSpeed = targetspeed2;

      previoustime = opto_seconds;
    }
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

int time(void)
{ 

  while(1)
  
  {

    if((TIFR1 & (1 << 5)) == (1 << 5)){

      TCNT1 = 0;
      TIFR1 = 0b00100000;
      counter = ICR1;

    }
    else
    {
      counter = 0;
    }

    opto_seconds = ((counter * 0.000064)*8); // converts counter value to seconds for 1 full rotation
  }      
  return opto_seconds;
}

float computePid(double currentspeed){

  // There is no time involved in calculating the derivative and cumulative error
  // because of the equal delay between every function execution

  error = setSpeed - currentspeed;
  cumError += error;
  if (lastError == 0){
      rateError = 0;
  } else {
      rateError = error - lastError;
  }
  double out = kp*error + ki*cumError + kd*rateError;
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