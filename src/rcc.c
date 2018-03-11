/*
 * This file is part of Project 38214
 *
 * Copyright (C) 2017 Auvidea GmbH
 *
 */

#include "rcc.h"

RCC_ClocksTypeDef rccClocks;
volatile uint32_t Milliseconds = 0;
volatile uint32_t Seconds = 0;
uint16_t PrescalerValue = 0;

void rccConfiguration(void) {

	SystemInit(); //Ensure CPU is running at correctly set clock speed
	SystemCoreClockUpdate(); //Update SystemCoreClock variable to current clock speed
	SysTick_Config(SystemCoreClock/MillisecondsIT); //Set up a systick interrupt every millisecond

	GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE); //| RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD | RCC_AHBPeriph_GPIOE, ENABLE);
    GPIO_StructInit(&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;

    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // exclude PA13 & PA14 for SWD
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All & ~(GPIO_Pin_13 | GPIO_Pin_14);
    GPIO_Init(GPIOA, &GPIO_InitStructure);

}

void SysTick_Handler(void) {
    Milliseconds++; 				//Increment millisecond variable
    if(Milliseconds%1000 == 999){ 	//If 1000 milliseconds have passed, increment seconds
        Seconds++;
    }
}

//Delay function for millisecond delay
void DelayMil(uint32_t MilS) {
      volatile uint32_t MSStart = Milliseconds;
      while ((Milliseconds - MSStart) < MilS)
    	  asm volatile("nop");
}

//Delay function for second delay
void DelaySec(uint32_t S) {
      volatile uint32_t Ss = Seconds;
      while	((Seconds - Ss) < S)
    	  asm volatile("nop");
}

uint32_t timerMilli(void) {
	return Milliseconds;
}
