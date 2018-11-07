/*
 * This file is part of Project CAN2UART
 *
 * Copyright (C) 2017 Auvidea GmbH
 *
 */

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "usart.h"

commStruct_t commData;
i2cStruct_t i2cData;

uint16_t adc, temp1, temp2, temperature, adc7_2, adc7_1, adc8_1, adc8_2;

char str[10];

void USART1_IRQHandler(void);

const char *commParameterStrings[] = {
    "BAUD_RATE"
};
//char usartStr[128];
volatile char t;
char rxStr[MAX_STRLEN+1];
char *tokens[260];
static int16_t cnt = 0;
int errorCode = 0;
bool echo;
bool error;

//extern uint16_t ledflash, flashspeed, flsdelay, ncount, dcount;

enum errorState errorState;

uint32_t val0, val1, val2, val3;

static inline void commWrite(commStruct_t *comm, uint8_t *data, uint8_t n) {
    int i = 0;
    while (commData.txTail != commData.txHead) {

    }
    for (i = 0; i < n; i++) {
    	commData.txBuf[commData.txHead] = data[i];
    	commData.txHead = (commData.txHead + 1) % USART1_BUF_SIZE;
    }
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

/*void USART_WriteString(const char *s) {
	uint8_t data[1];
	while(*s) {
		data[0] = *s;
		commWrite(NULL, data, 1);
		s++;
	}
}*/


void txbyte(int c) {
	commData.txBuf[commData.txHead] = c;
	commData.txHead = (commData.txHead + 1) % USART1_BUF_SIZE;
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

void printVersion(void) {
	//USART_WriteString("v ");
	//USART_WriteString(VERSION);
	//USART_WriteString("\r\n");
}

void cleanString(void) {
	int i;
	errorCode = 0; // replace errorState
	for(i=0; i<MAX_STRLEN; i++) {
		rxStr[i] = '\0';
	}
}

unsigned int hexCharToInt(char input) {
	    if (input > 47 && input < 58) {
			return input - 48;
	    } else if (input > 64 && input < 71) {
			return input - 55;
	    } else if (input > 96 && input < 103) {
			return input - 87;
	    } else {
	    	errorState = BAD_VALUE;
	    	return 99;
	    }
}

unsigned int decCharToInt(char input) {
	    if (input > 47 && input < 58) {
	    	return input - 48;
	    } else {
	    	errorState = BAD_VALUE;
	    	return 99;
	    }
}

int power10(int n)
{
    static int pow10[8] = {
        1, 10, 100, 1000, 10000,
        100000, 1000000, 10000000
    };

    return pow10[n];
}

int strHandler(int n) {
	int i = 0, j = 0, ret = 0;
	int buf[10];
	buf[0] = *tokens[n]++;
	buf[1] = *tokens[n]++;
	buf[2] = *tokens[n]++;
	buf[3] = *tokens[n]++;
	buf[4] = *tokens[n]++;
	buf[5] = *tokens[n]++;
	buf[6] = *tokens[n]++;
	buf[7] = *tokens[n]++;
	buf[8] = *tokens[n]++;
	buf[9] = *tokens[n]++;
	if (buf[0] == '0' && buf[1] == 'x') {
		for(i=2; i<10; i++) {
			if(buf[i] == 0) {
				while(i>2) {
					ret += hexCharToInt(buf[--i]) << ((j++)*4);
				}
				return ret;
			}
		}

	}
	else {

		for(i=0; i<10; i++) {
			if(buf[i] == 0) {
				while(i>0) {
					ret += decCharToInt(buf[--i]) * power10(j++);
				}
				return ret;
			}
		}
	}
	errorState = BAD_VALUE;
	return 0;
}

int debugStrHandler(int n){
	int buf[2];
	buf[0] = *tokens[n]++;
	buf[1] = *tokens[n]++;
	if (buf[0] == 'd' && buf[1] == 'd'){
		return 1;
	}
	else{
		return 0;
	}
}



int split(char *string, char *delim, char *tokens[])
{
    int count = 0;
    char *token;
    char *stringp;

    stringp = string;
    while (stringp != NULL) {
        token = strsep(&stringp, delim);
        tokens[count] = token;
        count++;
    }
    return count;
}

void rx_tty(void) {
	int count;
	if (commData.rxTail != commData.rxHead) {
		t =	commData.uartRX[commData.rxTail];
		commData.rxTail = (commData.rxTail + 1) % USART1_BUF_SIZE;
		if (t == 25) {
			echo = YES;
			//USART_WriteString("echo enabled\r\n>");
		} else if(t == 14) {
			echo = NO;
			//USART_WriteString("echo disabled\r\n");
		} else if(t == 5) {
			echo = !echo; //toggle echo
			//if(echo == NO) USART_WriteString("echo disabled\r\n");
			//else if(echo == YES) USART_WriteString("echo enabled\r\n>");
		} else if(t == 27) {
			  if(echo == 1) {
				//USART_WriteString("\r\n>");
			  }
			  cleanString();
			cnt = 0;
		} else if( (t != 13) && (t != 'Q') && (t !=0x04) && (cnt < MAX_STRLEN) ) {
			if(echo == YES) {
				//snprintf(usartStr, 3, "%c", t);
				//USART_WriteString(usartStr);
			}
			rxStr[cnt] = t;
			cnt++;
			if(t == 8 || t == 127){
				rxStr[--cnt] = '\0';
				rxStr[--cnt] = '\0';
				if(cnt<0) cnt = 0;
			}
		} else { // otherwise reset the character counter and print the received string
		  if(echo == YES) {
			 //USART_WriteString("\r\n");
		  }
		  cnt = 0;
		  count = split(rxStr, " ", tokens);
			if(strcmp(tokens[0], "v") == 0) {
				printVersion();
			} else if(strcmp(tokens[0], "w") == 0) {
				if(count < 4) {
					errorState = BAD_QUANTITY;
				} else {
					i2cData.device_address =  strHandler(1);
					i2cData.register_address = strHandler(2);
					uint8_t i, j;
					j = 0;
					for(i=3; i<count; i++) {
						i2cData.tx_values[j++] = strHandler(i);
					}
					i2cData.number_values = j;
					if(errorState == OK) {
						write_I2C();
					}
				}
			} else if(strcmp(tokens[0], "cansend") == 0) {
				if(count < 4) {
					errorState = BAD_QUANTITY;
				} else {
					uint8_t can_id = strHandler(1);
					uint8_t can_dlc = strHandler(2);
					uint8_t can_data_send[can_dlc];
					uint8_t i; //todo scope
					for(i=0; i<can_dlc; i++) {
						can_data_send[i] = strHandler(i+3);
					}
					if(errorState == OK) {
						send_can_message(can_id, can_dlc, can_data_send);
					}
				}
			} else if(strcmp(tokens[0], "candump") == 0) {
				if(count > 1) {
					errorState = BAD_QUANTITY;
				} else {
					if(errorState == OK) {
						dump_can_message();
					}
				}
			} else if(strcmp(tokens[0], "canmode") == 0) {
				if(count != 2) {
					errorState = BAD_QUANTITY;
				} else {
					if(errorState == OK) {
						if(strHandler(1) >= 1){
							//canmode=1;
							//USART_WriteString("canmode normal\r\n");
						} else {
							//canmode=0;
							//USART_WriteString("canmode loopback\r\n");
						}
					}
				}
			} else if(strcmp(tokens[0], "r") == 0){
				int mode = 0;
				i2cData.device_address =  strHandler(1);
				if(count == 2) {
					i2cData.register_address = 0x00;
					mode = 1;
					if(errorState == OK) {
						read_I2C(mode);
					}
				} else if(count == 3) {
					i2cData.register_address = strHandler(2);
					i2cData.number_values = 1;
					mode = 2;
					if(errorState == OK) {
						read_I2C(mode);
					}
				} else if(count == 4) {
					i2cData.register_address = strHandler(2);
					i2cData.number_values = strHandler(3);
					mode = 2;
					if(errorState == OK) {
						read_I2C(mode);
					}
				} else {
					errorState = BAD_QUANTITY;
				}
			} else if(strcmp(tokens[0], "led") == 0) {
				if(strcmp(tokens[1], "on") == 0) {
					if(strcmp(tokens[2], "flash") == 0) {
							//ncount = 0;
							//dcount = 0;
							//flsdelay = strHandler(4);
							//flashspeed = strHandler(3);
							//ledflash = 1;
					} else {
						GPIO_SetBits(GPIOB, GPIO_Pin_5);
						//ledflash = 0;
					}
				} else if(strcmp(tokens[1], "off") == 0) {
					GPIO_ResetBits(GPIOB, GPIO_Pin_5);
					//ledflash = 0;
				} else {
					errorState = BAD_VALUE;
				}
			} else if(strcmp(tokens[0], "l") == 0) {
				if(count == 1) {
					if(errorState == OK) {
						list_I2C();
					}
				} else {
					errorState = BAD_QUANTITY;
				}
			}
			cleanString();
			switch(errorState) {
				case OK: break;
				case UNSUPPORTED: //USART_WriteString("unsupported option\r\n"); break;
				case BAD_QUANTITY: //USART_WriteString("unsupported number of arguments\r\n"); break;
				case BAD_DEV_ADDR: //USART_WriteString("check device address\r\n"); break;
				case BAD_REG_ADDR: //USART_WriteString("check register address\r\n"); break;
				case BAD_VALUE: //USART_WriteString("bad numerical value\r\n"); break;
				case BAD_SPI_SRC: //USART_WriteString("option not supported for selected SPI Master\r\n"); break;
				default: break;
			}
			if(echo == YES) {
				//USART_WriteString(">");
		  	}
		  	errorState = OK;
		}
	}

}

void usartInit(void) {

    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(USART_CLK,ENABLE);

    GPIO_PinAFConfig(USART_GPIO_PORT, USART_RX_SOURCE, USART_AF_PORT);
    GPIO_PinAFConfig(USART_GPIO_PORT, USART_TX_SOURCE, USART_AF_PORT);

    GPIO_InitStructure.GPIO_Pin =  USART_RX_PIN | USART_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    /* Enable USART1 IRQ on RX */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // Enable USART1 Receive Interrupt
    USART_Cmd(USART1, ENABLE);
    echo = YES;
}

void USART1_IRQHandler(void)
{
	if ((USART1->ISR & USART_FLAG_RXNE) != (uint16_t)RESET) {
		commData.uartRX[commData.rxHead] = USART_ReceiveData(USART1);
		commData.rxHead = (commData.rxHead + 1) % USART1_BUF_SIZE;
	}
	if ((USART1->ISR & USART_FLAG_TXE) != (uint16_t)RESET)   {
    	if ( commData.txTail != commData.txHead ) {
			USART_SendData(USART1, commData.txBuf[commData.txTail]);
			commData.txTail = (commData.txTail + 1) % USART1_BUF_SIZE;
			if ( commData.txTail == commData.txHead)
				USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    	}
		else {
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE); //disable Transmit Data Register empty interrupt
		}
    }
}
