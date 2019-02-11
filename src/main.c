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

//uint16_t flashspeed, flsdelay;
uint16_t ledflash;
//uint8_t watchdog = 0;

uint8_t canmode = 1; //canmode = 0 loopback; canmode >= 1 normal

//int ncount = 0;
//int dcount = 0;

extern volatile uint32_t Milliseconds;
extern volatile uint32_t Seconds;

//CanRxMsg can_rx_msg;
//uint8_t CAN_received = 0;

//void WWDG_Set_Counter(uint8_t cnt)
//{
//	WWDG_Enable(cnt);
//}

//void TIM2_IRQHandler(void)
//{}

//void WWDG_IRQHandler(void)
//{
//	WWDG_Set_Counter(0x7f);
//	WWDG_ClearFlag();
//}

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
	//DelaySec(1);

	// bring blinkm off as the above stop script may stop at any color
	i2cData.tx_values[0] = 'n';
	i2cData.tx_values[1] = 0x00;
	i2cData.tx_values[2] = 0x00;
	i2cData.tx_values[3] = 0x00;
	I2C_WrReg(0x09, 0x00, i2cData.tx_values, 4);
	//DelaySec(1);

	// set blikm to red
	//i2cData.tx_values[0] = 'n';
	//i2cData.tx_values[1] = 0xff;
	//i2cData.tx_values[2] = 0x00;
	//i2cData.tx_values[3] = 0x00;
	//I2C_WrReg(0x09, 0x00, i2cData.tx_values, 4);
	//DelaySec(1);

	// set blikm to green
	//i2cData.tx_values[0] = 'n';
	//i2cData.tx_values[1] = 0x00;
	//i2cData.tx_values[2] = 0xff;
	//i2cData.tx_values[3] = 0x00;
	//I2C_WrReg(0x09, 0x00, i2cData.tx_values, 4);
	//DelaySec(1);

	// set blikm to blue
	i2cData.tx_values[0] = 'n';
	i2cData.tx_values[1] = 0x00;
	i2cData.tx_values[2] = 0x00;
	i2cData.tx_values[3] = 0xff;
	I2C_WrReg(0x09, 0x00, i2cData.tx_values, 4);
	//DelaySec(1);

	// turn off blikm
	//i2cData.tx_values[0] = 'n';
	//i2cData.tx_values[1] = 0x00;
	//i2cData.tx_values[2] = 0x00;
	//i2cData.tx_values[3] = 0x00;
	//I2C_WrReg(0x09, 0x00, i2cData.tx_values, 4);
	//DelaySec(1);

	// test read i2c, work fine with incr set to 0, not work with incr set to 1;
	// restart all to get work with incr set to 0 after set incr set to 1
	//i2cData.tx_values[0] = 'Z'; //'g'; //'Z'; //'a';
	//I2C_WrReg(0x09, 0x00, i2cData.tx_values, 1);
	//DelaySec(5);
	//I2C_RdReg(0x09, 0x00, i2cData.rx_values, 16, 0);

	// Windows Watchdog
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	//WWDG_SetPrescaler(WWDG_Prescaler_1);
	//WWDG_SetWindowValue(0x5f);
	//WWDG_Enable(0x7f);
	//WWDG_ClearFlag();
	//NVIC_InitTypeDef NVIC_InitStructure;
	//NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
	//NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//NVIC_Init(&NVIC_InitStructure);
	//WWDG_EnableIT();

	// Independent Watchdog
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_4);
	while (IWDG_GetFlagStatus(IWDG_FLAG_PVU) == RESET) ;
	IWDG_SetReload(0xfff);
	while (IWDG_GetFlagStatus(IWDG_FLAG_RVU) == RESET) ;
	//IWDG_ReloadCounter();
	IWDG_Enable();

	uint32_t Milliseconds_old = Milliseconds;
	uint32_t Milliseconds_old_hb = Milliseconds;
	uint32_t count_lw20_reading = 0;

	while (1)
	{
//		if(ledflash == 1)
//		{
//			if(ncount != flashspeed)
//			{
//				GPIO_SetBits(GPIOB, GPIO_Pin_5);
//				ncount++;
//			}
//			if(ncount == flashspeed)
//			{
//					dcount++;
//					GPIO_ResetBits(GPIOB, GPIO_Pin_5);
//			}
//			if(dcount == flsdelay)
//			{
//				ncount = 0;
//				dcount = 0;
//			}
//		}
//		else if(ledflash == 0)
//		{
//			ncount = 0;
//			dcount = 0;
//		}

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
			//if(watchdog)
			//{
			//	Reset_Handler();
			//}
			//IWDG_ReloadCounter();
		}
		//WWDG_SetCounter(0x7f);

		//if(CAN_received)
		if(CAN_MessagePending(CAN, CAN_FIFO0) > 0)
		{
			CanRxMsg can_rx_msg;

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

			switch (can_rx_msg.StdId)
			{
				case 0x00: //BlinkM LED
				{
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
							for (uint8_t i = 0; i < can_rx_msg.DLC - 1; i++)
							{
								//i2c_tx_msg[i] = can_rx_msg.Data[i+1];
								i2cData.tx_values[i] = can_rx_msg.Data[i+1];
							}
							I2C_WrReg(can_rx_msg.Data[0], 0x00, i2cData.tx_values, can_rx_msg.DLC - 1);
							break;
						case 'a':
						case 'Z':
						case 'R':
						case 'g':
							break;
					}
					break;
				}
				case 0x01: // LW20 altimeter
				{
					//watchdog = 1;
					//WWDG_SetCounter(0x7f);

					uint8_t i = 0;
					uint8_t j = 0;
					uint8_t TransmitMailbox = 0;
					uint8_t valid_data = 0;

					CanTxMsg can_tx_msg;
					can_tx_msg.RTR = CAN_RTR_DATA;
					can_tx_msg.IDE = CAN_ID_STD;

					// arrange the message to write to i2c
					for (i = 0; i < can_rx_msg.DLC - 1; i++)
					{
						//i2c_tx_msg[i] = can_rx_msg.Data[i+1];
						i2cData.tx_values[i] = can_rx_msg.Data[i+1];
					}
					i2cData.tx_values[i++] = '\r';
					i2cData.tx_values[i] = '\n';

					// write lw20 command to i2c
					I2C_WrReg(can_rx_msg.Data[0], 0x00, i2cData.tx_values, can_rx_msg.DLC + 1);

					// read lw20 from i2c
					//ret = I2C_RdReg(can_rx_msg.Data[0], 0x00, i2c_rx_msg, 1, 1);
					I2C_RdRegLW(can_rx_msg.Data[0], i2cData.rx_values, 16);

					//watchdog = 0;

					// arrange the lw20 reading into CAN frame
					can_tx_msg.StdId = can_rx_msg.StdId;
					can_tx_msg.Data[0] = can_rx_msg.Data[0];
					j = 1;
					for (i = 0; i < 32; i++)
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

					// Send lw20 reading to TX2 on CAN bus
					TransmitMailbox = CAN_Transmit(CAN, &can_tx_msg);
					i = 0;
					while((CAN_TransmitStatus(CAN, TransmitMailbox)  !=  CANTXOK) && (i  <=  0xFFFF))
					{
					  i++;
					}
					i = 0;
					while((CAN_MessagePending(CAN, CAN_FIFO0) < 1) && (i  <=  0xFFFF))
					{
					  i++;
					}

					count_lw20_reading++;
					//IWDG_ReloadCounter();
					break;
				}
				case 0x05:
				{
					uint32_t hb = 0;
					if(Milliseconds >= Milliseconds_old_hb)
					{
						hb = Milliseconds - Milliseconds_old_hb;
					}
					else
					{
						hb = 0xffffffff - Milliseconds_old_hb + Milliseconds;
					}
					Milliseconds_old_hb = Milliseconds;
					//hb = Milliseconds;

					uint8_t i = 0;
					uint8_t TransmitMailbox = 0;

					CanTxMsg can_tx_msg;
					can_tx_msg.RTR = CAN_RTR_DATA;
					can_tx_msg.IDE = CAN_ID_STD;
					can_tx_msg.StdId = can_rx_msg.StdId;
					can_tx_msg.DLC = 8;
					can_tx_msg.Data[0] = hb & 0xff;
					can_tx_msg.Data[1] = (hb >> 8) & 0xff;
					can_tx_msg.Data[2] = (hb >> 16) & 0xff;
					can_tx_msg.Data[3] = (hb >> 24) & 0xff;
					can_tx_msg.Data[4] = count_lw20_reading & 0xff;
					can_tx_msg.Data[5] = (count_lw20_reading >> 8) & 0xff;
					can_tx_msg.Data[6] = (count_lw20_reading >> 16) & 0xff;
					can_tx_msg.Data[7] = (count_lw20_reading >> 24) & 0xff;

					// Send lw20 reading to TX2 on CAN bus
					TransmitMailbox = CAN_Transmit(CAN, &can_tx_msg);
					i = 0;
					while((CAN_TransmitStatus(CAN, TransmitMailbox)  !=  CANTXOK) && (i  <=  0xFFFF))
					{
					  i++;
					}
					i = 0;
					while((CAN_MessagePending(CAN, CAN_FIFO0) < 1) && (i  <=  0xFFFF))
					{
					  i++;
					}

					break;
				}
				default:
				{
					break;
				}
			}//switch on CAN id

			//CAN_received = 0;
		}//if(CAN_received)

		IWDG_ReloadCounter();
	} //while

	return 0;
}
