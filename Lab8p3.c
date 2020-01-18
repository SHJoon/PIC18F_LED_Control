#include <p18f4321.h>
#include <stdio.h>
#include <math.h>
#pragma config OSC = INTIO2
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOR =OFF
#define TPulse PORTCbits.RC0
#define RED PORTCbits.RC3
#define GREEN PORTCbits.RC4
#define BLUE PORTCbits.RC5

//Prototype Area
void Init_IO(void);
void Wait_Half_Second(void);
void Wait_One_Second(void);
void gen_short_beep (void);
void gen_long_beep (void);
void play_series_beep_tone(char);
unsigned int Tint(float);
unsigned int GET_FULL_ADC(void);
void SPI_out(char);
void T0ISR(void);
void interrupt high_priority chk_isr(void); 
void Do_Init(void);
void init_UART(void);
void Serial_RX_ISR(void);
void do_print_menu(void);
void Do_Pattern(void);
void LED_Output(char, char);

//Variable Declaration
int TMR_flag;
float Vin;

void Init_IO()
{
    TRISA = 0x01;                           //Set Port A, bit 0 as input and others as output
    TRISB = 0x03;                           //Set PORT B, bit 0 and 1 as inputs and others as output
    TRISC = 0x00;                           //Set Port C as output
    TRISD = 0x00;                           //Set Port D as output
    TRISE = 0x00;                           //Set Port E as output
}

void Wait_Half_Second()
{
    for (int i=0; i < 8500; i ++);          //For loop until 500 msec is reached
}

void Wait_One_Second()
{
    Wait_Half_Second();                     //Wait for half second (or 500 msec)
    Wait_Half_Second();                     //Wait for half second (or 500 msec)
}

void gen_short_beep (void)
{
    PR2 = 0x52;                             //values for short beep
    CCPR1L = 0x29;
    CCP1CON = 0x1C;
    T2CON = 0x05;                           //Turn T2 on here
    Wait_Half_Second();                     //Wait half-second
    T2CON = 0x00;                           //Turn T2 off here
    Wait_Half_Second();
}

void gen_long_beep (void)
{
    PR2 = 0xA6;                             //values for long beep
    CCPR1L = 0x37;
    CCP1CON = 0x0C;
    T2CON = 0x05;                           //Turn T2 on here
    Wait_One_Second();                      //Wait half-second
    T2CON = 0x00;                           //Turn T2 off here
    Wait_One_Second();
}

void play_series_beep_tone(char sequence_number)
{
    if (sequence_number == 0)               //If sequence is 0, generate 2 short beeps
    {
        gen_short_beep();
        gen_short_beep();
    }
    else
    {
        for(int i = 1; i <= sequence_number; i++)
        {
            if ((i % 2) == 0)               //Otherwise, generate sound accordingly
                gen_long_beep();
            else
                gen_short_beep();
        }
    }
}

void Do_Init()
{
    OSCCON = 0x60;
    Init_IO();
    ADCON0 = 0x01;                          //Change AN0 from analog to digital signal
    ADCON1 = 0x0E;                          //Change 0x0E from analog to digital signal
    ADCON2 = 0xA9;                          //Change 0xA9 from analog to digital signal
    T0CON=0x02;                             //Timer0 off, increment on positive
                                            //edge, 1:8 prescaler
    TMR0H=0x0B;                             //Set Timer High with dummy values
    TMR0L=0xDB;                             //Set Timer Low with dummy values
    INTCONbits.TMR0IE=1;                    //Set interrupt enable
    INTCONbits.TMR0IF=0;                    //Clear interrupt flag
    INTCONbits.GIE=1;                       //Set the Global Interrupt Enable
    T0CONbits.TMR0ON=1;                     //Turn on Timer0
}

unsigned int GET_FULL_ADC(void)
{
    int result;
    ADCON0bits.GO=1;                        //Start Conversion
    while(ADCON0bits.DONE==1);              //wait for conversion to be completed
    result = (ADRESH * 0x100) + ADRESL;     //combine result of upper byte and
                                            //lower byte into result
    return result;                          //return the result.
}

unsigned int Tint (float Vin)
{
    int x;
    if (Vin >= 3)                           //Controls RGB LED for speed signal
    {
        x = 2;
        RED = 1;
        GREEN = 0;
        BLUE = 0;
    }
    else if(Vin >= 2)
    {
        x = 1;
        RED = 0;
        GREEN = 1;
        BLUE = 0;
    }
    else
    {
        x = 0;
        RED = 0;
        GREEN = 0;
        BLUE = 1;
    }
    return x;
}

void interrupt high_priority T0ISR()
{
    int T0L, T0H;
    if (Tint(Vin) == 0)                     //Low Voltage = 200ms
    {
        T0H = 0xF3;
        T0L = 0xD5;
    }
    else if (Tint(Vin) == 1)                //Medium Voltage = 100ms
    {
        T0H = 0xE7;
        T0L = 0x89;
    }
    else                                    //High Voltage = 50ms
    {
        T0H = 0xCF;
        T0L = 0x10;
    }
    INTCONbits.TMR0IF=0;                    //Clear the interrupt flag
    TMR0H=T0H;                              //Reload Timer High and
    TMR0L=T0L;                              //Timer Low
    TPulse = ~TPulse;                       //flip logic state of TPulse
    TMR_flag = 1;
}

void main()
{
    Do_Init();
    while(1)
    {
        Vin = GET_FULL_ADC() * 5.0 / 1000.0;
    }
}