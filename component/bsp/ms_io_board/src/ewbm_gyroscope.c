/**
******************************************************************************
* @file    ewbm_gyroscope.c
* @author  eWBM
* @version V0.1
* @date    29-07-2016
* @brief   Gyroscope Sensor module
******************************************************************************
* @attention
*
* Copyright(c) 2015 ~ 2020 eWBM Korea , Ltd.  
* All rights reserved.
* This software contains confidential information of eWBM Korea Co.,Ltd.
* and unauthorized distribution of this software, or any portion of it, are
* prohibited.
*
******************************************************************************
*/

#include <string.h>
#include "ewbm_i2c.h"

#include "ewbm_gyroscope.h"

//#define GYRO_DBG_ENABLE
#ifdef GYRO_DBG_ENABLE
#include "ewbm_uart.h"
#define GYRO_DBG(fmt,args...)        UART_Printf(fmt,##args)
#else
#define GYRO_DBG(fmt,args...)
#endif

/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_GYROSCOPE
  * @{
  */ 

/** @defgroup GYRO_Internal_Types GYROSCOPE Exported Types
  * @{
  */

static tI2C_HANDLE  gI2CHandle;

/**
  * @}
  */

/** @addtogroup GYRO_External_Func
  * @{
  */

/** @addtogroup GYRO_External_Func_G1
  * @{
  */

/**
  * @brief  Initializes the Gyrocope sensor
  * @retval  if success, return EWBMOK value
  */
int GYRO_Init(void)
{
    gI2CHandle.channel = I2C_CHANNEL_2;
    gI2CHandle.freq = I2C_FREQ_100KHZ;

    if(I2C_Init(&gI2CHandle) != EWBMOK)
        return EWBMERR;

    if(I2C_MasterModeEnable(&gI2CHandle) != EWBMOK)
        return EWBMERR;

    return EWBMOK;
}

/**
  * @brief  DeInitializes the Gyrocope sensor.
  * @retval  if success, return EWBMOK value
  */
int GYRO_DeInit(void)
{
    I2C_MasterModeDisable(&gI2CHandle);
    I2C_DeInit(&gI2CHandle);
    return EWBMOK;
}

/**
  * @}
  */

/** @addtogroup GYRO_External_Func_G2
  * @{
  */


/**
  * @brief  Get device id of the gyrocope sensor.
  * @param  deviceID: pointer to a get the device id
  * @retval  if success, return EWBMOK value
  */
int GYRO_GetDeviceID(uint8_t* deviceID)
{
    tI2C_MSG sndMsg;
    tI2C_MSG recvMsg;
    uint8_t  sndbuf;
    uint8_t  recvbuf = 0;

    memset(&sndMsg, 0, sizeof(tI2C_MSG));
    memset(&recvMsg, 0, sizeof(tI2C_MSG));

    sndbuf = MPU6050_REG_WHO_AM_I;

    sndMsg.slaveAddr = MPU6050_I2C_ADDRESS;
    sndMsg.buf = &sndbuf;
    sndMsg.len = 1;

    if(I2C_MasterSend(&gI2CHandle, &sndMsg) == EWBMOK) {
        recvMsg.slaveAddr = MPU6050_I2C_ADDRESS;
        recvMsg.buf = &recvbuf;
        recvMsg.len = 1;
       
        I2C_MasterRecv(&gI2CHandle, &recvMsg);
    } else {
        return EWBMERR;
    }

    GYRO_DBG("L3GD20H Device ID:(0x%x) \r\n", recvbuf);

    *deviceID = recvbuf;
    
    return EWBMOK;
}
/**
  * @brief  Control register setting to enalbe a gyrocope sensor.
  * @param  mode:  operation mode of gyroscope sensor.
  * @param  xyz_en: enable a x-axis, y-axis and z-axis of gyroscope sensor.
  * @retval  if success, return EWBMOK value
  */
int GYRO_Control(uint8_t reg, uint8_t data)
{
    tI2C_MSG sndMsg;
    tI2C_MSG recvMsg;
    uint8_t  sndbuf[2];

    memset(&sndMsg, 0, sizeof(tI2C_MSG));
    memset(&recvMsg, 0, sizeof(tI2C_MSG));

    sndbuf[0] = reg;
    sndbuf[1] = data;

    sndMsg.slaveAddr = MPU6050_I2C_ADDRESS;
    sndMsg.buf = sndbuf;
    sndMsg.len = 2;

    if(I2C_MasterSend(&gI2CHandle, &sndMsg) != EWBMOK) {
        GYRO_DBG("L3GD20H CTRL1 write fail \r\n");
        return EWBMERR;
    }

    return EWBMOK;
}


//uint8_t swap;
//#define SWAP(x,y) swap = x; x = y; y = swap

/**
  * @brief  Read a x-axis, y-axis and z-axix accelerator data of the gyrocope sensor.
  * @param  x: pointer to a get the x-axis
  * @param  y: pointer to a get the y-axis
  * @param  z: pointer to a get the z-axis
  * @retval  if success, return EWBMOK value
  */
int GYRO_ReadXYZ(float* x, float* y, float* z)
{
    tI2C_MSG sndMsg;
    tI2C_MSG recvMsg;
    uint8_t  sndbuf = 0;
    uint8_t  recvbuf = 0;
    uint8_t  tmpctrl4reg = 0;
    uint8_t  tmpbuffer[6]={0};
    int      i;

    memset(&sndMsg, 0, sizeof(tI2C_MSG));
    memset(&recvMsg, 0, sizeof(tI2C_MSG));

    // check ctrl4 value for sensitivity
    sndbuf = MPU6050_REG_GYRO_XOUT_H;
    sndMsg.slaveAddr = MPU6050_I2C_ADDRESS;
    sndMsg.buf = &sndbuf;
    sndMsg.len = 1;

    if(I2C_MasterSend(&gI2CHandle, &sndMsg) == EWBMOK) {
        recvMsg.slaveAddr = MPU6050_I2C_ADDRESS;
        recvMsg.buf = &tmpctrl4reg;
        recvMsg.len = 1;
        I2C_MasterRecv(&gI2CHandle, &recvMsg);
        GYRO_DBG("--FULLSCALE:(0x%x) \r\n", tmpctrl4reg);
    } else {
        return EWBMERR;
    }

    // read x, y, z value and save data to tmpbuffer[]
    for(i=0; i<6; i++) {
        sndbuf = MPU6050_REG_GYRO_XOUT_H+i;
        sndMsg.slaveAddr = MPU6050_I2C_ADDRESS;
        sndMsg.buf = &sndbuf;
        sndMsg.len = 1;

        if(I2C_MasterSend(&gI2CHandle, &sndMsg) == EWBMOK) {
            recvMsg.slaveAddr = MPU6050_I2C_ADDRESS;
            recvMsg.buf = &recvbuf;
            recvMsg.len = 1;
            I2C_MasterRecv(&gI2CHandle, &recvMsg);
            tmpbuffer[i] = recvbuf;
            GYRO_DBG("[%d]:(0x%x) \r\n", i, recvbuf);
        } else {
            return EWBMERR;
        }
    }

    // Swap all high and low bytes.
    // After this, the registers values are swapped,
    // so the structure name like x_accel_l does no
    // longer contain the lower byte.
//    for(i=0; i<14; i+=2) {
//        SWAP (tmpbuffer[i], tmpbuffer[i+1]);
//    }

   *x = tmpbuffer[1]<<8|tmpbuffer[0];
   *y = tmpbuffer[3]<<8|tmpbuffer[2];
   *z = tmpbuffer[5]<<8|tmpbuffer[4];

    return EWBMOK;
}


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */



