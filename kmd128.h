/* -------------------------------------------- */
/* KMD Mega128 Education Board ->>> kmd128.h */
/* -------------------------------------------- */

#define BYTE unsigned char
#define WORD unsigned int

#define LCD_COMMAND *((BYTE *)0x8000) // LCD COMMAND 
#define LCD_DATA *((BYTE *)0x8001) // LCD data
#define FND_COMMAND *((BYTE *)0x9000) // FND data
#define DAC_COMMAND *((BYTE *)0xA000) // DA Converter data
#define CS0 *((BYTE *)0xB000) // CS0 data
#define CS1 *((BYTE *)0xB800) // CS1 data
#define CS2 *((BYTE *)0xC000) // CS2 data
#define CS3 *((BYTE *)0xC800) // CS3 data
#define CS4 *((BYTE *)0xD000) // CS4 data

#define FND_DIGIT PORTB

#define OFF 1
#define ON 0

void MCU_initialize(void) /* initialize ATmege128 MCU */
{ 
MCUCR = 0x80; // enable external memory and I/O
XMCRA = 0x00; // 0x1100-0xFFFF=1 wait 
XMCRB = 0x80; // enable bus keeper, use PC0-PC7 as address
SFIOR = 0x00;
DDRB = 0x0F; // Key input, PB4~PB7
PORTB = 0xFF; // FND Digit Selet Bit -> PB0~PB3

DDRD = 0xFF;
PORTD = 0xFF;

DDRE = 0xFE; // Key output, PE4~PE7
PORTE = 0xFE;

DDRE |= _BV(2); // Run Led 
DDRF &= ~(_BV(0)); // VRE (AD0) 
DDRF &= ~(_BV(1)); // LM35DZ (AD1) 
DDRF &= ~(_BV(2)); // CdS (AD2) 
DDRF &= ~(_BV(3)); // External Input (AD3) 
DDRG |= _BV(3); // Buzzer 
}

void Delay_us(BYTE time_us) /* time delay for us */
{ 
BYTE i;

for(i = 0; i < time_us; i++) // 4 cycle +
{ 
asm(" PUSH R0 "); // 2 cycle +
asm(" POP R0 "); // 2 cycle +
asm(" PUSH R0 "); // 2 cycle +
asm(" POP R0 "); // 2 cycle +
asm(" PUSH R0 "); // 2 cycle +
asm(" POP R0 "); // 2 cycle = 16 cycle = 1 us for 16MHz
}
}

void Delay_ms(WORD time_ms) /* time delay for ms */
{ 
WORD i;

for(i = 0; i < time_ms; i++)
{ 
Delay_us(250);
Delay_us(250);
Delay_us(250);
Delay_us(250);
}
}

void Beep(void) /* beep for 50 ms */
{ 
PORTG |= _BV(3); // buzzer on 
Delay_ms(50); // delay 50 ms 
PORTG &= ~(_BV(3)); // buzzer off 
}

void RUN_LED(void)
{
PORTE &= ~(_BV(2)); 
Delay_ms(50); 
PORTE |= _BV(2);
}

void LCD_command(BYTE command) /* write a command(instruction) to text LCD */
{
LCD_COMMAND = command; // output command
Delay_us(50);
}

void LCD_data(BYTE data) /* display a character on text LCD */
{
LCD_DATA = data; // output data
Delay_us(50);
}

void LCD_string(BYTE command, BYTE *string) /* display a string on LCD */
{
LCD_command(command); // start position of string

while(*string != '\0') // display string
{ 
LCD_data(*string);
string++;
}
}

void LCD_initialize(void) /* initialize text LCD module */
{
Delay_ms(2);

LCD_command(0x38); // LCD function set(8 bit, 16x2 line, 5x7 dot)
Delay_ms(2);
LCD_command(0x38); // LCD function set(8 bit, 16x2 line, 5x7 dot)
Delay_ms(2);
LCD_command(0x38); // LCD function set(8 bit, 16x2 line, 5x7 dot)
Delay_ms(2);
LCD_command(0x0C); // LCD display control(display ON, cursor OFF, blink OFF)
Delay_ms(2);
LCD_command(0x06); // LCD entry mode set(increment, not shift)
Delay_ms(2);
LCD_command(0x02); // Retrun Home
Delay_ms(2);
LCD_command(0x01); // clear display
Delay_ms(2);
LCD_command(0x01); // clear display
Delay_ms(2);
}

void UART1_initialize(void)
{
DDRD = 0xFB; 
UCSR1A = 0x00; // Asynchronous Normal Mode
UCSR1B = 0x18; // Receive enable, Transmitte enable
UCSR1C = 0x06; // 비동기 방식, No parity bit, 1 stop bit 
UBRR1H = 0x00;
UBRR1L = 8; // 16MHz에서 115,200 bps
}

char getch(void)
{
BYTE data;

loop_until_bit_is_set(UCSR0A,7);
data = UDR0;
UCSR0A |= 0x80; //Clear RXC0
return(data);
}

void putch(BYTE data)
{
loop_until_bit_is_set(UCSR1A,5);
UDR1 = data;
UCSR1A |= 0x20; //Clear UDRE0
}

BYTE key_flag = 0;
BYTE Key_scan(void) /* Input Key Scan 0~15 */
{ 
BYTE i, key, row, column;
BYTE row_scan[4] = {0xE0, 0xD0, 0xB0, 0x70};


PORTE = 0x00; // Any key pressed? 
Delay_us(1);
column = PINB & 0xF0;

if(column == 0xF0) // If not, key_flag = 0? 
{ 
if(key_flag != 0)
{ 
Delay_ms(2); 
key_flag = 0; 
}
return 0x00; 
}

else if(key_flag != 0) // If key_flag != 0, continous key 
return 0x00;

else
{ 
Beep(); 
key_flag = 1; 

for(i=0; i<4; i++) // Scan 
{ 
row = row_scan[i];
PORTE = row;
Delay_us(1);
column = PINB & 0xF0;

if(column != 0xF0)
break;
}
if(i == 4)
return 0x00;

key = row + ((column >> 4) & 0x0F);
switch(key) 
{
case 0x7E : key = '0'; break;
case 0x7D : key = '1'; break;
case 0x7B : key = '2'; break;
case 0x77 : key = '3'; break;
case 0xBE : key = '4'; break;
case 0xBD : key = '5'; break;
case 0xBB : key = '6'; break;
case 0xB7 : key = '7'; break;
case 0xDE : key = '8'; break;
case 0xDD : key = '9'; break;
case 0xDB : key = 'a'; break;
case 0xD7 : key = 'b'; break;
case 0xEE : key = 'c'; break;
case 0xED : key = 'd'; break;
case 0xEB : key = 'e'; break;
case 0xE7 : key = 'f'; break;
default: key = 0x00; break;
}
return key;
}
}


#pragma memory = dataseg(EXT_SRAM_32KB)

#pragma memory = default 

#pragma memory = dataseg(EXT_LCD_COMMAND)

#pragma memory = default 

#pragma memory = dataseg(EXT_LCD_DATA)

#pragma memory = default

#pragma memory = dataseg(EXT_FND_COMMAND)

#pragma memory = default

#pragma memory = dataseg(EXT_DAC_COMMAND)

#pragma memory = default 
