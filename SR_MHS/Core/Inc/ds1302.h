/*
 * ds1302.h
 *
 *  Created on: May 30, 2022
 *      Author: MichałKos(252933)
 */

#ifndef INC_DS1302_H_
#define INC_DS1302_H_

#include "main.h"

#define _DS1302_SPI			hspi3
#define _DS1302_CS_GPIO		CS_RTC_GPIO_Port
#define _DS1302_CS_PIN		CS_RTC_Pin

#define Time_24_Hour		0x00
#define Time_Start			0x00

#define DS1302_SECOND		0x80
#define DS1302_MINUTE		0x82
#define DS1302_HOUR			0x84
#define DS1302_DAY			0x86
#define DS1302_MONTH		0x88
#define DS1302_WEEK			0x8A
#define DS1302_YEAR			0x8C
#define DS1302_WRITE		0x8E
#define DS1302_POWER		0x90
#define CHARGE_UP  			0xa5

uint8_t DS1302_Read(uint8_t addr);
void DS1302_Write(uint8_t addr, uint8_t data);
uint8_t DS1302_ReadRam(uint8_t addr);
void DS1302_WriteRam(uint8_t addr, uint8_t data);
void ReadDS1302Clock(uint8_t *p);
void DS1302_init();

#endif /* INC_DS1302_H_ */
