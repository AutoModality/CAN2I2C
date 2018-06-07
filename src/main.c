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
	i2cInit();
	canInit();
    chipReset();
	flsdelay = 2000;
	flashspeed = 1500;
	ledflash = 1;

	DelaySec(1);	// Adjust this delay to the start of J130, it cannot initialize the CAN bus
					// before J130 starts.

	//bbInit();
	//i2cInit();

	//uint8_t tx_values[256];

//	// test LW20
//	i2cData.tx_values[0] = '?';
//	i2cData.tx_values[1] = '\r';
//	i2cData.tx_values[2] = '\n';
//	I2C_WrReg(0x66, 0x00, i2cData.tx_values, 3);
//	DelayMil(1000);
//
//	i2cData.device_address = 0x66;
//    i2cData.register_address = 0;
//    i2cData.number_values = 16;
//    //read_I2C(2);
//    I2C_RdRegLW(0x66, i2cData.rx_values, 16);
//
//	while(1)
//	{
//		i2cData.tx_values[0] = '?';
//		i2cData.tx_values[1] = 'L';
//		i2cData.tx_values[2] = 'D';
//		i2cData.tx_values[3] = '\r';
//		i2cData.tx_values[4] = '\n';
//		I2C_WrReg(0x66, 0x00, i2cData.tx_values, 5);
//		DelayMil(1000);
//
//		i2cData.device_address = 0x66;
//		i2cData.register_address = 0;
//		i2cData.number_values = 16;
//		//read_I2C(2);
//		I2C_RdRegLW(0x66, i2cData.rx_values, 16);
//	}

	// stop blinkm default script
	i2cData.tx_values[0] = 'o';
	I2C_WrReg(0x09, 0x00, i2cData.tx_values, 1);
	DelaySec(5);

	// bring blinkm off as the above stop script may stop at any color
	i2cData.tx_values[0] = 'n';
	i2cData.tx_values[1] = 0x00;
	i2cData.tx_values[2] = 0x00;
	i2cData.tx_values[3] = 0x00;
	I2C_WrReg(0x09, 0x00, i2cData.tx_values, 4);
	//DelaySec(5);

	// test read i2c, work fine with incr set to 0, not work with incr set to 1;
	// restart all to get work with incr set to 0 after set incr set to 1
	//i2cData.tx_values[0] = 'Z'; //'g'; //'Z'; //'a';
	//I2C_WrReg(0x09, 0x00, i2cData.tx_values, 1);
	//DelaySec(5);
	//I2C_RdReg(0x09, 0x00, i2cData.rx_values, 16, 0);

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
