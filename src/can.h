/*
 * This file is part of Project CAN2UART
 *
 * Copyright (C) 2017 Auvidea GmbH
 *
 */

#ifndef _can_h
#define _can_h

#include "stm32f0xx_can.h"

#define CAN_CAN				CAN
#define CAN_CLK				RCC_APB1Periph_CAN
#define CAN_RX_PIN			GPIO_Pin_8
#define CAN_TX_PIN			GPIO_Pin_9
#define CAN_GPIO_PORT		GPIOB
#define CAN_RX_SOURCE		GPIO_PinSource8
#define CAN_TX_SOURCE		GPIO_PinSource9
#define CAN_AF_PORT			GPIO_AF_4
#define CAN_GPIO_CLK        RCC_AHBPeriph_GPIOB


extern void canInit(void);

extern void can_loopback(void);
extern void send_can_message(uint8_t id, uint8_t dlc, uint8_t * data);
extern void dump_can_message(void);

#endif

