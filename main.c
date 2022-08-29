#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz clock speed
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "ALCD.H"
#define BUTTON_MASK (1<<PB0)
#define BUTTON_MASK1 (1<<PB4)//Speed inc button
#define BUTTON_MASK2 (1<<PB5)//speed Dec button
#define BUTTON_MASK3 (1<<PC1)// CW button
#define BUTTON_MASK4 (1<<PC2)//CC button
#define BUTTON_PIN PINB
#define BUTTON_PORT PORTB
#define BUTTON_PIN2 PINC
#define BUTTON_PORT2 PORTC
#define PWM1H OCR1AH
#define PWM1L OCR1AL
#define PWM2H OCR1BH
#define PWM2L OCR1BL
volatile uint8_t button_down;
volatile uint8_t button_down1;
volatile uint8_t button_down2;
volatile uint8_t button_down3;
volatile uint8_t button_down4;
static inline void debounce1(void){//speed inc
	static uint8_t count = 0;
	static uint8_t Last_button_state = 0;
	uint8_t current_state = (~BUTTON_PIN & BUTTON_MASK1) != 0;
	if (current_state != Last_button_state)
	{         count=count+1;
		if (count >= 4) {
			
			Last_button_state = current_state;
			if (current_state != 0) {
				button_down1 = 1;
			}
			count = 0;
		}
	}
	else {
	count = 0;      }
};
static inline void debounce2(void){
		static uint8_t count = 0;
		static uint8_t Last_button_state = 0;
		uint8_t current_state = (~BUTTON_PIN & BUTTON_MASK2) != 0;
		if (current_state != Last_button_state)
		{         count=count+1;
			if (count >= 4) {
				
				Last_button_state = current_state;
				if (current_state != 0) {
					button_down2 = 1;
				}
				count = 0;
			}
		}
		else {
		count = 0;      }
	
};
static inline void debounce3(void){
		static uint8_t count = 0;
		static uint8_t Last_button_state = 0;
		uint8_t current_state = (~BUTTON_PIN2 & BUTTON_MASK3) != 0;
		if (current_state != Last_button_state)
		{         count=count+1;
			if (count >= 4) {
				
				Last_button_state = current_state;
				if (current_state != 0) {
					button_down3 = 1;
				}
				count = 0;
			}
		}
		else {
		count = 0;      }
	
};
static inline void debounce4(void){
		static uint8_t count = 0;
		static uint8_t Last_button_state = 0;
		uint8_t current_state = (~BUTTON_PIN2 & BUTTON_MASK4) != 0;
		if (current_state != Last_button_state)
		{         count=count+1;
			if (count >= 4) {
				
				Last_button_state = current_state;
				if (current_state != 0) {
					button_down4 = 1;
				}
				count = 0;
			}
		}
		else {
		count = 0;      }
};


static inline void debounce(void)
{
	static uint8_t count = 0;
	static uint8_t Last_button_state = 0;
	uint8_t current_state = (~BUTTON_PIN & BUTTON_MASK) != 0;
	if (current_state != Last_button_state)
	{         count=count+1;
		if (count >= 4) {
			
			Last_button_state = current_state;
			if (current_state != 0) {
				button_down = 1;
			}
			count = 0;
		}
	}
	else {
	count = 0;      }
};
//static inline void debounce2(int Row){
	//PORTC |=(1<<Row);
	//static uint8_t count = 0;
	//static uint8_t Last_button_state = 0;
	//uint8_t current_state = (~PINC & 1<<Row) != 0;
	//if (current_state != Last_button_state)
	//{         count=count+1;
		//if (count >= 4) {
			//
			//Last_button_state = current_state;
			//if (current_state != 0) {
				//button_down2 = 1;
			//}
			//count = 0;
		//}
	//}
	//else {
	//count = 0;}
//};

//Functions
//char read_key(); //read key pressed from keypad
int get_number(char number);//convert char to number
void ADC_setup();
float ADC_read();



int main(void)
{
	 //char Key_pressed='?';
     lcd_init(16);
	 ADC_setup();
	 BUTTON_PORT |= BUTTON_MASK;
	 //manual mode buttons
	 BUTTON_PORT |= BUTTON_MASK1;
	 BUTTON_PORT |= BUTTON_MASK2;
	 BUTTON_PORT2 |= BUTTON_MASK3;
	 BUTTON_PORT2 |= BUTTON_MASK4;
	 DDRC &=~ (1<<0);
	 float Data;
	 float DigitalV;
	 float DigitalV_M=20.00;
	 char*Data_ch=malloc(31*2);
	 int mode=0;
	 int clear_LCD=1;
	 int clear_LCD2=1;
	 DDRB |= (1 << 1)|(1 << 2);
	 DDRB|= (1<<3);
	 int CW_CC=1;//decides direction in manual mode
	
    while (1) 
    {
		debounce();
		if (button_down!=0)
		{
			lcd_clear();
			button_down = 0;
			clear_LCD=1;
			if(mode==0){
				mode=1;
			}
			else{
				mode=0;
			}
		}
		//the 2 modes of operations
		if (mode==0)//auto mode
		{
			
			//clears screen only at first switch
			if (clear_LCD==1)
			{
				lcd_clear();
				clear_LCD=0;
			}
			if (clear_LCD2==1)
			{
				lcd_gotoxy(0,0);
				lcd_puts("Auto mode");
			}


			//TCNT1=0X00;
			//ICR1=0X00;
			//OCR1A=0X00;
			//OCR1B=0X00;
			//TIMSK1=0X00;
			 TCCR1A |= (1 << COM1A1)|(1 << COM1B1);// set none-inverting mode
			 TCCR1A |= (1 << WGM11);
			 TCCR1B |= (1 << WGM12)|(1 << WGM13);// set Fast PWM mode using ICR1 as TOP
			 TCCR1B |= (1 << CS10);// START the timer with no prescaler
			 ICR1 = 0xFFFF; // set TOP to 16bit
			 
			
			//voltage reading from pot
			Data=ADC_read();
			DigitalV = (((5*Data)/1024)/5)*100;
			dtostrf(DigitalV,5,2,Data_ch);
			lcd_gotoxy(0,1);
			lcd_puts(Data_ch);
			lcd_putchar('%');
			////SETUP
			//TCCR0A|= (1<<COM0A1)|(1<<WGM00)|(1<<WGM01); 
			//TCCR0B=0X01;//Fast PWM Mode
			//TIMSK0|= (1<<TOIE0);
			//rest of auto code here (use read voltage to drive motor)
			if (DigitalV>50){//clockwise code (speed is increasing 2.5 to 5)
				PORTB|= (1<<3);//Enable the motor
				lcd_gotoxy(6,1);
				lcd_puts("CW");
				//clockwise code
				OCR1A =DigitalV*(655.35);//2^16 is 65535 so max value is FFFF in hexa to get that number we multiply by 655.35 and give that to PWM
				OCR1B=0x0000;
				//lcd_init(16);
				clear_LCD2=1;
				
			}
			else if(DigitalV<50 && DigitalV>0){
				//counter clock wise code(speed is decreasing 2.5 to 0)
				PORTB|= (1<<3);//Enable the motor
				lcd_gotoxy(6,1);
				lcd_puts("CC");

				OCR1A =0x0000;
				OCR1B=(50+DigitalV)*(655.35);
				clear_LCD2=1;
				
			}
			else {
				if (clear_LCD2==1){
					lcd_clear();
					clear_LCD2=0;
				}
				
				lcd_gotoxy(0,0);
				lcd_puts("stopped");
				PORTB&=~(1<<3);
				
			}

			
		}
		
		else{//manual mode operation
			//clears screen only at first switch
			if (clear_LCD==1){
				lcd_clear();
				clear_LCD=0;
			}
			//setup PWM
			 TCCR1A |= (1 << COM1A1)|(1 << COM1B1);// set none-inverting mode
			 TCCR1A |= (1 << WGM11);
			 TCCR1B |= (1 << WGM12)|(1 << WGM13);// set Fast PWM mode using ICR1 as TOP
			 TCCR1B |= (1 << CS10);// START the timer with no prescaler
			 ICR1 = 0xFFFF; // set TOP to 16bit
			lcd_gotoxy(0,0);
			lcd_puts("Manual mode");
			dtostrf(DigitalV_M,5,2,Data_ch);
			lcd_gotoxy(0,1);
			lcd_puts(Data_ch);
			lcd_putchar('%');
			debounce1();
			debounce2();
			debounce3();
			debounce4();
			if (button_down1!=0){//check if speed inc
				button_down1=0;
				DigitalV_M+=5;
			}
			
			if (button_down2!=0){//check if speed dec
				button_down2=0;
				DigitalV_M-=5;
			}
			
			if (button_down3!=0){//check if CW
				button_down3=0;
				CW_CC=1;
			}
			
			if (button_down4!=0){//check of CC
				button_down4=0;
				CW_CC=0;
			}
			if (DigitalV_M>100)
			{
				DigitalV_M=100;
			}
			else if (DigitalV_M<0)
			{
				DigitalV_M=0;
			}
			if (CW_CC==1 &&DigitalV_M!=0)//CW direction
			{
				PORTB|= (1<<3);//Enable the motor
				OCR1A =DigitalV_M*(655.35);
				OCR1B=0x0000;
				lcd_gotoxy(6,1);
				lcd_puts("CW");
			}
			else if(CW_CC==0 &&DigitalV_M!=0) {
				PORTB|= (1<<3);//Enable the motor
				OCR1A =0x0000;
				OCR1B=DigitalV_M*(655.35);
				lcd_gotoxy(6,1);
				lcd_puts("CC");
			}
			else if(DigitalV_M==0)
			{
				PORTB&=~(1<<3);
			}
			
			//rest of manual code here
			
		}
    }
}



//char read_key(){
	//DDRB &=~ 0xff;//column pins(changing every pin one at a time to output)
	//DDRC &=~ 0xff;//row pins (always will be input)
	//PORTC |= 0xff;//pull-up resistors for row pins only
	//for (int i=0;i<4;i++)
	//{
		//DDRB|= (1<<i); //setting the PIN i  only to be output
		//PORTB &=~ (1<<i);// sending a 0 Volt from pin i
		//for (int j=0;j<4;j++)//scanning each row to see if a button is pressed
		//{
			//if ((PINC&1<<j)==0)
			//{
				//debounce2(j);
				//while(button_down2==0)
				//{
					//debounce2(j);
				//}
				//return key [j][i]; //return the pressed key from the 2D matrix
			//}
		//}
		//DDRB &=~ 0xff; // return them all to input again before checking another column
	//}
	//return '?'; //in case nothing gets pressed return '?' (will print nothing if this function returns ?)
//}
int get_number(char number ){
	switch(number)
	{
		case '0' :return 0;break;
		case '1' :return 1;break;
		case '2' :return 2;break;
		case '3' :return 3;break;
		case '4' :return 4;break;
		case '5' :return 5;break;
		case '6' :return 6;break;
		case '7' :return 7;break;
		case '8' :return 8;break;
		case '9' :return 9;break;
	}
	return 0;
}
void ADC_setup(){
	ADMUX |= (1<<REFS0);
	ADCSRA |= (1<<ADEN)  |(1<<ADPS0) |(1<<ADPS1) |(1<<ADPS2) |(1<<ADSC); //enabling the ADC chip, dividing clk by 128, and starting conversion
}
float ADC_read(){
	ADCSRA |= (1<<ADSC);//start conversion
	ADMUX &=~ (1<<MUX0)	 |(1<<MUX1) |(1<<MUX2) |(1<<MUX3) ;//choosing which potentiometer to read from
	while (ADCSRA & (1<<ADSC));// wait until conversion end
	return ADC;

}
