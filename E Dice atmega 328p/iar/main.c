#include <ioavr.h>
#include <intrinsics.h>
#include <stdlib.h>
#define E 0
#define RS 1
#define delTime 30000
#define D1 100  //3 distances
#define D2 50
#define D3 20
#define LCDPORT PORTB
#define LCDCTRL PORTC
 
static volatile int distance = 0;
static volatile int i=0;
static volatile long q=0;
static volatile int working=0;
static volatile int mode=0;
 
void init (void)
{
    DDRD=0x10;// the 4th as output for triggering, rest are the inputs
    PORTD=0x00;
    DDRC=0xFF;
    DDRB=0xFF;
    EICRA |= (1 << ISC00);  
    EICRA |= (1 << ISC11);
    EIMSK |= (1 << INT0);  
    EIMSK |= (1 << INT1);
    TCCR1B = 0;  
    TCCR0B|=(1<<CS00);
    TIMSK0|=(1<<TOIE0);
}
 
void send (unsigned char data){
    LCDCTRL|=(1<<E);
    LCDPORT=(data>>4);
    __delay_cycles(1000);
    LCDCTRL&=~(1<<E);
  __delay_cycles(1000);    
  LCDCTRL|=(1<<E);
    LCDPORT=(data);
    __delay_cycles(1000);
    LCDCTRL&=~(1<<E);
    __delay_cycles(1000);
}
 
void initlcd (void){
    __delay_cycles(100000);
    LCDCTRL&=~(1<<RS);
    send(0x33);
    send(0x32);
    send(0x28);
    send(0x06);
    send(0x0C);
    send(0x01);
    __delay_cycles(100000);
   
}
 
void clear (void){
    LCDCTRL&=~(1<<RS);
    send(0x01);
    __delay_cycles(400000);
    LCDCTRL|=(1<<RS);  
}
 
void goTo10 (void){
    LCDCTRL&=~(1<<RS);
    send(0x8A);
   __delay_cycles(6000);
    LCDCTRL|=(1<<RS);  
}
 
void goTo0 (void){
    LCDCTRL&=~(1<<RS);
    send(0x80);
   __delay_cycles(6000);
    LCDCTRL|=(1<<RS);  
}
 
int main(void)
{
    int r;
    int k;
    int check=mode;;
    init();
    initlcd();
    LCDCTRL|=(1<<RS);
 
      char txt[5];
      char n1[]="Object in ";
      txt[3]='c';
      txt[4]='m';
   __enable_interrupt();
 
    while(1)
      {
        if(mode==0)             // chcecking the operation mode
        {
          if(mode!=check)
          {
            clear();
            check=mode;
          }
         
            if(distance<1000 && distance>=100)   //conditions needed for coversion of integers to chars
            {
                txt[2]=distance%10+'0';
                distance=distance/10;
                txt[1]=distance%10+'0';
                distance=distance/10;
                txt[0]=distance%10+'0';
               
                for(r=0;r<5;r++)
                {
                  send(txt[r]);
                }
                goTo0();              
                __delay_cycles(5000000);
            }
             else if(distance<100 && distance>=10)
            {
                txt[2]=distance%10+'0';
                distance=distance/10;
                txt[1]=distance%10+'0';
                txt[0]=0+'0';
               
                for(r=0;r<5;r++)
                {
                  send(txt[r]);
                }
                goTo0();
                __delay_cycles(5000000);
            }
            else if(distance<10 && distance>0)
            {
                txt[2]=distance%10+'0';
                txt[1]=0+'0';
                txt[0]=0+'0';
               
                for(r=0;r<5;r++)
                {
                  send(txt[r]);
                }
                goTo0();
                __delay_cycles(5000000);
            }
            else if(distance<0)
            {
                send('-');
                send('-');
                send('-');  
                goTo0();              
            }
        }
        else
        {
         if(mode!=check)
          {
            clear();
            for(k=0;k<10;k++)
              {
                 send(n1[k]);
              }
            check=mode;
          }
          if(distance<D1&&distance>=D2)
          {          
              send('D');
              send('1');
              goTo10();            
              __delay_cycles(800000);
          }
          else if(distance<D2&&distance>=D3)
          {
              send('D');
              send('2');
              goTo10();          
              __delay_cycles(800000);
          }
          else if(distance<D3&&distance>=0)
          {
              send('D');
              send('3');
              goTo10();        
              __delay_cycles(800000);
          }
          else if(distance>=D1)
          {
              send('-');
              send('-');
              goTo10();            
          }        
        }
       if(working==0)   //Triggering the sensor
       {
         working=1;
          PORTD|=(1<<PD4);        
          __delay_cycles(300);
          PORTD &=~(1<<PD4);    
       }                
     }
}
 
 
#pragma vector = TIMER0_OVF_vect        //preventing the pushbutton oscilating
__interrupt void click()
{
  if(q<delTime)
  {
    q++;
  }
}
 
#pragma vector = INT0_vect
__interrupt void mier()
{
 
    if (i==1)                  //signal changes from high to low
    {
     
        TCCR1B=0;              //stoping the counter
       
        /*
          Theoretical recalculations of width of the pulse to distance
          16MHz=1/T => T=0.0625us
          (0.0625us*340m/s)/2=0.001063 cm
           distance=TCNT1*0.001063  with prescaler =1
           distance=TCNT1*0.001063*8 with prescaler =8
           1/900=0.0011
        */
       
       
        //Practical recalculations of width of the pulse to distance
 
        if(TCNT1>=28000)
        {
          distance=-1;
        }
        else if(TCNT1<400)
        {
          distance=-1;
        }
        else if(TCNT1<1600)
        {
          distance = TCNT1/108;    
        }else if(TCNT1>=1600 && TCNT1<6500)
        {
            distance=TCNT1/114;
        }
        else if(TCNT1>=6500)
        {
            distance=TCNT1/115;
        }
        TCNT1=0;                                //reset the counter
 
        i=0;
        working=0;
    }
 
    if (i==0 && working==1)                    /////signal changes from low to high
    {
        TCCR1B|=(1<<CS11);                     //starting the counter
        i=1;
    }
}
 
 #pragma vector = INT1_vect
__interrupt void switchMode()
{
  if(q==delTime)
  {
    if(mode==0)
  {  
    mode=1;  
   
  }else if (mode==1)
  {  
    mode=0;      
  }
  q=0;
  }
}