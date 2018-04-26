/*
 * GccApplication3.c
 *
 * Created: 2018-01-14 22:18:37
 * Author : Kamil
 */ 


#include <ioavr.h>  
#include <intrinsics.h> 
#include <string.h> 
#include <stdlib.h>

int type = 0;  // type/mode of 1 or 2 dices

int numbers[10] = {35,239,147,135,79,7,3,175,0,4}; // numbers to 7 seg. display


void display_number(int a)      // function to display prepared numbers on 3 digits display
{
  PORTD = numbers[a];
  
  if(numbers[a] == 7 || numbers[a]==3)
  {
    PORTB |= (1 << PB0);
  }
   else
     PORTB &=~(1 << PB0);
}


int losowanie()      // draw number
{ 
  int val = TCNT1%6 +1;     // counter value modulo 6 + 1 to achive numbers from 1-6
   return val;
}


void dig(int a, int b)   // function turning on proper segment on digit
{    
          PORTB &=~(1<<PB1);    // display first digit 
          display_number(a);
          PORTB |=(1<<PB2);
         __delay_cycles(80000);
       
          if(type==2)   // display both digits if type = 2
       {
          PORTB &=~(1<<PB2);
          PORTB |=(1<<PB1);
          display_number(b);
          __delay_cycles(80000);
       }
                        
}

void digits(int a, int b)   // function switching digits
{
  for(int i=0;;i++)
  {if(i >= 2)
    i=0;  
   if(i==0)
   {
    PORTB &=~(1<<PB1);
    display_number(b);
    PORTB |=(1<<PB2);
    __delay_cycles(5000);
   }
   
    if(i==1)
   {
    PORTB &=~(1<<PB2);
    display_number(a);
    PORTB |=(1<<PB1);
    __delay_cycles(5000);
   } 
  }
}


void mod()   // choosing one or two dices at the beggining
{  
  int out=0;
  
  for(int x=0;;x++)
  {    
    if(x%2 == 0)
    {
      for(long x=0;x<=250000;x++)
       {
        PORTB |= (1<<PB1);
        display_number(1);
        if(PINC & (1<<PC2))
        {
          type=1;
          out=1;
          break;
        }
      }
    }
    
      if(x%2 ==1)
      {
        for(long x=0;x<=250000;x++)
        {
            if(PINC & (1<<PC2))
            {
              type = 2;
              out=1;
              break;
            }            
        PORTB |= (1<<PB1);
        display_number(2);
        }
      } 
    if(out==1)
      break;
  }
}
int main(void)
{
  int a=0;
  int b=0;
  DDRB |= 0xFF;  // initiate ports
  DDRD |= 0xFF;
  DDRC |= 0xFF;
  TCCR1B|=(1<<CS00);  // turn on counter
    mod();

    while (1) 
    {
      a = losowanie();
      b = losowanie();     
           
         for(int x=0;;x++)
        {         
          dig(a,b); 
          if(PINC & (1<<PC2))
          {
           break; 
          }
        }
    }
}
