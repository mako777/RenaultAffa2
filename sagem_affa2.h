/*
 *	sagem_affa2.h
 *
 *  Created on: 13 lis 2018
 *      Author: djraszit, Mako
 */

#ifndef SAGEM_AFFA2_H_
#define SAGEM_AFFA2_H_

#define ICON_AF					3 << 4
#define ICON_AF_BLINK			1 << 4
#define ICON_I_NEWS				3 << 0
#define ICON_I_NEWS_BLINK		1 << 0
#define ICON_I_TRAFFIC			3 << 2
#define ICON_I_TRAFFIC_BLINK	1 << 2
#define ICON_TUNER_LIST			3 << 6
#define ICON_TUNER_LIST_BLINK	1 << 6
#define ICON_TUNER_PRESET_ON	1 << 9
#define ICON_TUNER_MANU_ON		1 << 13
#define ICON_MSS_ON				1 << 10
#define ICON_DOLBY_ON			1 << 11

#define ICON_ALL			0xffff

#define NO_SCROLL			0x01
#define SWITCH_TEXT			0x02
#define SCROLL_TEXT			0x03

void sagem_affa2_set_icon(uint16_t icon);
void sagem_affa2_clr_icon(uint16_t icon);
void sagem_affa2_channel(uint8_t ch);
void sagem_affa2_init();
void read_sagem(uint8_t *buf);
void write_sagem(uint8_t *buf);
uint8_t make_address(uint8_t ile, uint8_t ktora, uint8_t typ);
void write_text_sagem(const char *text, uint8_t scroll_type);


#endif /* SAGEM_AFFA2_H_ */
