/*
 *	sagem_affa2.cpp
 *
 *  Created on: 13 lis 2018
 *      Author: djraszit, Mako
 */

#include "I2cbase.h"
#include "sagem_affa2.h"
#include <util/delay.h>
#include <string.h>
#include "constDef.h"
#include "usart.h"

//preambules
//                           size              icons       icons chan  addr
uint8_t lcd_preambule1[8] = {0x0f, 0x90, 0x7f, 0xff, 0x00, 0xff, 0x60, 0x01};

//                           size              chan  addr
uint8_t lcd_preambule2[5] = {0x0c, 0x90, 0x7e, 0x40, 0x01};

//                           size        mask  icons       icons
uint8_t lcd_preambule3[6] = {0x05, 0x90, 0x71, 0xff, 0xff, 0xff};

void sagem_affa2_set_icon(uint16_t icon){
	lcd_preambule3[3] &= ~(icon & 0xff);
	lcd_preambule3[5] &= ~(icon >> 8);
	write_sagem(lcd_preambule3);
	uint8_t data[2];
	data[0] = 0x01;
	data[1] = 0x11;
	write_sagem(data);
}

void sagem_affa2_clr_icon(uint16_t icon){
	lcd_preambule3[3] |= (icon & 0xff);
	lcd_preambule3[5] |= (icon >> 8);
	write_sagem(lcd_preambule3);
	uint8_t data[2];
	data[0] = 0x01;
	data[1] = 0x11;
	write_sagem(data);
}

void sagem_affa2_channel(uint8_t ch){
	lcd_preambule1[6] = ch;
	lcd_preambule2[3] = ch;
	write_sagem(lcd_preambule1);
	write_sagem(lcd_preambule2);
}

#define DELAY	_delay_us(100)

void sagem_affa2_init(){
	LCD_MIRQ_INPUT;
	LCD_MIRQ_SET;
	DELAY;
	uint8_t data_to_send[2] = { 0x00, 0x00 };
	uint8_t data_read[6];
	write_sagem(data_to_send);
	DELAY;
	write_sagem(data_to_send);
	DELAY;
	do {
		data_to_send[0] = 0x01;
		data_to_send[1] = 0x10;
		write_sagem(data_to_send);
		DELAY;
		data_to_send[0] = 0x01;
		data_to_send[1] = 0x11;
		write_sagem(data_to_send);
		DELAY;
		read_sagem(data_read);
		DELAY;
	} while (data_read[1] == 0x00);
}

void read_sagem(uint8_t *buf){
	uint8_t i;
	start:
	LCD_MIRQ_OUTPUT;
	LCD_MIRQ_SET;
	DELAY;
	LCD_MIRQ_CLR;
	DELAY;
	I2C_Start();
	I2C_SendAddr(0x47);
	if (TW_STATUS == TW_MR_SLA_NACK) {
		LCD_MIRQ_INPUT;
		LCD_MIRQ_SET;
		DELAY;
		goto start;
	}
	buf[0] = I2C_ReceiveData_ACK();
	for (i = 0; i < buf[0]; i++) {
		buf[i + 1] = I2C_ReceiveData_ACK();
	}
	I2C_Stop();
	DELAY;
	LCD_MIRQ_INPUT;
	LCD_MIRQ_SET;
	_delay_ms(7);
}

void write_sagem(uint8_t *buf){
	uint8_t i = 0;
	start:
	LCD_MIRQ_OUTPUT;
	LCD_MIRQ_SET;
	DELAY;
	LCD_MIRQ_CLR;
	DELAY;
	I2C_Start();
	I2C_SendAddr(0x46);
	if(TW_STATUS == TW_MT_SLA_NACK){
		LCD_MIRQ_INPUT;
		LCD_MIRQ_SET;
		DELAY;
		goto start;
	}
	for(i = 0; i < buf[0] + 1; i++){
		I2C_SendByte(buf[i]);
	}
	I2C_Stop();

	DELAY;
	LCD_MIRQ_INPUT;
	LCD_MIRQ_SET;
	_delay_ms(7);
}

uint8_t make_address(uint8_t ile, uint8_t ktora, uint8_t typ){
	return ((ile - 1) << 5) | (ktora << 2) | (typ & 0x03);
}

void write_text_sagem(const char *text, uint8_t scroll_type){
	uint8_t buf[17];
	uint8_t i = 0;
	uint8_t how_many_lines = 1;
	uint8_t current_line = 0;
	uint8_t s = strlen(text);
	if(scroll_type == NO_SCROLL){
		how_many_lines = 1;
	}
	while(i++ < s){
		if(i % 8 == 0 && i > 0){
			how_many_lines++;
		}
	}
	if(s <= 8){
		how_many_lines = 1;
		scroll_type = NO_SCROLL;
	}
	while(current_line < how_many_lines){
		memcpy(&buf, &lcd_preambule2, lcd_preambule2[0] - 7);
		uint8_t preamb_size = buf[0] - 7;

		buf[4] = make_address(how_many_lines, current_line, scroll_type);
		i = 0;
		while(i < 8){
			buf[preamb_size + i] = *text++;
			i++;
			if(*text == 0){
				break;
			}
		}
		while(i < 8){
			buf[preamb_size + i] = 0x20;
			i++;
		}
		write_sagem(buf);
		current_line++;
	}
}
