#include <avr/io.h>

#include <stdio.h>

#include <avr/interrupt.h>

#include <util/delay.h>

#include "kmd128.h"

#define BYTE    unsigned char

#define WORD    unsigned int

volatile unsigned int pulse_count=0;

volatile unsigned char togle = 0;

unsigned char str;

char t[3];

 

void Lineposition(WORD i)

{

PORTB &= ~(_BV(0)); // command mode

PORTB |= (_BV(2)); // enable high

PORTC = i; // LCD ������ġ

Delay_ms(2);

PORTB &= ~(_BV(2)); // enable low


 


}

void LCD_in(WORD i)

{

PORTB |= (_BV(0)); // data mode

PORTB |= (_BV(2)); // enable high

PORTC = i; // ASCII ������� ��1��

Delay_ms(2);

PORTB &= ~(_BV(2)); // enable low


 


}

 

int get_value(void)

{

    ADMUX = 0x00;  //use AREF, adc channel 0

    ADCSRA = 0xe7; //adc enable, start, free running, prescaler 128

 

    while(ADCSRA&(1 << ADIF) == 0); // ��ȯ�Ϸ� �� ������ ���

 

    return ADC; // ADC�� ��ȯ

}

 

 

static FILE mystdout = FDEV_SETUP_STREAM(putch, NULL,_FDEV_SETUP_WRITE);

void main()

{

  //  MCU_initialize();                   // MCU Initialize   

  //  Delay_ms(50);                       

 //  LCD_initialize();                   // LCD Initialize     

 UART1_initialize();                 // UART0 Initialize

    stdout = &mystdout;      

   

  

  XMCRA = 0x00;

    MCUCR = 0x00; // dis-enable external memory and I/O

Delay_ms(2);                       


 


// LCD initialize 

DDRC = 0xff; // port direction as output

DDRB |= _BV(0);

DDRB |= _BV(1);

DDRB |= _BV(2);


 


PORTB &= ~(_BV(0)); // command mode

PORTB &= ~(_BV(1)); // write mode

PORTB &= ~(_BV(2)); // enable low

Delay_ms(2); // LCD ��� ���� �ð� ���� �ʿ���


 


    // LCD function set(8 bit, 16x2 line, 5x7 dot)

PORTB |= (_BV(2)); // enable high

PORTC = 0x3C; // ��� �Է�

Delay_ms(2); // ���� �ð�

PORTB &= ~(_BV(2)); // enable low: �����ͳ� ����� �Է��ϰ� E�� falling edge�� Ŭ���� ��


 


// LCD display control(display ON, cursor OFF, blink OFF)

PORTB |= (_BV(2)); // enable high

PORTC = 0x0C;

Delay_ms(2);

PORTB &= ~(_BV(2)); // enable low

Delay_ms(2);


 


// LCD entry mode set(increment, not shift)

PORTB |= (_BV(2)); // enable high

PORTC = 0x06;

Delay_ms(2);

PORTB &= ~(_BV(2)); // enable low

// Retrun Home

PORTB |= (_BV(2)); // enable high

PORTC = 0x02;

Delay_ms(2);

PORTB &= ~(_BV(2)); // enable low


 


// clear display

PORTB |= (_BV(2)); // enable high

PORTC = 0x01;

Delay_ms(2);

PORTB &= ~(_BV(2)); // enable low


 


   // LCD  ���� 

Lineposition(0x80);

    LCD_in(0x4E);

Lineposition(0x81);

    LCD_in(0x41);

Lineposition(0x82);

    LCD_in(0x4D);

Lineposition(0x83);

    LCD_in(0x45);

Lineposition(0x84);

    LCD_in(0x3A);


 


Lineposition(0x86);

    LCD_in(0x4B);

Lineposition(0x87);

    LCD_in(0x49);

Lineposition(0x88);

    LCD_in(0x4D);

Lineposition(0x8A);

LCD_in(0x53);

Lineposition(0x8C);

LCD_in(0x48);


 


Lineposition(0xc0);

    LCD_in(0x44);

Lineposition(0xc1);

    LCD_in(0x69);

Lineposition(0xc2);

    LCD_in(0x73);

Lineposition(0xc3);

    LCD_in(0x74);

Lineposition(0xc4);

    LCD_in(0x61);

Lineposition(0xc5);

    LCD_in(0x6E);

Lineposition(0xc6);

    LCD_in(0x63);

Lineposition(0xc7);

    LCD_in(0x65);

Lineposition(0xc8);

LCD_in(0x3A);


 


// write data (���� ����)

 

   DDRD = 0x20;

   DDRE = 0x00;

   Delay_ms(5);

   EIMSK = 0x10;

   EICRB = 0x03;

   ACSR=0x80;

   SREG |=_BV(7);

   

   DDRF = 0xff;

 

 

   DDRF &= ~(_BV(0)); // CDS 

 

   int res = 0;

 

   while(1)

   {

      pulse(); 

      Delay_ms(10);

 

 res = get_value();

 

 

   //   printf( "%d \r\n", res ); 

 

 

 if (pulse_count < 5)

 {

     PORTF |= _BV(2);   

     Delay_ms(100);

     PORTF &= ~(_BV(2));

     Delay_ms(50);

 

     PORTF |= _BV(3);   

     Delay_ms(100);

     PORTF &= ~(_BV(3));

     Delay_ms(50);


 }


 

 

if (res < 500)

 {

 PORTA |= _BV(4);   

 }

else {

PORTA &= ~(_BV(4));

}


 


int N1000;

int N100;

int N10;

int N1;


 


N1000 = (int)pulse_count/1000;      // ������ ���� �� �ڸ��� 

    N100  = (int)(pulse_count/100)%10;

    N10  = (int)(pulse_count/10)%10;

N1  = (int)pulse_count%10;

    


 


                                          // LCD �Ÿ� 

Lineposition(0xcA);

itoa(N1000,&t[0],10);

LCD_in(t[0]);

Lineposition(0xcB);

itoa(N100,&t[1],10);

LCD_in(t[1]);

Lineposition(0xcC);

itoa(N10,&t[2],10);

LCD_in(t[2]);

Lineposition(0xcD);

itoa(N1,&t[3],10);

LCD_in(t[3]);

    Delay_ms(100);

   }

}

void pulse(void)   //10usec High �޽� ��µ˴ϴ�.

{

   PORTD |= _BV(0);   //input pulse -> PB5

   Delay_us(1000000000000000000000000);

   PORTD &= ~(_BV(0));

}

 

 

 

ISR(TIMER0_OVF_vect)

{

   pulse_count++;   //58sec ���� �����մϴ�.

}

 

 

 

 

ISR(INT4_vect)

{

        if(togle == 0) // ī���� �����ϰ� �ܺ� ���ͷ�Ʈ(INT4)�� �������� Falling Edge���� �ɸ����� ��.

        {

                pulse_count = 0; // ������ ���� �Ÿ��� �ʱ�ȭ

                //******************** TIMER COUNTER RESISTER ********************

                TIMSK = 0x01; // Timer Counter 0 Enable 

                //OCE2, TOIE2, TICIE1, OCIE1A, OCIE1B, TOEI1, OCEI0, TOIE0

                TCCR0 = 0x02; // 8ck

                TCNT0 = 140;    //58us/per count

                EICRB = 0x02; // EXT_int4 falling edge interrupt

                togle = 1;

        }

        else // ī���͸� ������Ű�� �ܺ� ���ͷ�Ʈ(INT4)�� �������� Riging Edge���� �ɸ����� ��.

        {

                TIMSK = 0x00; // All Timer Disable

                togle = 0;

    printf("%d\n\r",pulse_count);

                EICRB = 0x03; //INt4 high_edge interrupt

        }                          

            

}

