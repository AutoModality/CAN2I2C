/*
 * This file is part of Project CAN2UART
 *
 * Copyright (C) 2017 Auvidea GmbH
 *
 */

#include <stdio.h>
#include "i2c.h"
#include "usart.h"

void write_I2C(void) {
	I2C_WrReg(i2cData.device_address, i2cData.register_address, i2cData.tx_values, i2cData.number_values); // &TxData: an array is no pointer! http://www.mikrocontroller.net/topic/73475
}

void read_I2C(mode) {
	int i, j;
	uint16_t ret = 0; // return error
	ret = I2C_RdReg(i2cData.device_address, 0x00, i2cData.rx_values, 1, 1);
	if (ret == BAD_DEV_ADDR) {
		return;
	} else {
		if(mode==1) {
			//USART_WriteString("     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\r\n");
			for(j=0; j<16; j++) {
				//snprintf(usartStr, 5, "%X0: ", j);
				//USART_WriteString(usartStr);
				I2C_RdReg(i2cData.device_address, i2cData.register_address, i2cData.rx_values, 16, 1);
				for(i=0; i<16; i++){
					//snprintf(usartStr, 5, "%.2X ", i2cData.rx_values[i]);
					//USART_WriteString(usartStr);
					i2cData.register_address++;
				}
				//USART_WriteString("\r\n");
			}
		} else if(mode==2) {
			I2C_RdReg(i2cData.device_address, i2cData.register_address, i2cData.rx_values, i2cData.number_values, 0);
			for(i=0; i<i2cData.number_values; i++) {
				//snprintf(usartStr, 40, "reg: 0x%.2X data: 0x%.2X\r\n", i2cData.register_address++, i2cData.rx_values[i]);
				//USART_WriteString(usartStr);
			}

		}
	}
}

void list_I2C(void) {
	int i = 0, j = 0;
	int ret = 0;
	uint8_t DevAddr = 3;
	USART_WriteString("responding I2C devices:\r\n");
	USART_WriteString("     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\r\n");
	for(j=0; j<8; j++) {
		snprintf(usartStr, 5, "%X0: ", j);
		USART_WriteString(usartStr);
		for(i=0; i<16; i++){
			if ( (j==0 && (i == 0 || i == 1 || i == 2)) || (j==7 && (i == 8 || i == 9 || i == 10 || i == 11 || i == 12 || i == 13 || i == 14 || i == 15)) ) {
				USART_WriteString("   ");
			} else {
				ret = I2C_listDev(DevAddr);
				if (ret==0){
					snprintf(usartStr, 10, "%.2X ", DevAddr);
					USART_WriteString(usartStr);
					DevAddr = DevAddr + 1;
				} else {
					USART_WriteString("-- ");
					DevAddr = DevAddr + 1;
				}
			}
		}
		USART_WriteString("\r\n");
	}
}
