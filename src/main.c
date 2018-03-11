/**
*****************************************************************************
**
**  PROJECT      : CAN2UART
**
**  AUTHOR		 : Daniel Wahl + Florian Dederichs
**
**  Environment  : Atollic TrueSTUDIO(R)
**
**  COPYRIGHT(C) : 2017 Auvidea GmbH
**
*****************************************************************************
**/



#include "rcc.h"
#include "usart.h"
#include "i2c.h"
#include "can.h"

uint16_t flashspeed, ledflash, flsdelay;

uint8_t canmode = 1;					//canmode = 0 loopback 			canmode >= 1 normal

int ncount = 0;
int dcount = 0;

int main(void)
{
	rccConfiguration();
	DelayMil(10);
	//DelaySec(30);	// Adjust this delay to the start of J130, it cannot initialize the CAN bus
					// before J130 starts.

	i2cInit();
	canInit();
    chipReset();
	flsdelay = 2000;
	flashspeed = 1500;
	ledflash = 1;

	bbInit();
	i2cInit();

	uint8_t tx_values[256];

	// stop blinkm default script
	tx_values[0] = 'o';
	I2C_WrReg(0x09, 0x00, tx_values, 1);
	DelaySec(5);

	// bring blinkm off
	tx_values[0] = 'n';
	tx_values[1] = 0x00;
	tx_values[2] = 0x00;
	tx_values[3] = 0x00;
	I2C_WrReg(0x09, 0x00, tx_values, 4);
	DelaySec(5);

	while (1)
	{
		if(ledflash == 1)
		{
			if(ncount != flashspeed)
			{
				GPIO_SetBits(GPIOB, GPIO_Pin_5);
				ncount++;
			}
			if(ncount == flashspeed)
			{
					dcount++;
					GPIO_ResetBits(GPIOB, GPIO_Pin_5);
			}
			if(dcount == flsdelay)
			{
				ncount = 0;
				dcount = 0;
			}
		}
		else if(ledflash == 0)
		{
			ncount = 0;
			dcount = 0;
		}
	}

	return 0;
}
