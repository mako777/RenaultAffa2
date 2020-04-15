/*
 * constDef.h
 *
 *  Created on: 13 lis 2018
 *      Author: Mako
 */

#ifndef CONSTDEF_H_
#define CONSTDEF_H_

//#include <avr/io.h>
//#include <avr/eeprom.h>

// Pin MIRQ ekranu LCD [PC3] Dowolny pin
#define LCD_MIRQ_PORT		PORTC
#define LCD_MIRQ_DDR			DDRC
#define LCD_MIRQ				PC3
#define LCD_MIRQ_SET			LCD_MIRQ_PORT |=  (1<<LCD_MIRQ)			// ustawia stan H
#define LCD_MIRQ_CLR			LCD_MIRQ_PORT &= ~(1<<LCD_MIRQ)			// ustawia stan L
#define LCD_MIRQ_IS_SET		(PINC & (1<<LCD_MIRQ))						// maskuje bit pinu, if(IS_SET) da wynik true jeœli pin jest w stanie wysokim
#define LCD_MIRQ_OUTPUT		LCD_MIRQ_DDR |=  (1<<LCD_MIRQ)			// ustawia pin jako wyjœcie
#define LCD_MIRQ_INPUT		LCD_MIRQ_DDR &= ~(1<<LCD_MIRQ)			// ustawia pin jako wejœcie

#define LCD_RADIO_ON_CONF	DDRC |= (1<<PC2)
#define LCD_RADIO_ON_ON		PORTC |= (1<<PC2)
#define LCD_RADIO_ON_OFF	PORTC &= ~(1<<PC2)

#define LED_PORT_CONF		DDRB |= (1<<PB5)
#define LED_SET				PORTB |= (1<<PB5)
#define LED_CLR				PORTB &= ~(1<<PB5)

#define ADC_12V_BATT_PIN	6			// ADC6
#define ADC_BUTTONS_PIN		7			// ADC7
//#define ADC_REF_VOLT			4.325		// AVCC; zmierzone miernikiem na 4,325
#define ADC_REF_VOLT			1.0855	// wewnêtrzne 1,1V; zmierzone miernikiem troszkê faluje w zakresie 1,0854 - 1,0856 (bardzo powoli faluje)

// R2R 4bit DAC do sterownia radiem
#define R2R_PORT				DDRD
#define R2R_PORT_CONF		PORTD 	&= ~(1<<PD4) & ~(1<<PD5) & ~(1<<PD6) & ~(1<<PD7)
#define R2R_SET_0000			R2R_PORT &= ~(1<<PD4) & ~(1<<PD5) & ~(1<<PD6) & ~(1<<PD7)
#define R2R_SET_0001			R2R_PORT |=  (1<<PD7)
#define R2R_SET_0010			R2R_PORT |=  (1<<PD6)
#define R2R_SET_0011			R2R_PORT |=  (1<<PD6) |  (1<<PD7)
#define R2R_SET_0100			R2R_PORT |=  (1<<PD5)
#define R2R_SET_0101			R2R_PORT |=  (1<<PD5) |  (1<<PD7)
#define R2R_SET_0110			R2R_PORT |=  (1<<PD5) |  (1<<PD6)
#define R2R_SET_0111			R2R_PORT |=  (1<<PD5) |  (1<<PD6) |  (1<<PD7)
#define R2R_SET_1000			R2R_PORT |=  (1<<PD4)
#define R2R_SET_1001			R2R_PORT |=  (1<<PD4) |  (1<<PD7)
#define R2R_SET_1010			R2R_PORT |=  (1<<PD4) |  (1<<PD6)
#define R2R_SET_1011			R2R_PORT |=  (1<<PD4) |  (1<<PD6) |  (1<<PD7)
#define R2R_SET_1100			R2R_PORT |=  (1<<PD4) |  (1<<PD5)
#define R2R_SET_1101			R2R_PORT |=  (1<<PD4) |  (1<<PD5) |  (1<<PD7)
#define R2R_SET_1110			R2R_PORT |=  (1<<PD4) |  (1<<PD5) |  (1<<PD6)
#define R2R_SET_1111			R2R_PORT |=  (1<<PD4) |  (1<<PD5) |  (1<<PD6) |  (1<<PD7)
/*
 * Trochê inna drabinka R2R, bo niema w niej pod³¹czenia do masy.
 * Radio podaje 3,3V na kablu steruj¹cym i trzeba ten kabel zwieraæ do masy przez oporniki, a¿eby to uzyskaæ robimy tak:
 * Ustawiamy piny na stan niski i jako wejœcia co da nam pseudo nie pod³¹czone nic do kabla steruj¹cego, czyli brak klawisza.
 * Nastêpnie jak chcemy "coœ klikn¹æ" to przestawiamy na chwilê odpowiednie piny na wyjœcia, dziêki temu pojawi siê "zwarcie" do masy przez oporniki,
 * po czym ustawaimy z powrotem wszsytkie piny jako wejœcia (aka brak klawisza). I tyli, powinno dzia³aæ :P  U¿yte oporniki to 10k i 20k
 */
#define R2R_SET_00	R2R_SET_0000
#define R2R_SET_01	R2R_SET_0001
#define R2R_SET_02	R2R_SET_0010
#define R2R_SET_03	R2R_SET_0011
#define R2R_SET_04	R2R_SET_0100
#define R2R_SET_05	R2R_SET_0101
#define R2R_SET_06	R2R_SET_0110
#define R2R_SET_07	R2R_SET_0111
#define R2R_SET_08	R2R_SET_1000
#define R2R_SET_09	R2R_SET_1001
#define R2R_SET_10	R2R_SET_1010
#define R2R_SET_11	R2R_SET_1011
#define R2R_SET_12	R2R_SET_1100
#define R2R_SET_13	R2R_SET_1101
#define R2R_SET_14	R2R_SET_1110
#define R2R_SET_15	R2R_SET_1111


//	Klawisze na pilocie przy kierownicy
#define REMOTE_KEY_LOAD					0x00			// Ten na dole pilota
#define REMOTE_KEY_LOAD_LONG			0x80			// D³ugie przytrzymanie (jest to jako pojedyncze klikniêcie, pojawia siê po przytrzymaniu 3 sekundy)
#define REMOTE_KEY_SRC_RIGHT			0x01			// na górze
#define REMOTE_KEY_SRC_RIGHT_LONG	0x81
#define REMOTE_KEY_SRC_LEFT			0x02
#define REMOTE_KEY_SRC_LEFT_LONG		0x82
#define REMOTE_KEY_VOLUME_UP			0x03
#define REMOTE_KEY_VOLUME_UP_HOLD	0x43			// przytrzymanie klawisza volume daje powtarzaj¹ce siê wywo³ania tego klawisza
#define REMOTE_KEY_VOLUME_DOWN		0x04
#define REMOTE_KEY_VOLUME_DOWN_HOLD	0x44
#define REMOTE_KEY_MUTE					0x05			// naciœniêcie jednoczeœnie volume up i down
#define REMOTE_KEY_MUTE_LONG			0x85
//#define REMOTE_KEY_ROLL_UP				0x41 || 0x42 || 0xC3 || 0XC4 || 0XC5 || 0XC6		// * ; 0x0141, 0x0142, 0x01C3, 0X01C4, 0X01C5, 0X01C6
//#define REMOTE_KEY_ROLL_DOWN			0x01 || 0X02 || 0X83 || 0X84 || 0X85 || 0X86		// * ; 0x0101, 0x0102, 0x0183, 0x0184, 0x0185, 0x0186
#define REMOTE_KEY_ROLL_UP1			0x41
#define REMOTE_KEY_ROLL_UP2			0x42
#define REMOTE_KEY_ROLL_UP3			0xC3
#define REMOTE_KEY_ROLL_UP4			0XC4
#define REMOTE_KEY_ROLL_UP5			0XC5
#define REMOTE_KEY_ROLL_UP6			0XC6
#define REMOTE_KEY_ROLL_DOWN1			0x01
#define REMOTE_KEY_ROLL_DOWN2			0X02
#define REMOTE_KEY_ROLL_DOWN3			0X83
#define REMOTE_KEY_ROLL_DOWN4			0X84
#define REMOTE_KEY_ROLL_DOWN5			0X85
#define REMOTE_KEY_ROLL_DOWN6			0X86
/*
 * * Informacja o rolce:
 * Kod klawiszy to tak naprawdê dwa bajty (3 i 4 z pakietu który wysy³a wyœwietlacz do radia).
 * Normalne klawisze maj¹ "pierwszy" bajt równy 0x00
 * Rolka z drugiej strony ma "pierwszy" bajt równy 0x01
 * Wiêc tutaj u¿ywamy tylko "drugiego" bajtu do okreœlenia konkretnego przycisku, a w programie znajdzie siê jeszcze jeden "IF",
 * który bêdzie sprawdzaæ "pierwszy" bajt i ustalaæ czy zosta³ u¿yty przycisk czy rolka.
 *
 * Natomiast pytanie dlaczego jest a¿ tyle ró¿nych kodów dla rolki, otó¿ wyœwietlacz wysy³a ró¿ne kody w zale¿noœci od tego jak siê tej rolki "u¿ywa"
 * Przy normalnym niezbyt szybkim krêceniu dla ka¿dego "klikniêcia" rolki jest generowane wywo³anie z kodem 0x01 (dla krêcenia w dó³ lub 0x41 w górê),
 * jeœli przyspieszymy krêcenie to dalej ka¿de "klikniêcie" rolki generuje wywo³ania ale zaczynaj¹ pojawiaæ siê kody 0x02 (0x42).
 * A jeœli pójdziemy jeszcze dalej i zaczniemy robiæ gwa³towne "poci¹gniêcia" rolk¹ to wszsytkie "klikniêcia" rolki które mog³y by siê pojawiæ zostan¹
 * zebrane w ramach jednego wywo³ania z kodem 0x83 (0xC3), i tak jeœli bêdziemy jeszcze mocniej krêciæ rolk¹ to ka¿de jedno "poci¹gniêcie" bêdzie zbierane
 * do jednego wywo³ania, tylko z wy¿szym numerem (0x84/0xC4, 0x85/0xC5, 0x86/0xC6).
 * Koñcówka 6 to najwy¿sza wartoœæ jak¹ uda³o mi siê uzyskaæ, a by³o to ju¿ naprawdê "szaleñcze" obchodzenie siê z t¹ rolk¹ :)
 */

#endif /* CONSTDEF_H_ */
