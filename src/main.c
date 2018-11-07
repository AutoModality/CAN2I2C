#include "rcc.h"
#include "usart.h"
#include "i2c.h"
#include "can.h"

#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"

//uint16_t flashspeed, ledflash, flsdelay;

//uint8_t canmode = 1; //canmode = 0 loopback; canmode >= 1 normal

//int ncount = 0;
//int dcount = 0;

int main(void)
{
	rccConfiguration();
	DelayMil(10);
	i2cInit();
	canInit();
    chipReset();
	//flsdelay = 2000;
	//flashspeed = 1500;
	//ledflash = 1;

	DelaySec(1);	// Adjust this delay to the start of J130, it cannot initialize the CAN bus
					// before J130 starts.

	// stop blinkm default script
	i2cData.tx_values[0] = 'o';
	I2C_WrReg(0x09, 0x00, i2cData.tx_values, 1);
	DelaySec(5);

	// bring blinkm off as the above stop script may stop at any color
	i2cData.tx_values[0] = 'n';
	i2cData.tx_values[1] = 0x00;
	i2cData.tx_values[2] = 0xff;
	i2cData.tx_values[3] = 0x00;
	I2C_WrReg(0x09, 0x00, i2cData.tx_values, 4);
	DelaySec(5);

	while (1)
	{
		/*Status = VL53L0X_PerformSingleRangingMeasurement(pMyDevice, &RangingMeasurementData);

		if (Status == VL53L0X_ERROR_NONE)
		{

		}*/
	}

	return 0;
}
