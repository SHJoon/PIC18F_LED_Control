#include <p18f4321.h>
#include <stdio.h>
#include <math.h>
#pragma config OSC = INTIO2
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOR =OFF

//Prototype Area
void Init_IO(void);
void Do_Init(void);

void Init_IO()
{
    TRISA = 0x00;                       //Set Port A as output
    TRISB = 0x03;                       //Set PORT B, bit 0 and 1 as inputs and others as output
    TRISC = 0x00;                       //Set Port C as output
    TRISD = 0x00;                       //Set Port D as output
    TRISE = 0x00;                       //Set Port E as output
}

void Do_Init()
{
    Init_IO();
    ADCON0 = 0x01;                      //Change AN0 from analog to digital signal
    ADCON1=0x0E;                        //Change 0x0E from analog to digital signal
    ADCON2 = 0xA9;                      //Change 0xA9 from analog to digital signal
}

void main()
{
    Do_Init();
    while(1)
    {                                   //7 6 5 4 3 2 1 0
        PORTA = 0x05 << 2;              //x x 0 1 0 1 x x
        PORTB = 0x05 << 2;              //x x 0 1 0 1 x x
        PORTD = 0x55;                   //0 1 0 1 0 1 0 1
        
        for(int i = 0; i < 10000; i++);
        
        PORTA = 0x0A << 2;
        PORTB = 0x0A << 2;
        PORTD = 0xAA;
        
        for(int i = 0; i < 10000; i++);
    }
}