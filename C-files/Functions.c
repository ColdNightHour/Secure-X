#include <avr/io.h>
#include <stdio.h>
#include "Functions.h"
void Custom_Char() {
	const unsigned char W1[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0,0x0};
	const unsigned char W2[8] = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4,0x0};
	const unsigned char W3[8] = {0x1f,0x11,0xa,0x4,0xa,0x1f,0x1f,0x0};
	const unsigned char W4[8] = {0x0,0xe,0x15,0x17,0x11,0xe,0x0,0x0};
	const unsigned char CLK1[8] = {0x8,0x14,0xa,0xa,0xb,0x1,0x9,0x1f};
	const unsigned char CLK2[8] = {0x0,0x0,0x1,0x1,0x3,0x2,0x2,0x3};
	const unsigned char CLK3[8] = {0x1,0x3,0xf,0xf,0xf,0x3,0x1,0x0};
	const unsigned char CLK4[8] = {0x8,0x10,0x0,0x18,0x0,0x10,0x8,0x0};
	unsigned char i;		
	LCD_WriteCommand(0x40);
	
	for(i = 0; i < 8; ++i) {
		LCD_WriteData(W1[i]);
	}
	for(i = 0; i < 8; ++i) {
		LCD_WriteData(W2[i]);
	}
	for(i = 0; i < 8; ++i) {
		LCD_WriteData(W3[i]);
	}
	
	for(i = 0; i < 8; ++i) {
		LCD_WriteData(W4[i]);
	}
	
	for(i = 0; i < 8; ++i) {
		LCD_WriteData(CLK1[i]);
	}
	
	for(i = 0; i < 8; ++i) {
		LCD_WriteData(CLK2[i]);
	}
	
	for(i = 0; i < 8; ++i) {
		LCD_WriteData(CLK3[i]);
	}
	
	for(i = 0; i < 8; ++i) {
		LCD_WriteData(CLK4[i]);
	}
	LCD_WriteCommand(0x80);
}
char* Sec2Read(long x) {
	long hr, min, sec;
	hr = x / 3600;
	min = (x % 3600) / 60;
	sec = ((x % 3600) % 60) / 1;
	char h1, h2, m1, m2, s1, s2;	
	h1 = (hr /10) + '0';
	h2 = (hr % 10) + '0';
	m1 = (min / 10) + '0';
	m2 = (min % 10) + '0';
	s1 = (sec / 10) + '0';
	s2 = (sec % 10) + '0';
	char *time_z = "  00h:00m:00s";
	time_z[2] = h1;
	time_z[3] = h2;
	time_z[6] = m1;
	time_z[7] = m2;
	time_z[10] = s1;
	time_z[11] = s2;
	return time_z;
}
