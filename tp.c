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

PORTC = i; // LCD 글자위치

Delay_ms(2);

PORTB &= ~(_BV(2)); // enable low


 


}

void LCD_in(WORD i)

{

PORTB |= (_BV(0)); // data mode

PORTB |= (_BV(2)); // enable high

PORTC = i; // ASCII 문자출력 “1”

Delay_ms(2);

PORTB &= ~(_BV(2)); // enable low


 


}

 

int get_value(void)

{

    ADMUX = 0x00;  //use AREF, adc channel 0

    ADCSRA = 0xe7; //adc enable, start, free running, prescaler 128

 

    while(ADCSRA&(1 << ADIF) == 0); // 변환완료 될 때까지 대기

 

    return ADC; // ADC값 반환

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

Delay_ms(2); // LCD 제어를 위해 시간 지연 필요함


 


    // LCD function set(8 bit, 16x2 line, 5x7 dot)

PORTB |= (_BV(2)); // enable high

PORTC = 0x3C; // 명령 입력

Delay_ms(2); // 여유 시간

PORTB &= ~(_BV(2)); // enable low: 데이터나 명령을 입력하고 E를 falling edge로 클럭을 줌


 


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


 


   // LCD  글자 

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


 


// write data (글자 쓰기)

 

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


 


N1000 = (int)pulse_count/1000;      // 정수부 추출 각 자리수 

    N100  = (int)(pulse_count/100)%10;

    N10  = (int)(pulse_count/10)%10;

N1  = (int)pulse_count%10;

    


 


                                          // LCD 거리 

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

void pulse(void)   //10usec High 펄스 출력됩니다.

{

   PORTD |= _BV(0);   //input pulse -> PB5

   Delay_us(1000000000000000000000000);

   PORTD &= ~(_BV(0));

}

 

 

 

ISR(TIMER0_OVF_vect)

{

   pulse_count++;   //58sec 마다 증가합니다.

}

 

 

 

 

ISR(INT4_vect)

{

        if(togle == 0) // 카운터 시작하고 외부 인터럽트(INT4)를 다음에는 Falling Edge에거 걸리도록 함.

        {

                pulse_count = 0; // 측정된 이전 거리값 초기화

                //******************** TIMER COUNTER RESISTER ********************

                TIMSK = 0x01; // Timer Counter 0 Enable 

                //OCE2, TOIE2, TICIE1, OCIE1A, OCIE1B, TOEI1, OCEI0, TOIE0

                TCCR0 = 0x02; // 8ck

                TCNT0 = 140;    //58us/per count

                EICRB = 0x02; // EXT_int4 falling edge interrupt

                togle = 1;

        }

        else // 카운터를 정지시키고 외부 인터럽트(INT4)를 다음에는 Riging Edge에서 걸리도록 함.

        {

                TIMSK = 0x00; // All Timer Disable

                togle = 0;

    printf("%d\n\r",pulse_count);

                EICRB = 0x03; //INt4 high_edge interrupt

        }                          

            

}

