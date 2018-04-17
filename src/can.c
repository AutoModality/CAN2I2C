/*
 * This file is part of Project CAN2UART
 *
 * Copyright (C) 2017 Auvidea GmbH
 *
 */

#include "main.h"
#include "can.h"
#include "usart.h"
#include <stdio.h>

#include "stm32f0xx_can.h"

void CEC_CAN_IRQHandler(void);

int countCAN = 0;
char tmpStr[64] = "Send a message via UART\n\r";

void canInit(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    CAN_InitTypeDef CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable GPIO clock */
    RCC_AHBPeriphClockCmd(CAN_GPIO_CLK, ENABLE);

    // Connect CAN pins to AF
    GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_RX_SOURCE, CAN_AF_PORT);
    GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_TX_SOURCE, CAN_AF_PORT);

    // Configure CAN RX and TX pins
    GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN | CAN_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStructure);

    /* CAN Periph clock enable */
    RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);

    // CAN register init
    CAN_DeInit(CAN_CAN);
    CAN_StructInit(&CAN_InitStructure);

    // CAN cell init
    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;

    /* CAN Baudrate = 1MBps (CAN clocked at 48 MHz)
     * 48MHz / 3 = 16tq; 16tq = 8tq + 7tq (r1) Sample Point at ca. 50%
     * refer to:  http://www.bittiming.can-wiki.info
     */
    CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;
    CAN_InitStructure.CAN_Prescaler = 3;
    CAN_Init(CAN_CAN, &CAN_InitStructure);

    // all filters for CAN1
    // CAN_SlaveStartBank(27);

    // accept all to start with
    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    /* Enable CAN IRQ on message pending */
	NVIC_InitStructure.NVIC_IRQChannel = CEC_CAN_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    CAN_ITConfig(CAN_CAN, CAN_IT_FMP0, ENABLE); // Enable CAN message pending Interrupt
}

void can_loopback(void) {
	CanRxMsg RxMessage;
	RxMessage.StdId = 0x00;
	RxMessage.IDE = CAN_ID_STD;
	RxMessage.DLC = 0;
	RxMessage.Data[0] = 0x00;
	RxMessage.Data[1] = 0x00;
	RxMessage.Data[2] = 0x00;
	RxMessage.Data[3] = 0x00;
	RxMessage.Data[4] = 0x00;
	RxMessage.Data[5] = 0x00;
	RxMessage.Data[6] = 0x00;
	RxMessage.Data[7] = 0x00;

	uint8_t message_received = 0;
	uint8_t i;
	if(CAN_MessagePending(CAN, CAN_FIFO0) > 0) {
		CAN_Receive(CAN, CAN_FIFO0, &RxMessage);
		message_received = 1;
	}

	if(message_received == 1) {
		CanTxMsg TxMessage;
		uint8_t j;
		uint8_t TransmitMailbox = 0;
		TxMessage.StdId = RxMessage.StdId;
		TxMessage.RTR = CAN_RTR_DATA;
		TxMessage.IDE = CAN_ID_STD;
		TxMessage.DLC = RxMessage.DLC;

		for(j=0; j<TxMessage.DLC ;j++) {
			TxMessage.Data[j] = RxMessage.Data[(TxMessage.DLC-1)-j];
		}

		TransmitMailbox = CAN_Transmit(CAN, &TxMessage);
		i = 0;
		while((CAN_TransmitStatus(CAN, TransmitMailbox)  !=  CANTXOK) && (i  !=  0xFF))
		{
		  i++;
		}
		i = 0;
		while((CAN_MessagePending(CAN, CAN_FIFO0) < 1) && (i  !=  0xFF))
		{
		  i++;
		}
	}
}



void send_can_message(uint8_t id, uint8_t dlc, uint8_t * data) {
	CanTxMsg TxMessage;
	uint32_t i = 0;
	uint8_t j;
	uint8_t TransmitMailbox = 0;
	TxMessage.StdId = id;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.IDE = CAN_ID_STD;
	TxMessage.DLC = dlc;

	for(j=0; j<dlc ;j++) {
		TxMessage.Data[j] = data[j];
	}

	TransmitMailbox = CAN_Transmit(CAN, &TxMessage);
	i = 0;
	while((CAN_TransmitStatus(CAN, TransmitMailbox)  !=  CANTXOK) && (i  !=  0xFFFF))
	{
	  i++;
	}
	i = 0;
	while((CAN_MessagePending(CAN, CAN_FIFO0) < 1) && (i  !=  0xFFFF))
	{
	  i++;
	}
}

void dump_can_message(void) {
	CanRxMsg RxMessage;
	//uint8_t i = 0;

	RxMessage.StdId = 0x00;
	RxMessage.IDE = CAN_ID_STD;
	RxMessage.DLC = 0;
	RxMessage.Data[0] = 0x00;
	RxMessage.Data[1] = 0x00;
	RxMessage.Data[2] = 0x00;
	RxMessage.Data[3] = 0x00;
	RxMessage.Data[4] = 0x00;
	RxMessage.Data[5] = 0x00;
	RxMessage.Data[6] = 0x00;
	RxMessage.Data[7] = 0x00;
	CAN_Receive(CAN, CAN_FIFO0, &RxMessage);
//	snprintf(tmpStr, 63, "%.3x [%i] %.2x", (int)RxMessage.StdId, RxMessage.DLC, RxMessage.Data[0]);
//	USART_WriteString(tmpStr);
//	for(i = 1; i<RxMessage.DLC; i++) {
//		snprintf(tmpStr, 63, " %.2x",RxMessage.Data[i]);
//		USART_WriteString(tmpStr);
//	}
//	USART_WriteString("\n\r");
}

void CEC_CAN_IRQHandler(void)
{
	CanRxMsg can_rx_msg;
	CanTxMsg can_tx_msg;
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t TransmitMailbox = 0;
	uint8_t valid_data = 0;

	can_rx_msg.StdId = 0x00;
	can_rx_msg.IDE = CAN_ID_STD;
	can_rx_msg.DLC = 0;
	can_rx_msg.Data[0] = 0x00;
	can_rx_msg.Data[1] = 0x00;
	can_rx_msg.Data[2] = 0x00;
	can_rx_msg.Data[3] = 0x00;
	can_rx_msg.Data[4] = 0x00;
	can_rx_msg.Data[5] = 0x00;
	can_rx_msg.Data[6] = 0x00;
	can_rx_msg.Data[7] = 0x00;
	CAN_Receive(CAN, CAN_FIFO0, &can_rx_msg);

	can_tx_msg.StdId = 0x00;
	can_tx_msg.IDE = CAN_ID_STD;
	can_tx_msg.DLC = 0;
	can_tx_msg.Data[0] = 0x00;
	can_tx_msg.Data[1] = 0x00;
	can_tx_msg.Data[2] = 0x00;
	can_tx_msg.Data[3] = 0x00;
	can_tx_msg.Data[4] = 0x00;
	can_tx_msg.Data[5] = 0x00;
	can_tx_msg.Data[6] = 0x00;
	can_tx_msg.Data[7] = 0x00;

	// I2C send buffer
	uint8_t i2c_tx_msg[32];
	uint8_t i2c_rx_msg[32];

	switch (can_rx_msg.StdId)
	{
		case 0x00: //BlinkM LED
			switch (can_rx_msg.Data[1])
			{
				case 'n':
				case 'c':
				case 'h':
				case 'C':
				case 'H':
				case 'p':
				case 'o':
				case 'f':
				case 't':
				case 'W':
				case 'L':
				case 'A':
				case 'B':
					for (i = 0; i < can_rx_msg.DLC - 1; i++)
					{
						i2c_tx_msg[i] = can_rx_msg.Data[i+1];
					}
					I2C_WrReg(can_rx_msg.Data[0], 0x00, i2c_tx_msg, can_rx_msg.DLC - 1);
					break;
				case 'a':
				case 'Z':
				case 'R':
				case 'g':
					break;
			}
			break;
		case 0x01: // LW20 altimeter
			// arrange the message to write to i2c
			for (i = 0; i < can_rx_msg.DLC - 1; i++)
			{
				i2c_tx_msg[i] = can_rx_msg.Data[i+1];
			}
			i2c_tx_msg[i++] = '\r';
			i2c_tx_msg[i] = '\n';

			// write lw20 command to i2c
			I2C_WrReg(can_rx_msg.Data[0], 0x00, i2c_tx_msg, can_rx_msg.DLC + 1);

			// read lw20 from i2c
			//ret = I2C_RdReg(can_rx_msg.Data[0], 0x00, i2c_rx_msg, 1, 1);
			I2C_RdRegLW(can_rx_msg.Data[0], i2c_rx_msg, 16);

			// arrange the lw20 reading into CAN frame
			can_tx_msg.StdId = can_rx_msg.StdId;
			can_tx_msg.Data[0] = can_rx_msg.Data[0];
			j = 1;
			for (i = 0; i < 32; i++)
			{
				if (!valid_data)
				{
					if (i2c_rx_msg[i] == ':')
					{
						valid_data = 1;
					}
				}
				else
				{
					//if (i2c_rx_msg[i] != '\r') // it is not as said in spec, end with '\r\n'
					if (i2c_rx_msg[i] != ' ')
					{
						can_tx_msg.Data[j] = i2c_rx_msg[i];
						j++;
					}
					else
					{
						break;
					}
				}
			}
			can_tx_msg.DLC = j;

			// Send lw20 reading to TX2 on CAN bus
			TransmitMailbox = CAN_Transmit(CAN, &can_tx_msg);
			i = 0;
			while((CAN_TransmitStatus(CAN, TransmitMailbox)  !=  CANTXOK) && (i  !=  0xFFFF))
			{
			  i++;
			}
			i = 0;
			while((CAN_MessagePending(CAN, CAN_FIFO0) < 1) && (i  !=  0xFFFF))
			{
			  i++;
			}
			break;
		default:
			break;
	}
}
