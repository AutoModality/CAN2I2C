/*
 * This file is part of Project CAN2UART
 *
 * Copyright (C) 2017 Auvidea GmbH
 *
 */

#ifndef USART_H_
#define USART_H_

#include <stdint.h>
#include "rcc.h"
#include "i2c.h"
#include "can.h"

#define USART_CLK           RCC_APB2Periph_USART1
#define USART_RX_PIN		GPIO_Pin_10
#define USART_TX_PIN		GPIO_Pin_9
#define USART_GPIO_PORT		GPIOA
#define USART_RX_SOURCE		GPIO_PinSource10
#define USART_TX_SOURCE		GPIO_PinSource9
#define USART_AF_PORT       GPIO_AF_1
#define USART_BAUDRATE		115200
#define USART1_BUF_SIZE		128

#define MAX_STRLEN 1291

extern char usartStr[128];

typedef enum { NO=0, YES } bool;

typedef struct {
   // canNode_t *node;
    uint32_t baud;
    uint8_t paramName[16];
    uint8_t state;
    uint8_t emptyInterrupts;
    uint8_t sendNowToCan;
    uint32_t timeNextCanSend;
    volatile uint8_t uartRX[USART1_BUF_SIZE];
    volatile uint8_t rxBuf[8];
    volatile uint8_t txBuf[USART1_BUF_SIZE];
    uint8_t rxHead, rxTail;
    uint8_t txHead, txTail;
} commStruct_t;

extern commStruct_t commData;

enum errorState {
    OK = 0,
    UNSUPPORTED,
    BAD_VALUE,
    BAD_QUANTITY,
    BAD_DEV_ADDR,
    BAD_REG_ADDR,
    BAD_SPI_SRC,
};

extern enum errorState errorState;

extern uint8_t canmode;

extern char usartStr[128];
extern volatile char t;

extern void USART_WriteString(const char *s);
extern void txbyte(int c);
extern void rx_tty(void);
extern void usartInit(void);

extern void write_I2C(void);
extern void read_I2C(int mode);
extern void list_I2C(void);

#endif /* USART_H_ */
