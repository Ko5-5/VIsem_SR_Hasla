/*
 * ds1302.c
 *
 *  Created on: May 30, 2022
 *      Author: MichałKos(252933)
 */
#include "ds1302.h"

extern SPI_HandleTypeDef _DS1302_SPI;

uint8_t DS1302_Read(uint8_t addr){
	uint8_t data = 0, adr = addr|0x01;
	HAL_GPIO_WritePin(_DS1302_CS_GPIO, _DS1302_CS_PIN, GPIO_PIN_RESET);
	HAL_Delay(3);
	HAL_SPI_Transmit(&_DS1302_SPI, &adr, 8, 100);
	HAL_SPI_Receive(&_DS1302_SPI, &data, 8, 100);
	HAL_Delay(3);
	HAL_GPIO_WritePin(_DS1302_CS_GPIO, _DS1302_CS_PIN, GPIO_PIN_SET);
	return data;
}

void DS1302_Write(uint8_t addr, uint8_t data){
	HAL_GPIO_WritePin(_DS1302_CS_GPIO, _DS1302_CS_PIN, GPIO_PIN_RESET);
	HAL_Delay(3);
	HAL_SPI_Transmit(&_DS1302_SPI, &addr, 8, 100);
	HAL_SPI_Transmit(&_DS1302_SPI, &data, 8, 100);
	HAL_Delay(3);
	HAL_GPIO_WritePin(_DS1302_CS_GPIO, _DS1302_CS_PIN, GPIO_PIN_SET);
}

uint8_t DS1302_ReadRam(uint8_t addr){
	uint8_t tmp, res;
	tmp = (addr<<1)|0xc0;
	res = DS1302_Read(tmp);
	return res;
}

void DS1302_WriteRam(uint8_t addr, uint8_t data){
	uint8_t tmp;
	DS1302_Write(DS1302_WRITE, 0x00);
	tmp = (addr<<1)|0xc0;
	DS1302_Write(tmp,data);
	DS1302_Write(DS1302_WRITE,0x80);
}

void ReadDS1302Clock(uint8_t *p){
	uint8_t tmp = 0xbf, data;
	HAL_GPIO_WritePin(_DS1302_CS_GPIO, _DS1302_CS_PIN, GPIO_PIN_RESET);
	HAL_Delay(3);
	HAL_SPI_Transmit(&_DS1302_SPI, &tmp, 8, 100);
	HAL_SPI_Receive(&_DS1302_SPI, &data, 8, 100);
	p[5] = data;
	HAL_SPI_Receive(&_DS1302_SPI, &data, 8, 100);
	p[4] = data;
	HAL_SPI_Receive(&_DS1302_SPI, &data, 8, 100);
	p[3] = data;
	HAL_SPI_Receive(&_DS1302_SPI, &data, 8, 100);
	p[2] = data;
	HAL_SPI_Receive(&_DS1302_SPI, &data, 8, 100);
	p[1] = data;
	HAL_SPI_Receive(&_DS1302_SPI, &data, 8, 100);
	p[6] = data;
	HAL_SPI_Receive(&_DS1302_SPI, &data, 8, 100);
	p[0] = data;
	HAL_Delay(3);
	HAL_GPIO_WritePin(_DS1302_CS_GPIO, _DS1302_CS_PIN, GPIO_PIN_SET);
}

void DS1302_init(){
	uint8_t tmp;
	tmp = DS1302_ReadRam(0);
	if(tmp^0xa5){
		DS1302_WriteRam(0,0xa5);
		DS1302_Write(DS1302_WRITE,0x00);
		DS1302_Write(DS1302_YEAR,0x14);
		DS1302_Write(DS1302_MONTH,0x07);
		DS1302_Write(DS1302_DAY,0x15);
		DS1302_Write(DS1302_HOUR,0x17);
		DS1302_Write(DS1302_MINUTE,0x58);
		DS1302_Write(DS1302_SECOND,0x50);
		DS1302_Write(DS1302_WEEK,0x02);
		DS1302_Write(DS1302_POWER,0xc5);

		DS1302_Write(CHARGE_UP,0xa5);
		DS1302_Write(DS1302_WRITE,0x80);
	}
}
