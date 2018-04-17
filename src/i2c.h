/*
 * This file is part of Project CAN2UART
 *
 * Copyright (C) 2017 Auvidea GmbH
 *
 */

#include <stdint.h>

#ifndef I2C_H_
#define I2C_H_

//see https://github.com/pyrohaz/STM32F0-HMC5883_Barebones/blob/master/main.c
#define EP9351_SDA GPIO_Pin_11
#define EP9351_SCL GPIO_Pin_10
#define EP9351_GPIO GPIOB
#define EP9351_SDA_PS GPIO_PinSource11
#define EP9351_SCL_PS GPIO_PinSource10
#define EP9351_PIN_AF GPIO_AF_1
#define EP9351_I2C I2C1
#define I2C_TIMEOUT 1000

#define HDMI_IN			((uint8_t)0x3C)
#define EP9351_ADDR		((uint8_t)0x3C)

typedef struct {
    uint8_t device_address;
    uint8_t register_address;
    uint8_t number_values;
    uint8_t tx_values[256];
    uint8_t rx_values[256];
} i2cStruct_t;
extern i2cStruct_t i2cData;

enum de_mode { DE_ON = 0 , DE_OFF };
enum de_mode de_mode;
extern void chipReset(void);
extern void i2cInit(void);
extern void bbInit(void);
extern void i2c_wr_byte(uint8_t Addr, uint8_t Reg, uint8_t DataByte);
extern uint8_t i2c_rd_byte(uint8_t Addr, uint8_t Reg);
extern uint16_t I2C_WrReg(uint8_t Addr, uint8_t Reg, const uint8_t *Data, uint16_t DCnt);
extern uint16_t I2C_RdReg(uint8_t Addr, uint8_t Reg, uint8_t *Data, uint8_t DCnt, uint8_t incr);
extern uint16_t I2C_RdRegLW(uint8_t Addr, uint8_t *Data, uint8_t DCnt);
extern uint16_t I2C_listDev(uint8_t Addr);
extern void init_EP9351(void);
extern void rd_avi(void);
extern void pr_avi(void);
extern void init_edid(void);

#endif /* I2C_H_ */
