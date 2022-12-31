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
char readBuffer[100];

// Variables

unsigned int counter;
float inputseconds1;
float inputdistance1;
float inputseconds2;
float inputdistance2;
float opto_seconds;
float opto_time;
float targetspeed;
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
float currentspeed;
float speedtoreach;
float speed;
float one;
float speedcontrol;
int rpm;
int count = 0;
int speedcheck;
int check = 0;
double tireCircumference = 2 * 3.14 * 3.25;
double currenttime;
double voltage;


// PID constants

double kp = 6;
double ki = 0.1;
// double kd = 1;

// PID function variables

float error, lastError, setSpeed, cumError, rateError;

// Functions

float computePi();
float time();
unsigned int read_adc();

int main(void)
{
  // Registers

  uart_init();//initialize communication with PC - debugging
  io_redirect();//redirect printf function to uart, so text will be shown on PC

  // printf("page0%c%c%c",255,255,255);//init at 9600 baud.
  _delay_ms(20);


  TCCR1A = 0x00;
  DDRB &= ~0x01;
  PORTB |= 0x01;
  TCCR1B = 0XC5;
  DDRD |= 0x60;
  TCCR0A |= 0xA3;
  TCCR0B |= 0x05;
  TCCR1B = (1 << ICNC1) | (1 << ICES1);


  while (1) 
  {
    voltage = read_adc();
    // printf("page1.n0.val=%d%c%c%c", voltage, 255,255,255);

    for(int i = 0; i<7; i++)
    {
      scanf("%c", &readButton[i]);
      
      if(readButton[i] == 0x1A)//some error occurred - retrieve the 0xFF commands and start over
      {
          scanf("%c", &readButton[i]);
          scanf("%c", &readButton[i]);
          scanf("%c", &readButton[i]);
          continue;
      }
    }

    if(readButton[1] == 0x02 && readButton[2] == 0x03)
    {
      printf("get %s.val%c%c%c","n1",255,255,255);
      for(int i = 0; i<8;i++)
      {
        scanf("%c", &readNumber[i]);
      }

      for(int i = 0 ; i < 8 ; i++)
      {
        inputdistance1 = (readNumber[1]) | (readNumber[2] << 8) | (readNumber[3] << 16) | (readNumber[4] << 24);
      }

      printf("get %s.val%c%c%c","n2",255,255,255);

      for(int i = 0; i<8;i++)
      {
        scanf("%c", &readNumber[i]);
      }

      for(int i = 0 ; i < 8 ; i++)
      {
        inputseconds1 = (readNumber[1]) | (readNumber[2] << 8) | (readNumber[3] << 16) | (readNumber[4] << 24);
      }
    }

    for(int i = 0; i<7; i++)
    {
      scanf("%c", &readButton[i]);
      
      if(readButton[i] == 0x1A)//some error occurred - retrieve the 0xFF commands and start over
      {
          scanf("%c", &readButton[i]);
          scanf("%c", &readButton[i]);
          scanf("%c", &readButton[i]);
          continue;
      }
    }

    if(readButton[1] == 0x03 && readButton[2] == 0x03)
    
    {
      printf("get %s.val%c%c%c","n1",255,255,255);

      for(int i = 0; i<8;i++)
      {
          scanf("%c", &readNumber[i]);
      }

      for(int i = 0 ; i < 8 ; i++)
      {
        inputdistance2 = (readNumber[1]) | (readNumber[2] << 8) | (readNumber[3] << 16) | (readNumber[4] << 24);
      }
      
      printf("get %s.val%c%c%c","n2",255,255,255);

      for(int i = 0; i<8;i++)
      {
        scanf("%c", &readNumber[i]);
      }

      for(int i = 0 ; i < 8 ; i++)
      {
        inputseconds2 = (readNumber[1]) | (readNumber[2] << 8) | (readNumber[3] << 16) | (readNumber[4] << 24);
      }
    }

    for(int i = 0; i<7; i++)
    {
      scanf("%c", &readButton[i]);
      
      if(readButton[i] == 0x1A)//some error occurred - retrieve the 0xFF commands and start over
      {
          scanf("%c", &readButton[i]);
          scanf("%c", &readButton[i]);
          scanf("%c", &readButton[i]);
          continue;
      }
    }

    if(readButton[1] == 0x04 && readButton[2] == 0x01)
    
    {
      targetspeed1 = inputdistance1/inputseconds1;
      targetspeed2 = inputdistance2/inputseconds2;
      int motor = motorcontrol();
    }

  } 
}

int motorcontrol(void)
{ 
  count = 0;


  // Motor speed
  TCCR1B |= (1 << CS12) | (1 << CS10);
  OCR0A = 255;

  while(1)
  {
    
    count++;

    opto_time = time();


    
    if(voltage > 6.8)
    {
      OCR0A = 0;
      printf("page 7%c%c%c",255,255,255);//init at 9600 baud.
      check = 2;
    }

    if(opto_time != previoustime)
    {
      currentspeed = tireCircumference/opto_time;
      
      if(check == 0)
      {
        printf("page 4%c%c%c",255,255,255);
        
        totalrotations1 = totalrotations1 + 0.125;

        timepassed1 = timepassed1 + (opto_time/8);
              
        distancecovered1 = totalrotations1 * (tireCircumference);

        distanceleft1 = inputdistance1 - distancecovered1;

        timeleft1 = inputseconds1 - timepassed1;

        targetspeed1 = distanceleft1/timeleft1;

        targetspeed = targetspeed1;

        // DEBUGGING PRINTS BELOW

        // printf("\nICR1: %u", counter);
        // printf("     Time Passed: %f",timepassed1);
        // printf("     Speed(cm/s): %f.", currentspeed);
        // printf("     Target speed: %f",targetspeed1);
        // printf("     Total Rotations: %f", totalrotations1);
        // printf("     Distance covered: %f", distancecovered1);
        // printf("     Distance left: %f", distanceleft1);

        printf("page4.x0.val=%d%c%c%c", (int)currentspeed*100, 255,255,255);
        printf("page4.x1.val=%d%c%c%c", (int)distancecovered1*100, 255,255,255);
        printf("page4.x2.val=%d%c%c%c", (int)distanceleft1*100, 255,255,255);
        printf("page4.n0.val=%d%c%c%c", (int)OCR0A, 255,255,255);
        printf("page4.x3.val=%d%c%c%c", (int)voltage*10, 255,255,255);

        if( distancecovered1 == inputdistance1 | distancecovered1 > inputdistance1)
        {
          check = 1;
          OCR0A = 0;

          // printf("\nCondition 1 has been met.\n");
          // printf("\nCondition 2 will start in 5 seconds.\n");
          _delay_ms(5000);
          OCR0A = 255;

        }
      }

      if(check == 1)
      {

        printf("page 5%c%c%c",255,255,255);

        totalrotations2 = totalrotations2 + 0.125;

        timepassed2 = timepassed2 + (opto_time/8);
              
        distancecovered2 = totalrotations2 * (tireCircumference);

        distanceleft2 = inputdistance2 - distancecovered2;

        timeleft2 = inputseconds2 - timepassed2;

        targetspeed2 = distanceleft2/timeleft2;

        targetspeed = targetspeed2;

        //DEBUGGING PRINTS BELOW

        // printf("\nICR1: %u", counter);
        // printf("     Time Passed: %f",timepassed2);
        // printf("     Speed(cm/s): %f.", currentspeed);
        // printf("     Target speed: %f",targetspeed2);
        // printf("     Total Rotations: %f", totalrotations2);
        // printf("     Distance covered: %f", distancecovered2);
        // printf("     Distance left: %f", distanceleft2);

        printf("page5.x0.val=%d%c%c%c", (int)currentspeed*100, 255,255,255);
        printf("page5.x1.val=%d%c%c%c", (int)distancecovered2*100, 255,255,255);
        printf("page5.x2.val=%d%c%c%c", (int)distanceleft2*100, 255,255,255);
        printf("page5.n0.val=%d%c%c%c", (int)OCR0A, 255,255,255);
        printf("page5.x3.val=%d%c%c%c", (int)voltage*10, 255,255,255);


      }

      // Below, there is stuff that should be in the while loop -
      // first executing the computePi function, then checking and assigning the value to OCR0A
      speed = currentspeed;
      control = computePi();
      speedcontrol = OCR0A + control;
      if(speedcontrol < OCR0A)
      {
        speedcheck = 0;
      }
      else if(speedcontrol > OCR0A)
      {
        speedcheck = 1;
      }
      // Checking if the control value is between 0 and 255 to avoid a fatal error
      if (speedcontrol > 0 && speedcontrol < 255)
      {
        // control = (int)control;
        OCR0A += control;
        // printf("    OCRA: %d", OCR0A);
      }
      if( (distancecovered2 == inputdistance2) | (distancecovered2 > inputdistance2))
      {
        check = 2;
        OCR0A = 0;
        // printf("\nCondition 2 has been met.\n");
        // printf("\nThank you for joining us on this horrendeous journey.\n");

        printf("page 6%c%c%c",255,255,255);        

      }
    }
    previoustime = opto_seconds;
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

float computePi()
{
  // There is no time involved in calculating the derivative and cumulative error
  // because of the equal delay between every function execution
  // printf("     adjusted");

  error = targetspeed - speed;
  // printf("     Error: %f",error);
  // printf("     Target speed: %f",targetspeed1);
  cumError += error;

  double out = kp*error + ki*cumError;
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
