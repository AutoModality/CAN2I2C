#include <stdio.h>
#include <stdlib.h>
//#include <sys/ioctl.h>
#include <fcntl.h>
#include <time.h>
//#include <linux/i2c-dev.h>
#include <unistd.h>
#include <string.h>
#include "vl53l0x_platform.h"
#include "vl53l0x_api.h"
#include "i2c.h"
#include "rcc.h"

static int i2c_write(uint8_t I2CDevAddr, uint8_t cmd, uint8_t * data, uint8_t len)
{
    uint8_t * buf = malloc(len+1);

    buf[0] = cmd;
    memcpy(buf+1, data, len);

    if (I2C_WrReg(I2CDevAddr, 0x00, buf, len+1))
    {
        //printf("Failed to write to the i2c bus.\n");
        free(buf);
        return VL53L0X_ERROR_CONTROL_INTERFACE;
    }
    free(buf);

    return VL53L0X_ERROR_NONE;
}

static int i2c_read(uint8_t I2CDevAddr, uint8_t cmd, uint8_t * data, uint8_t len){

    if (I2C_WrReg(I2CDevAddr, 0x00, &cmd, 1))
    {
        //printf("Failed to write to the i2c bus.\n");
        return VL53L0X_ERROR_CONTROL_INTERFACE;
    }

    I2C_RdRegLW(I2CDevAddr, data, len);
    //I2C_RdReg(I2CDevAddr, 0x00, data, len, 0);
    //if (I2C_RdReg(I2CDevAddr, 0x00, data, len, 0))
    //{
    //    printf("Failed to read from the i2c bus.\n");
    //    return VL53L0X_ERROR_CONTROL_INTERFACE;
    //}

    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_LockSequenceAccess(VL53L0X_DEV Dev){
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    return Status;
}

VL53L0X_Error VL53L0X_UnlockSequenceAccess(VL53L0X_DEV Dev){
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    return Status;
}

VL53L0X_Error VL53L0X_WriteMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata, uint32_t count){
    return i2c_write(Dev->I2cDevAddr, index, pdata, count);
}

VL53L0X_Error VL53L0X_ReadMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata, uint32_t count){
    return i2c_read(Dev->I2cDevAddr, index, pdata, count);
}

VL53L0X_Error VL53L0X_WrByte(VL53L0X_DEV Dev, uint8_t index, uint8_t data){
	return i2c_write(Dev->I2cDevAddr, index, &data, 1);
}

VL53L0X_Error VL53L0X_WrWord(VL53L0X_DEV Dev, uint8_t index, uint16_t data){
    uint8_t buf[4];
    buf[1] = data>>0&0xFF;
    buf[0] = data>>8&0xFF;
    return i2c_write(Dev->I2cDevAddr, index, buf, 2);
}

VL53L0X_Error VL53L0X_WrDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t data){
    uint8_t buf[4];
    buf[3] = data>>0&0xFF;
    buf[2] = data>>8&0xFF;
    buf[1] = data>>16&0xFF;
    buf[0] = data>>24&0xFF;
    return i2c_write(Dev->I2cDevAddr, index, buf, 4);
}

VL53L0X_Error VL53L0X_UpdateByte(VL53L0X_DEV Dev, uint8_t index, uint8_t AndData, uint8_t OrData){

    int32_t status_int;
    uint8_t data;

    status_int = i2c_read(Dev->I2cDevAddr, index, &data, 1);

    if (status_int != 0){
        return  status_int;
    }

    data = (data & AndData) | OrData;
    return i2c_write(Dev->I2cDevAddr, index, &data, 1);
}

VL53L0X_Error VL53L0X_RdByte(VL53L0X_DEV Dev, uint8_t index, uint8_t *data){
    uint8_t tmp = 0;
    int ret = i2c_read(Dev->I2cDevAddr, index, &tmp, 1);
    *data = tmp;
    // printf("%u\n", tmp);
    return ret;
}

VL53L0X_Error VL53L0X_RdWord(VL53L0X_DEV Dev, uint8_t index, uint16_t *data){
    uint8_t buf[2];
    int ret = i2c_read(Dev->I2cDevAddr, index, buf, 2);
    uint16_t tmp = 0;
    tmp |= buf[1]<<0;
    tmp |= buf[0]<<8;
    // printf("%u\n", tmp);
    *data = tmp;
    return ret;
}

VL53L0X_Error  VL53L0X_RdDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t *data){
    uint8_t buf[4];
    int ret = i2c_read(Dev->I2cDevAddr, index, buf, 4);
    uint32_t tmp = 0;
    tmp |= buf[3]<<0;
    tmp |= buf[2]<<8;
    tmp |= buf[1]<<16;
    tmp |= buf[0]<<24;
    *data = tmp;
    // printf("%zu\n", tmp);
    return ret;
}

VL53L0X_Error VL53L0X_PollingDelay(VL53L0X_DEV Dev){
    //usleep(5000);
	DelayMil(5); //delay 5ms
    return VL53L0X_ERROR_NONE;
}
