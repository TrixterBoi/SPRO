#define F_CPU 16000000UL //needs to be defined for the delay functions to work.
#define BAUD 9600
#define NUMBER_STRING 1001

#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h> //here the delay functions are found
#include "usart.h"

int initialization(void);
int time(void);
int motor(void);
int rpm;

int main(void)
{
  char readButton[100];
  char readNumber[100];
  uart_init();//initialize communication with PC - debugging
  io_redirect();//redirect printf function to uart, so text will be shown on PC
  
  //do
  {
    
    printf("page 2%c%c%c",255,255,255);//init at 9600 baud.
    _delay_ms(20);

    while (1) 
    {
      int rpm = 30;
      //rpm = time();
      //printf("page1.n0.val=%d%c%c%c", (int)rpm, 255,255,255);
      _delay_ms(100);

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
      if(readButton[1] == 0x02 && readButton[2] == 0x06)
      {
        printf("page2.n1.val=%d%c%c%c",100, 255,255,255);
      }
      if(readButton[1] == 0x02 && readButton[2] == 0x07)
      {
        printf("page2.n1.val=%d%c%c%c",200, 255,255,255);
      }
      if(readButton[1] == 0x02 && readButton[2] == 0x08)
      {
        printf("page2.n1.val=%d%c%c%c",300, 255,255,255);
      }
      if(readButton[1] == 0x02 && readButton[2] == 0x09)
      {
        printf("page2.n2.val=%d%c%c%c",25, 255,255,255);
      }
      if(readButton[1] == 0x02 && readButton[2] == 0x0A)
      {
        printf("page2.n2.val=%d%c%c%c",30, 255,255,255);
      }
      if(readButton[1] == 0x02 && readButton[2] == 0x0B)
      {
        printf("page2.n2.val=%d%c%c%c",35, 255,255,255);
      }
      if(readButton[1] == 0x02 && readButton[2] == 0x05)
      {

        printf("get %s.val%c%c%c","page2.n1",255,255,255);

        for(int i = 0; i<8;i++)
        {
            scanf("%c", &readNumber[i]);
        }

        for(int i = 0 ; i < 8 ; i++)
        {
          int centimeters = readNumber[1] | readNumber[2] << 8 | readNumber[3] << 16 | readNumber[4] << 24;
        }

        printf("get %s.val%c%c%c","page2.n2",255,255,255);

        for(int i = 0; i<8;i++)
        {
          scanf("%c", &readNumber[i]);
        }

        for(int i = 0 ; i < 8 ; i++)
        {
          int seconds = readNumber[1] | readNumber[2] << 8 | readNumber[3] << 16 | readNumber[4] << 24;
        }

        //motor function called
        
      }
    } 
  }
  //while (read_adc() > 6.8);
}


int time(void)
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
  double speed;
  double tireCircumference = 2 * 3.14 * 3.25;
  DDRD |= 0x60;
  TCCR0A |= 0xA3;
  TCCR0B |= 0x05;

  

  TCCR1B = (1 << ICNC1) | (1 << ICES1) | (1 << CS12) | (1 << CS10);


  OCR0A = 255;

  while(1)
  {

    if((TIFR1 & (1 << 5)) == (1 << 5)){

      TCNT1 = 0;
      TIFR1 = 0b00100000;
      counter = ICR1;

    }      
    printf("\nICR1: %u", counter);

    seconds = ((counter * 0.000064)*8); // converts counter value to seconds for 1 full rotation

    printf("          One Rotation: %f",seconds);

    rpm = 60/seconds;
    speed = tireCircumference/seconds;
    
    printf("          Speed(cm/s): %f.", speed);

  }      
  return seconds;
}

int motor()
{


}

unsigned int read_adc(void)
{
  ADMUX = ADMUX | 0X40;
  ADCSRB = ADCSRB & (0XF8);
  ADCSRA = ADCSRA | 0XE7;
  unsigned int adclow = ADCL;
  return (adclow + ((ADCH & 0x03) << 8));
}

// Variables we need anyway for the speed and distance calculations
double speed;
double tireCircumference = 2 * 3.14 * 6.5;

// PID constants
double kp = 1;
double kd = 1;
double ki = 1;

// PID function variables
double error, lastError, setSpeed, cumError, rateError, control;

int motorcontrol(void){

  
  DDRD |= 0x60;
  TCCR0A |= 0xA3;
  TCCR0B |= 0x05;
  int count = 0;

  cumError = 0;
  lastError = 0;
  // Below, there is stuff that should be in the while loop -
  // first executing the computePid function, then checking and assigning the value to OCR0A
  while(1)
  {
      speed = rpm * tireCircumference;
      control = computePid(speed);

      // Checking if the control value is between 0 and 255 to avoid a fatal error
      if (control > 0 && control < 255){
          control = (int)control;
          OCR0A = control;
      }
  }
}

// There is no time involved in calculating the derivative and cumulative error
// because of the equal delay between every function execution

float computePid(double speed){
    error = setSpeed - speed;
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