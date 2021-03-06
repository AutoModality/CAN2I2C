/*
 * This file is part of Project 38214
 *
 * Copyright (C) 2017 Auvidea GmbH
 *
 */

#ifndef _rcc_h
#define _rcc_h

#include <stddef.h>
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"


#define RTC_MASK_YEAR	(0b1111111<<25)
#define RTC_MASK_MONTH	(0b1111<<21)
#define RTC_MASK_DAY	(0b11111<<16)
#define RTC_MASK_HOUR	(0b11111<<11)
#define RTC_MASK_MINUTE	(0b111111<<5)
#define RTC_MASK_SECOND (0b11111)

#define MillisecondsIT 1e3

extern RCC_ClocksTypeDef rccClocks;
extern void rccConfiguration(void);
extern void DelayMil(uint32_t MilS);
extern void DelaySec(uint32_t S);
extern uint32_t timerMilli(void);

#endif
