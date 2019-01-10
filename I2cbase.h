#ifndef I2CBase_H_
#define I2CBase_H_

#include <stdint.h>
#include <util/twi.h>

#define I2CBUSCLOCK		12000UL

#define I2C_OK          0
#define I2C_STARTError	1
#define I2C_NoNACK		3
#define I2C_NoACK		4

extern uint8_t I2C_Error;
inline void I2C_SetError(uint8_t err) { I2C_Error=err;};

void I2C_Init();
void I2C_Start();
static inline void I2C_Stop() {TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);};
void I2C_SetBusSpeed(uint16_t speed);
void I2C_SendAddr(uint8_t address);
void I2C_SendByte(uint8_t byte);
uint8_t I2C_ReceiveData_ACK();
uint8_t I2C_ReceiveData_NACK();
static inline void I2C_WaitForComplete() {while (!(TWCR & _BV(TWINT)));};
static inline void I2C_WaitTillStopWasSent() {while (TWCR & _BV(TWSTO));};
void I2C_SendStartAndSelect(uint8_t addr);
uint8_t I2C_WaitForComplete_timeout();
uint8_t print_tw_status(char *name);

#endif
