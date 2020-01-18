#include <p18f4321.h>
#include <stdio.h>
#include <math.h>
#pragma config OSC = INTIO2
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOR =OFF

//Prototype Area
void Init_IO(void);
void Wait_Half_Second(void);
void Wait_One_Second(void);
void gen_short_beep (void);
void gen_long_beep (void);
void play_series_beep_tone(char);
void Do_Init(void);

void Init_IO()
{
    TRISA = 0x00;                       //Set Port A as output
    TRISB = 0x03;                       //Set PORT B, bit 0 and 1 as inputs and others as outputs
    TRISC = 0x00;                       //Set Port C as output
    TRISD = 0x00;                       //Set Port D as output
    TRISE = 0x00;                       //Set Port E as output
}

void Wait_Half_Second()
{
    for (int i=0; i < 8500; i ++);      //For loop until 500 msec is reached
}

void Wait_One_Second()
{
    Wait_Half_Second();                 //Wait for half second (or 500 msec)
    Wait_Half_Second();                 //Wait for half second (or 500 msec)
}

void gen_short_beep (void)
{
    PR2 = 0x52;                         //values for short beep
    CCPR1L = 0x29;
    CCP1CON = 0x1C;
    T2CON = 0x05;                       //Turn T2 on here
    Wait_Half_Second();                 //Wait half-second
    T2CON = 0x00;                       //Turn T2 off here
    Wait_Half_Second();
}

void gen_long_beep (void)
{
    PR2 = 0xA6;                         //values for long beep
    CCPR1L = 0x37;
    CCP1CON = 0x0C;
    T2CON = 0x05;                       //Turn T2 on here
    Wait_One_Second();                  //Wait half-second
    T2CON = 0x00;                       //Turn T2 off here
    Wait_One_Second();
}

void play_series_beep_tone(char sequence_number)
{
    if (sequence_number == 0)           //If sequence is 0, generate 2 short beeps
    {
        gen_short_beep();
        gen_short_beep();
    }
    else
    {
        for(int i = 1; i <= sequence_number; i++)
        {
            if ((i % 2) == 0)           //Otherwise, generate sound accordingly
                gen_long_beep();
            else
                gen_short_beep();
        }
    }
}

void Do_Init()
{
    OSCCON = 0x60;                      //4MHz clock speed
    Init_IO();
    ADCON0 = 0x01;                      //Change AN0 from analog to digital signal
    ADCON1 = 0x0E;                      //Change 0x0E from analog to digital signal
    ADCON2 = 0xA9;                      //Change 0xA9 from analog to digital signal
}

void main()
{
    Do_Init();
    while(1)
    {
        gen_long_beep();
        gen_short_beep();
        Wait_One_Second();
        play_series_beep_tone(4);
    }
}