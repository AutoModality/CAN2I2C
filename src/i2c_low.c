/*
 * This file is part of Project 38189
 *
 * Copyright (C) 2016 Auvidea GmbH
 *
 */

#include <stm32f0xx_i2c.h>
#include "i2c.h"
#include "usart.h"
#include "rcc.h"
#include <stdio.h>

enum errorState errorState;

void chipReset() {
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
	DelayMil(1);
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	DelayMil(1);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void i2c_wr_byte(uint8_t Addr, uint8_t Reg, uint8_t DataByte) {
	i2cData.device_address = Addr;
	i2cData.register_address = Reg;
	i2cData.tx_values[0] = DataByte;
	I2C_WrReg(i2cData.device_address, i2cData.register_address, i2cData.tx_values, 1);
}

uint8_t i2c_rd_byte(uint8_t Addr, uint8_t Reg) {
	I2C_RdReg(Addr, Reg, i2cData.rx_values, 1, 0);
	return i2cData.rx_values[0];
}

uint16_t I2C_WrReg(uint8_t Addr, uint8_t Reg, const uint8_t *Data, uint16_t DCnt) {
	Addr = (Addr<<1);
	uint16_t Cnt = 0;

	uint16_t i = 0;
	while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_BUSY) == SET) {
		i++;
		if(i == I2C_TIMEOUT) {
			return errorState = BAD_VALUE;
		}
	}
	I2C_TransferHandling(EP9351_I2C, Addr, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);
	i = 0;
	while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_TXIS) == RESET) {
		i++;
		if(i == I2C_TIMEOUT) {
			return errorState = BAD_DEV_ADDR;
		}
	}
	I2C_SendData(EP9351_I2C, Reg);
	i = 0;
	while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_TCR) == RESET) {
		i++;
		if(i == I2C_TIMEOUT) {
			return errorState = BAD_VALUE;
		}
	}
	I2C_TransferHandling(EP9351_I2C, Addr, DCnt, I2C_AutoEnd_Mode, I2C_No_StartStop);
	i = 0;
	while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_TXIS) == RESET) {
		i++;
		if(i == I2C_TIMEOUT) {
			return errorState = BAD_VALUE;
		}
	} // missing Data
	for(Cnt = 0; Cnt<DCnt; Cnt++){
//		snprintf(usartStr, 30, "Data[%d] = %X\n\r", Cnt ,Data[Cnt]);
//		USART_WriteString(usartStr);
		i = 0;
		while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_TXE) == RESET) {
			i++;
			if(i == I2C_TIMEOUT) {
				return errorState = BAD_VALUE;
			}
		}
		I2C_SendData(EP9351_I2C, Data[Cnt]);
	}
	i = 0;
	while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_STOPF) == RESET) {
		i++;
		if(i == I2C_TIMEOUT) {
			return errorState = BAD_VALUE;
		}
	}
	I2C_ClearFlag(EP9351_I2C, I2C_FLAG_STOPF);

	return 0;
}


uint16_t I2C_RdReg(uint8_t Addr, uint8_t Reg, uint8_t *Data, uint8_t DCnt, uint8_t incr) {
	Addr = ((Addr<<1)+1);
	uint16_t ret = 0;
	int Cnt, i = 0;

	for(Cnt = 0; Cnt<DCnt; Cnt++){
		while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_BUSY) == SET);
		I2C_TransferHandling(EP9351_I2C, Addr, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
		while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_TXIS) == RESET) {
			i++;
			if(i == I2C_TIMEOUT) {
				return errorState = BAD_DEV_ADDR;
			}
		}
		I2C_SendData(EP9351_I2C, (uint8_t)Reg);
		if (incr!=0) {
			while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_TC) == RESET);
			I2C_TransferHandling(EP9351_I2C, Addr, 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
			while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_RXNE) == RESET);
			Data[Cnt] = I2C_ReceiveData(EP9351_I2C);
			Reg++;
		} else if (incr==0) {
				while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_TC) == RESET);
				I2C_TransferHandling(EP9351_I2C, Addr, DCnt, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
				for(Cnt = 0; Cnt<DCnt; Cnt++){
					while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_RXNE) == RESET);
					Data[Cnt] = I2C_ReceiveData(EP9351_I2C);
				}
		}
		i = 0;
	}
	while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_STOPF) == RESET);
	I2C_ClearFlag(EP9351_I2C, I2C_FLAG_STOPF);
	ret = Data[0];
	return ret;
}

// Code for reading LW20 for its special timing, Auvidea
uint16_t I2C_RdRegLW(uint8_t Addr, uint8_t *Data, uint8_t DCnt)
{
	Addr = ((Addr<<1)+1);
	//uint16_t ret = 0;
	int Cnt = 0;

	uint16_t i = 0;
	while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_BUSY) == SET) {
		i++;
		if(i == I2C_TIMEOUT) {
			return errorState = BAD_VALUE;
		}
	}
	I2C_TransferHandling(EP9351_I2C, Addr, DCnt, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
	for(Cnt = 0; Cnt<DCnt; Cnt++)
	{
		i = 0;
		while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_RXNE) == RESET) {
			i++;
			if(i == I2C_TIMEOUT) {
				return errorState = BAD_VALUE;
			}
		}
		Data[Cnt] = I2C_ReceiveData(EP9351_I2C);
	}
	i = 0;
	while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_STOPF) == RESET) {
		i++;
		if(i == I2C_TIMEOUT) {
			return errorState = BAD_VALUE;
		}
	}
	I2C_ClearFlag(EP9351_I2C, I2C_FLAG_STOPF);
	//ret = Data[0];

	return 0;
}

/*
 * Routine probes every register of given address
 * returns 0 if true
 */
uint16_t I2C_listDev(uint8_t Addr){
	Addr = ((Addr<<1)+1);
	uint16_t i = 0;
	uint8_t Reg = 0;

	while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_BUSY) == SET);
	I2C_TransferHandling(EP9351_I2C, Addr, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
	while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_TXIS) == RESET) {
		i++;
		if(i == I2C_TIMEOUT) {
			return 1;
		}
	}
	I2C_SendData(EP9351_I2C, (uint8_t)Reg);
	while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_TC) == RESET);
	I2C_TransferHandling(EP9351_I2C, Addr, 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
	while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_RXNE) == RESET);
	I2C_ReceiveData(EP9351_I2C);
	while(I2C_GetFlagStatus(EP9351_I2C, I2C_FLAG_STOPF) == RESET);
	I2C_ClearFlag(EP9351_I2C, I2C_FLAG_STOPF);
	return 0;
}

//void i2cInit(uint8_t periph) {
void i2cInit(void) {
		//I2C_SoftwareResetCmd(EP9351_I2C);

		GPIO_InitTypeDef GPIO_InitStructure;
		I2C_InitTypeDef I2C_InitStructure;

		//Setup a systick interrupt every 1ms (1/1000 seconds)
		//SysTick_Config(SystemCoreClock/1000);
		//Enable GPIOB clock, required for the I2C output
		//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); //todo already enabled in rcc.c
		//Enable the I2C peripheral clock

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE); // STM32F042
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE); // STM32F072

//	    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_1 | GPIO_Pin_0;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);

	    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);

	    //Set the pin pb5 as output
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		//Set the pins EP9351_SDA and EP9351_SCL as alternate function GPIO pins
		GPIO_InitStructure.GPIO_Pin = EP9351_SDA | EP9351_SCL;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
		GPIO_Init(EP9351_GPIO, &GPIO_InitStructure);

		//Configure the pins to the I2C AF
		GPIO_PinAFConfig(EP9351_GPIO, EP9351_SDA_PS, EP9351_PIN_AF);
		GPIO_PinAFConfig(EP9351_GPIO, EP9351_SCL_PS, EP9351_PIN_AF);

		I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
		I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
		I2C_InitStructure.I2C_DigitalFilter = 0;
		I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
		I2C_InitStructure.I2C_OwnAddress1 = 0xAB;
		I2C_InitStructure.I2C_Timing = 0x10805E89;
		I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		I2C_Init(EP9351_I2C, &I2C_InitStructure);
		I2C_Cmd(EP9351_I2C, ENABLE);

		//I2C_ClearFlag(EP9351_I2C, I2C_FLAG_STOPF);
}

void bbInit(void) {
		GPIO_InitTypeDef GPIO_InitStructure;

		//Set the pins EP9351_SDA and EP9351_SCL as output for bit-banging mode
		GPIO_InitStructure.GPIO_Pin = EP9351_SDA | EP9351_SCL;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_1;
		GPIO_Init(EP9351_GPIO, &GPIO_InitStructure);
}
