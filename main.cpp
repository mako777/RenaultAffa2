/*
 *	Program do obs³ugi wyœwietlacza AFFA2++ w samochodach Renault
 *
 *  Created on: 13 lis 2018
 *      Author: Mako
 *
 Uk³ad pozwalaj¹cy na komunikacjê z wyœwietlaczem w samochodzie, wysy³a do wyœwietlacza
 tekst do wyœwietlenia i odbiera naciœniêcia przycisków na pilocie przy kierownicy.

 http://tlcdcemu.sourceforge.net/hardware.html
 Opis wtyczki od wyœwietlacza http://tlcdcemu.sourceforge.net/img/tl_pinout_anim.gif:
 C1  connector (¿ó³ta, przy radiu):
 1   DTA   I2C-Data
 2   CLK   I2C-Clock
 3   MIRQ  trzeba go œci¹gn¹æ do masy przed wysy³aniem
 4   n/c
 5   Radio ON	podaæ 5V ¿eby siê w³¹czy³ wyœwietlacz
 6   GND

 Bêdziemy u¿ywaæ wszystkich pinów z tej wtyczki.
 I2C do pinów I2C, MIRQ i "Radio on" do któregokolwiek pinu.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "I2cbase.h"
#include "sagem_affa2.h"
#include "timer.h"
#include "usart.h"
#include "constDef.h"
#include "AVRTools/Analog2Digital.h"
#include <stdint.h>
#include <avr/wdt.h>

void get_mcusr(void) __attribute__((naked))
__attribute__((section(".init3")));
void get_mcusr(void) {
	MCUSR = 0;
	wdt_disable();
}

#define TIMER0_COUNTER_START	105

volatile uint8_t timer1ms;
volatile uint16_t timer1s;
// Przerwanie timera 0; wywo³uje siê co 0,1ms
ISR(TIMER0_OVF_vect){
	TCNT0 = TIMER0_COUNTER_START;			// Reset licznika timera do pocz¹tkowej wartoœci
	++timer1ms;									// Zwiêkszenie licznika
	++timer1s;
}

void displayText(const char *text){
	printf("DisplayText: %s\n", text);
	write_text_sagem(text, SCROLL_TEXT);
}

#define DELAYY		_delay_ms(125)
uint8_t displayReset = 0, displayWatchdog = 0;
void welcomeScreen(union timeoutarg arg){
	switch (arg.v) {
	case 1: {
		displayText("lazaaaaa");
		DELAYY;
		sagem_affa2_set_icon(ICON_TUNER_LIST);
		DELAYY;
		sagem_affa2_set_icon(ICON_AF);
		DELAYY;
		sagem_affa2_set_icon(ICON_I_NEWS);
		DELAYY;
		sagem_affa2_set_icon(ICON_TUNER_PRESET_ON);
		DELAYY;
		sagem_affa2_set_icon(ICON_TUNER_MANU_ON);
		DELAYY;
		sagem_affa2_set_icon(ICON_I_TRAFFIC);
		DELAYY;
		sagem_affa2_set_icon(ICON_MSS_ON);
		DELAYY;
		sagem_affa2_set_icon(ICON_DOLBY_ON);
		arg.v = 2;
		timeout(1000, welcomeScreen, arg);
		break;
	}
	case 2: {
//		sagem_affa2_clr_icon(ICON_ALL);
		sagem_affa2_set_icon(ICON_AF_BLINK);
		sagem_affa2_set_icon(ICON_I_NEWS_BLINK);
		sagem_affa2_set_icon(ICON_I_TRAFFIC_BLINK);
		sagem_affa2_set_icon(ICON_TUNER_LIST_BLINK);
		displayText("Gotowy");
		arg.v = 3;
		timeout(3000, welcomeScreen, arg);
		break;
	}
	case 3: {
//		displayText(" ");
		sagem_affa2_clr_icon(ICON_ALL);
		write_text_sagem("*x*x*x*x*x*x*x*xx*x*x*x*", SWITCH_TEXT);
		break;
	}
	}
	displayReset = 0;
}

#define R2RDELAY	200
uint8_t resetPending = 0;
void r2rReset(union timeoutarg arg){
	R2R_SET_0000;
	resetPending = 0;
	displayReset = 1;
}
void r2rSet(uint8_t r2r){
	if(resetPending == 0){
		switch(r2r){
		case 0:	R2R_SET_00; break;
		case 1:	R2R_SET_01; break;
		case 2:	R2R_SET_02; break;
		case 3:	R2R_SET_03; break;
		case 4:	R2R_SET_04; break;
		case 5:	R2R_SET_05; break;
		case 6:	R2R_SET_06; break;
		case 7:	R2R_SET_07; break;
		case 8:	R2R_SET_08; break;
		case 9:	R2R_SET_09; break;
		case 10:	R2R_SET_10; break;
		case 11:	R2R_SET_11; break;
		case 12:	R2R_SET_12; break;
		case 13:	R2R_SET_13; break;
		case 14:	R2R_SET_14; break;
		case 15:	R2R_SET_15; break;
		default: R2R_SET_00;
		}
		resetPending = 1;
		displayReset = 0;
		union timeoutarg arg;
		timeout(R2RDELAY, r2rReset, arg);		// Dajmy chwilê ¿eby radio za³apa³o ¿e coœ zosta³o klikniête, po czym "odkliknijmy" to
	}
}

float measureVoltage(int adcInputPin){
	float voltage = 0.0;
	switch(adcInputPin){
	case ADC_12V_BATT_PIN: {
		// Przyk³adowe obliczenie napiêcia jest nastêpuj¹ce:
		// Mamy dzielnik napiêcia sk³¹daj¹cy siê z oporników R1=3MOhm i R2=1MOhm, pod³¹czenie: POMIAR+ -> R1 -> ADC -> R2 -> GND
		// Takie oporniki daj¹ zakres 0-20V (przy 20V na wejœciu jest 5V na pinie ADC), atmega ma 10 bitowy konwerter ADC, czyli 1024 mo¿liwe wartoœci napiêcia na pinie ADC z zakresu 0-5V
		// Daje to dok³adnoœæ pomaiaru 5V na poziomie 0,00488759V, ale ¿e mierzymy max 20V wiêc wynik trzeba pomno¿yæ razy 4 co daje dok³adnoœæ 0,01955034V
		// ADC odczytuje wartoœæ w zakresie 0-1023, ¿eby to przerobiæ na napiêcie trzeba odczytan¹ wartoœæ pomno¿yæ przez "dok³adnoœæ pomiaru", czyli 0,00488759V, a to jest nic innego jak
		// maksymalna wartoœæ napiêcia (czyli 5V) podzielona przez maksymaln¹ wartoœæ odczytu z ADC (czyli 1023), albo dla lepszego zobrazowania trzeba policzyæ sobie to od dupy strony, czyli
		// maksymalny odczyt z ADC powinien daæ wynik 5V, co daje 1023*X=5 -> X=5/1023 -> X=0,00488759, oczywiœcie tutaj u¿ywamy jeszcze dzielnika napiêcia, który zmniejsza nam napiêcie
		// 4-ro krotnie, czyli wynik mno¿ymy jeszcze przez 4 i mamy gotowe napiêcie z zakresu 0-20V, w sam raz np. do samochodu :)

		int aaa = readA2D(ADC_12V_BATT_PIN);
		voltage = aaa * ADC_REF_VOLT / 1023.0 * 15.815;		// Ostatnie mno¿enie wynika z dzielnika napiêcia oraz napiêcia odniesienia, ¿eby by³ zakres pomiarowy wiêkszy
//		printf("ADC %i | ", aaa);									// wartoœæ dobrana doœwiadczalnie miernikiem, ¿eby wskazanie atmegi zgadza³o siê z napiêciem faktycznym
		// Zastosowane oporniki to 563kOhm i 38kOhm (wartoœci zmierzone), napiêcie odniesienia wewnêtrzne (zmierzone na 1,0855V), daje to zakres dok³adnie 0 - 17,2V

	} break;
	case ADC_BUTTONS_PIN: {
		int aaa = readA2D(ADC_BUTTONS_PIN);
		voltage = aaa * ADC_REF_VOLT / 1023.0;
	} break;
	}

	return voltage;
}

float voltage = 0.0;
uint8_t refreshDisplay = 0;
char voltBuff[12];
void displayVoltage(union timeoutarg arg){
	if(refreshDisplay){
		sprintf(voltBuff, "%6.2f V", (double)voltage);
		voltBuff[3] = ',';
		displayText(voltBuff);
		timeout(777, displayVoltage, arg);
	}
}

uint8_t blink = 1;
int main() {
	LED_PORT_CONF;
	LCD_RADIO_ON_CONF;
	LCD_RADIO_ON_ON;

	R2R_PORT_CONF;
	R2R_SET_0000;
	DDRD &= ~(1<<PD3);
	PORTD |= (1<<PD3);

	// ####### Ustawienia timerów ########
	// Timer 0; 8 bit		Bêdzie s³u¿y³ do odmierzania czasu co 1ms (albo czêœciej)
	TCCR0B |= (1<<CS01);					// CS02 = 0; CS01 = 1; CS00 = 0; Wewnêtrzny zegar, preskaler 8 (str. 142 w nocie)
	TIMSK0 |= (1<<TOIE0);				// Uruchomienie przerwania po przepe³nieniu timera (str. 159)
	TCNT0 = TIMER0_COUNTER_START;		// Timer liczy od 105 do 255 co daje przerwanie równo co 0,1ms

	sei();		//Globalne uruchomienie przerwañ

	// ####### Ustawienie komunikacji RS232 z kompem ########
	//uart_set_FrameFormat(USART_8BIT_DATA|USART_1STOP_BIT|USART_NO_PARITY|USART_ASYNC_MODE);			// default settings
	uart_init(BAUD_CALC(115200));				// 8n1 transmission is set as default
	stdout = &uart0_io;							// attach uart stream to stdout & stdin
	stdin = &uart0_io;							// uart0_in and uart0_out are only available if NO_USART_RX or NO_USART_TX is defined
	uart_putstr("Serial start at speed: 115200 kbps\n");

	initA2D(kA2dReference11V);			// Inicjalizacja ADC

	I2C_Init();				// Start I2C

	sagem_affa2_init();	// Inicjalizacja wyœwietlacza

	union timeoutarg arg;
	arg.v = 1;
	timeout(1000, welcomeScreen, arg);		// Wywo³anie powitania po ma³ym opóŸnieniu (¿eby siê ustabilizowa³a komunikacja z ekranem)

	uint8_t uartBytes, dataRead[6], voltageBuffInd = 0;
	float voltageBuff[20];
	wdt_enable(WDTO_8S);
	while (1) {
		if(timer1ms > 10){		// Wywo³a siê co 1ms
			timer1ms = 1;
			timertick();
			wdt_reset();

			if(uart0_AvailableBytes() > 0){
				_delay_ms(50);
				uartBytes = uart0_AvailableBytes();
				char buffer[uartBytes+1];
				uart_getln(buffer, uartBytes+1);
				printf("Serial data: received %i bytes: %s\n", uartBytes, buffer);
				switch (buffer[0]){
				case '1':		// 1
					displayText("abcdefgh");
					break;
				case '2':		// 2
					displayText("Hello World");
					break;
				case '3':		// 3
					displayText("stary, nowy");
					break;
				case '4':		// 4
					displayText("88888888");
					break;
				case '5':		// 5
					displayText("Dupa zbita");
					break;
				case '6':		// 6
					displayText("ble ble elb elb");
					break;
				case '7':		// 7

					break;
				case '8':		// 8
					welcomeScreen(arg);
					break;
				case '9':		// 9
					sagem_affa2_set_icon(ICON_ALL);
					break;
				case '0':		// 0
					sagem_affa2_clr_icon(ICON_ALL);
				break;
				default:
					displayText(buffer);
//					write_text_sagem(buffer, SWITCH_TEXT);
				}
			}

			if(LCD_MIRQ_IS_SET){
				LED_CLR;
			}
			else{
				LED_SET;
				read_sagem(dataRead);
				if(dataRead[1] == 0x82){
					printf("Przycisk: %02X %02X %02X %02X %02X %02X\n", dataRead[0], dataRead[1], dataRead[2], dataRead[3], dataRead[4], dataRead[5]);
					refreshDisplay = 0;
					if(dataRead[3] == 0x00){			// Normalne przyciski
						switch (dataRead[4]){
						case REMOTE_KEY_LOAD: {				// przycisk na dole pilota
							// tutaj trzeba "nacisn¹æ" przycisk na radiu, czyli zewrzeæ na chwilê kabelek do masy przez odpowiedni opornik
							// lub ew. wykonaæ jak¹œ inn¹ akcjê dla danego przycisku
							printf("Przycisk na dole\n");
//							displayText("Dolny");
							refreshDisplay = 1;
							displayVoltage(arg);
							} break;
						case REMOTE_KEY_LOAD_LONG: {
							printf("Przycisk na dole d³ugo\n");
//							displayText("Dolny dl");
							welcomeScreen(arg);
							} break;
						case REMOTE_KEY_SRC_RIGHT: {		// na górze
							printf("Przycisk na górze prawy\n");
							displayText("Play/Pauza");
//							R2R_SET_15;
							r2rSet(15);

							} break;
						case REMOTE_KEY_SRC_RIGHT_LONG: {
							printf("Przycisk na górze prawy d³ugo\n");
							displayText("Gr pr dl");
//							R2R_SET_12;
							r2rSet(12);
							} break;
						case REMOTE_KEY_SRC_LEFT: {		// na górze
							printf("Przycisk na górze lewy\n");
							displayText("GPS/Mapa");
//							R2R_SET_08;
							r2rSet(8);
							} break;
						case REMOTE_KEY_SRC_LEFT_LONG: {
							printf("Przycisk na górze lewy d³ugo\n");
							displayText("Gr lw dl");
//							R2R_SET_06;
							r2rSet(6);
							} break;
						case REMOTE_KEY_VOLUME_UP: {
							printf("Przycisk g³oœniej\n");
							displayText("Glosniej");
//							R2R_SET_01;
							r2rSet(1);
							if(blink == 0){
								sagem_affa2_clr_icon(ICON_DOLBY_ON);
								blink = 1;
							}
							} break;
						case REMOTE_KEY_VOLUME_UP_HOLD: {
							printf("Przycisk g³oœniej trzyma\n");
							displayText("Glosniej");
//							R2R_SET_01;
							r2rSet(1);
							if(blink){
								sagem_affa2_set_icon(ICON_DOLBY_ON);
								blink = 0;
							}
							else{
								sagem_affa2_clr_icon(ICON_DOLBY_ON);
								blink = 1;
							}
							} break;
						case REMOTE_KEY_VOLUME_DOWN: {
							printf("Przycisk ciszej\n");
							displayText("Ciszej");
//							R2R_SET_02;
							r2rSet(2);
							if(blink == 0){
								sagem_affa2_clr_icon(ICON_DOLBY_ON);
								blink = 1;
							}
							} break;
						case REMOTE_KEY_VOLUME_DOWN_HOLD: {
							printf("Przycisk ciszej trzyma\n");
							displayText("Ciszej");
//							R2R_SET_02;
							r2rSet(2);
							if(blink){
								sagem_affa2_set_icon(ICON_DOLBY_ON);
								blink = 0;
							}
							else{
								sagem_affa2_clr_icon(ICON_DOLBY_ON);
								blink = 1;
							}
							} break;
						case REMOTE_KEY_MUTE: {			// naciœniête jednoczeœnie volume up i down
							printf("Przycisk wycisz\n");
							displayText("Wycisz");
//							R2R_SET_03;
							r2rSet(3);
							} break;
						case REMOTE_KEY_MUTE_LONG: {
							printf("Przycisk wycisz d³ugo\n");
							displayText("Wylacz radio");
//							R2R_SET_04;
							r2rSet(4);
							} break;
						}

					}
					else if(dataRead[3] == 0x01){			// rolka z ty³u pilota
//						if(dataRead[4] == REMOTE_KEY_ROLL_UP){
						if(dataRead[4] == REMOTE_KEY_ROLL_UP1 || dataRead[4] == REMOTE_KEY_ROLL_UP2 || dataRead[4] == REMOTE_KEY_ROLL_UP3 || dataRead[4] == REMOTE_KEY_ROLL_UP4 || dataRead[4] == REMOTE_KEY_ROLL_UP5 || dataRead[4] == REMOTE_KEY_ROLL_UP6){
							printf("Rolka do góry\n");
							displayText("Poprzedni utwor");
//							R2R_SET_09;
							r2rSet(9);
						}
//						else if(dataRead[4] == REMOTE_KEY_ROLL_DOWN){
						else if(dataRead[4] == REMOTE_KEY_ROLL_DOWN1 || dataRead[4] == REMOTE_KEY_ROLL_DOWN2 || dataRead[4] == REMOTE_KEY_ROLL_DOWN3 || dataRead[4] == REMOTE_KEY_ROLL_DOWN4 || dataRead[4] == REMOTE_KEY_ROLL_DOWN5 || dataRead[4] == REMOTE_KEY_ROLL_DOWN6){
							printf("Rolka do do³u\n");
							displayText("Nastepny utwor");
//							R2R_SET_05;
							r2rSet(5);
						}
					}
//					timeout(R2RDELAY, r2rReset, arg);		// Dajmy chwilê ¿eby radio za³apa³o ¿e coœ zosta³o klikniête, po czym "odkliknijmy" to
				}
				else if(dataRead[0] == 0x01 && (dataRead[1] == 0x00 || dataRead[1] == 0x01)){			// "Ping" z wyœwietlacza
					dataRead[1] = 0x11;																					// Odpowiadamy aby utrzymaæ komunikacjê
					write_sagem(dataRead);
					displayWatchdog = 0;
				}
			} //if(LCD_MIRQ_IS_SET) ... else
		} //if(timer1ms > 10)		// Wywo³a siê co 1ms

		if(timer1s % 500 == 0){
			voltageBuff[voltageBuffInd++] = measureVoltage(ADC_12V_BATT_PIN);
			if(voltageBuffInd > 19) voltageBuffInd = 0;
		}

		if(timer1s > 5000){		// Wywo³a siê co 1s ... albo ileœtam :) ... 5000 -> 500ms
			timer1s = 1;
			voltage = 0.0;
			for(uint8_t i=0; i<20; ++i){
				voltage += voltageBuff[i];
			}
			voltage /= 20;
//			printf("Napiêcie akumulatora:  %2.2f V\n", (double)voltage);

			if(displayReset > 0){
				if(++displayReset > 20){		// coœ ko³o 10 sekund
					arg.v = 3;
					welcomeScreen(arg);
				}
			}

			if(++displayWatchdog > 40){
				sagem_affa2_init();
				displayText("Restart");
				displayReset = 1;
			}
		}
	}
}

