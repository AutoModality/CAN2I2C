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

#include "stm32f0xx_can.h"

#define LW20_I2C_ADDR 0x66

uint16_t ledflash;

uint8_t canmode = 1; //canmode = 0 loopback; canmode >= 1 normal

extern volatile uint32_t Milliseconds;
extern volatile uint32_t Seconds;

int main(void)
{
	rccConfiguration();
	DelayMil(10);
	i2cInit();
	canInit();
    chipReset();

	DelaySec(1);

	// stop blinkm default script
	i2cData.tx_values[0] = 'o';
	I2C_WrReg(0x09, 0x00, i2cData.tx_values, 1);
	//DelaySec(1);

	// bring blinkm off as the above stop script may stop at any color
	i2cData.tx_values[0] = 'n';
	i2cData.tx_values[1] = 0x00;
	i2cData.tx_values[2] = 0x00;
	i2cData.tx_values[3] = 0x00;
	I2C_WrReg(0x09, 0x00, i2cData.tx_values, 4);
	DelaySec(1);

	// set blikm to red
	i2cData.tx_values[0] = 'n';
	i2cData.tx_values[1] = 0xff;
	i2cData.tx_values[2] = 0x00;
	i2cData.tx_values[3] = 0x00;
	I2C_WrReg(0x09, 0x00, i2cData.tx_values, 4);
	DelaySec(1);

	// set blikm to green
	i2cData.tx_values[0] = 'n';
	i2cData.tx_values[1] = 0x00;
	i2cData.tx_values[2] = 0xff;
	i2cData.tx_values[3] = 0x00;
	I2C_WrReg(0x09, 0x00, i2cData.tx_values, 4);
	DelaySec(1);

	// set blikm to blue
	i2cData.tx_values[0] = 'n';
	i2cData.tx_values[1] = 0x00;
	i2cData.tx_values[2] = 0x00;
	i2cData.tx_values[3] = 0xff;
	I2C_WrReg(0x09, 0x00, i2cData.tx_values, 4);
	DelaySec(1);

	// turn off blikm
	i2cData.tx_values[0] = 'n';
	i2cData.tx_values[1] = 0x00;
	i2cData.tx_values[2] = 0x00;
	i2cData.tx_values[3] = 0x00;
	I2C_WrReg(0x09, 0x00, i2cData.tx_values, 4);
	DelaySec(1);

	// Independent Watchdog
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_4);
	while (IWDG_GetFlagStatus(IWDG_FLAG_PVU) == RESET) ;
	IWDG_SetReload(0x3ff);
	while (IWDG_GetFlagStatus(IWDG_FLAG_RVU) == RESET) ;
	IWDG_Enable();

	uint32_t Milliseconds_old = Milliseconds;

	while (1)
	{
		if (Milliseconds - Milliseconds_old > 1000)
		{
			if (ledflash == 1)
			{
				GPIO_SetBits(GPIOB, GPIO_Pin_5);
				ledflash = 0;
			}
			else if(ledflash == 0)
			{
				GPIO_ResetBits(GPIOB, GPIO_Pin_5);
				ledflash = 1;
			}

			Milliseconds_old = Milliseconds;
		}

		// ReadLW20 altimeter
		i2cData.tx_values[0] = '?';
		i2cData.tx_values[1] = 'L';
		i2cData.tx_values[2] = 'D';
		i2cData.tx_values[3] = '\r';
		i2cData.tx_values[4] = '\n';

		// send LW20 reading to CAN
		uint8_t i = 0;
		uint8_t j = 0;
		uint8_t valid_data = 0;

		CanTxMsg can_tx_msg;
		can_tx_msg.RTR = CAN_RTR_DATA;
		can_tx_msg.IDE = CAN_ID_STD;

		// arrange the lw20 reading into CAN frame
		can_tx_msg.StdId = 0x01;
		can_tx_msg.Data[0] = LW20_I2C_ADDR;
		// write lw20 command to i2c
		if(!I2C_WrReg(LW20_I2C_ADDR, 0x00, i2cData.tx_values, 0x05))
		{
			// read lw20 from i2c
			//ret = I2C_RdReg(can_rx_msg.Data[0], 0x00, i2c_rx_msg, 1, 1);
			if(!I2C_RdRegLW(LW20_I2C_ADDR, i2cData.rx_values, 16))
			{
				j = 1;
				for (i = 0; i < 16; i++)
				{
					if (!valid_data)
					{
						//if (i2c_rx_msg[i] == ':')
						if (i2cData.rx_values[i] == ':')
						{
							valid_data = 1;
						}
					}
					else
					{
						//if (i2c_rx_msg[i] != '\r') // it is not as said in spec, end with '\r\n'
						//if (i2c_rx_msg[i] != '\r' && i2c_rx_msg[i] != '\n' && i2c_rx_msg[i] != ' ' && i2c_rx_msg[i] != '\t')
						if ((i2cData.rx_values[i] >= 48 && i2cData.rx_values[i] <= 57)
								|| (i2cData.rx_values[i] >= 65 && i2cData.rx_values[i] <= 90)
								|| (i2cData.rx_values[i] >= 97 && i2cData.rx_values[i] <= 122)
								|| i2cData.rx_values[i] == 46)
						{
							can_tx_msg.Data[j] = i2cData.rx_values[i];
							j++;
						}
						else
						{
							break;
						}
					}
				}
				can_tx_msg.DLC = j;
			}
			else
			{
				can_tx_msg.Data[1] = '-';
				can_tx_msg.Data[2] = '1';
				can_tx_msg.DLC = 3;

				//i2cInit();
			}
		}
		else
		{
			can_tx_msg.Data[1] = '-';
			can_tx_msg.Data[2] = '1';
			can_tx_msg.DLC = 3;

			//i2cInit();
		}
		//i2cInit();
		I2C_SoftwareResetCmd(EP9351_I2C); // reset (45hz without heartbeat) is faster than re-init (43hz without heartbeat)

		// Send lw20 reading to TX2 on CAN bus
		CAN_Transmit(CAN, &can_tx_msg);
		DelayMil(25); // keep this delay bigger than TX2 timer for LW20, it seems some of the
					  // I2C and CAN hardware operation disable the interrupts

		IWDG_ReloadCounter();
	} //while

	return 0;
}
