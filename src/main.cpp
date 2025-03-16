//---------------------------------------------------------
/*
NHD-0440AZ.c
Program for writing to Newhaven Display 4x40 Character AZ-Series LCDs

(c) Newhaven Display International, Inc. 

 	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
*/
//---------------------------------------------------------

#include <Wire.h>

#include <Arduino.h>
//---------------------------------------------------------

uint8_t E1	 (1 << 3);
uint8_t E2   (1 << 2);
uint8_t D_I	 (1 << 4);
// #define R_W	 P3_7;
// #define DB7..DB0	P1;

// volatile xdata at 0xB4 char E1;
// volatile xdata at 0xB1 char E2;
// volatile xdata at 0xB0 char D_I;
// volatile xdata at 0xB7 char R_W;

void setDatabus(char i) {
  PORTD = (i << 2) | (PORTD & 0x03);	//preserve the state of the 2 least significant bits
  PORTB = (i >> 6) | (PORTB & 0xFC);
  Serial.print(PORTB, BIN);
  Serial.print(" ");
  Serial.print(PORTD, BIN);
  Serial.println();
}

void Delayms(int n){
	int i;
	int j;
	for (i=0;i<n;i++)
		for (j=0;j<350;j++)
		{;}
}

void command1(char i){
  setDatabus(i);	//write data on lines 3&4
	PORTB &= ~D_I;					//Instruction register
	// R_W =0;					//write
	PORTB |=E1;				//pulse enable pin
  Delayms(2);				//wait 160us, or you can poll the busy flag from now on

	PORTB &= ~E1;				//reset enable pin
}
void command2(char i){		//command for lines 3&4
  setDatabus(i);	//write data on lines 3&4
	PORTB &= ~D_I;					//Instruction register
	// R_W =0;
	PORTB |=E2;				//pulse enable pin
  Delayms(2);				//wait 160us, or you can poll the busy flag from now on

	PORTB &= ~E2;				//reset enable pin
}
void write1(char i){		//write data on lines 1&2
  setDatabus(i);	//write data on lines 1&2
	PORTB |=D_I;					//Data register
	// R_W =0;					//write
	PORTB |=E1;				//pulse enable pin
  Delayms(2);				//wait 160us, or you can poll the busy flag from now on

	PORTB &= ~E1;				//reset enable pin
}
void write2(char i){	
  setDatabus(i);	//write data on lines 3&4
	PORTB |=D_I;					//Data register
	// R_W =0;
	PORTB |=E2;				//pulse enable pin
  Delayms(2);				//wait 160us, or you can poll the busy flag from now on

	PORTB &= ~E2;				//reset enable pin
}
void init(){
	Delayms(15);			//wait 15ms after power up
	command1(0x30);			//wake up controller 1
	Delayms(5);				//wait 5ms
	command2(0x30);			//wake up controller 2
	Delayms(5);				//wait 5ms
	command1(0x30);			//wake up again
	Delayms(1);				//wait at least 160us
	command2(0x30);
	Delayms(1);
	command1(0x30);			//wake up 3rd time
	Delayms(1);				//wait 160us, or you can poll the busy flag from now on
	command2(0x30);
	Delayms(1);
	command1(0x38);			//set interface length
	command2(0x38);
	command1(0x08);			//turn display off
	command2(0x08);
	command1(0x10);			//set cursor/display shift
	command2(0x10);
	command1(0x06);			//set cursor increment
	command2(0x06);
	command1(0x01);			//clear display
	command2(0x01);
	command1(0x0F);			//turn display on
	command2(0x0F);
}
void nextline1(){
	command1(0xc0);			//set DDRAM address to 40 (line 2)
}
void nextline2(){
	command2(0xc0);			//set DDRAM address to 40 (line 4)
}
void display(char *show){	//show character data on display
	char datum;
	int i;
	for (i=0;i<40;i++){
		datum=*show;			//point to 1st line data
		write1(datum);			//write to 1st line
		++show;					//next letter
	}
	nextline1();				//move address to line 2
	for (i=0;i<40;i++){
		datum=*show;
		write1(datum);			//write to 2nd line
		++show;
	}	 
	for (i=0;i<40;i++){
		datum=*show;
		write2(datum);			//write to 3rd line
		++show;
	}
	nextline2();				//move address to line 4
	for (i=0;i<40;i++){
		datum=*show;
		write2(datum);			//write to 4th line
		++show;
	}
}

void setup() {
  
  Serial.begin(9600);
  DDRD |= 0xFC;				//set port 0 as output
  DDRB |= 0x1F;				//set port 1 as output
  setDatabus(0x00);
  PORTB &= ~E1;				//reset enable pin
  PORTB &= ~E2;				//reset enable pin
  PORTB &= ~D_I;				//Instruction register
	// R_W=0;
	init();						//initialize display

  
}

void loop() {
  display((char *)"Newhaven Display");			//show the data in array "test"
  Delayms(1000);

}