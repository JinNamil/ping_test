/**
******************************************************************************
* @file    ewbm_htsensor.c
* @author  BNDTEK Team
* @version V0.1
* @date    22-November-2015
* @brief   Humidity & Temperature BSP module driver.
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

#include <stdint.h>  // standard types definitions
#include <string.h>

#include "ewbm_htsensor.h"
#include "ewbm_i2c.h"

//#define HTS_DBG_ENABLE
#ifdef HTS_DBG_ENABLE
#include "ewbm_uart.h"
#define HTS_DBG(fmt,args...)        UART_Printf(fmt,##args)
#else
#define HTS_DBG(fmt,args...)
#endif

/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_HTSENSOR
  * @{
  */ 

/** @defgroup HTSENSOR_Internal_Types HTSENSOR Internal Types
  * @{
  */

/**
  * @brief  I2C handle for humidity & temperature sensor
  */
static tI2C_HANDLE  gI2c2Handle;

/**
  * @}
  */

/** @addtogroup HTSENSOR_External_Func
  * @{
  */

/** @addtogroup HTSENSOR_External_Func_G1
  *  @brief    Initialization and Deinitialization functions
  * @{
  */

/**
  * @brief  Initializes the humidity & temperature sensor according to the specified parameters
  * @param  freq: i2c frequency
  * @retval if success, return EWBMOK value
  */
int HTS_Init(eHTS_CLKFREQ freq)
{
    gI2c2Handle.channel = I2C_CHANNEL_2;
    gI2c2Handle.freq = (eI2C_CLKFREQ) freq;

    if(I2C_Init(&gI2c2Handle) != EWBMOK)
        return EWBMERR;

    return EWBMOK;
}

/**
  * @brief  DeInitializes the humidity & temperature sensor
  * @retval if success, return EWBMOK value
  */
int HTS_DeInit(void)
{
    if(I2C_DeInit(&gI2c2Handle) != EWBMOK)
        return EWBMERR;

    return EWBMOK;
}

/**
  * @}
  */

/** @addtogroup HTSENSOR_External_Func_G2
  *  @brief    humidity & temperature sensor operation functions
  * @{
  */

/**
  * @brief  Enable the humidity & temperature sensor
  * @retval if success, return EWBMOK value
  */
int HTS_Enable(void)
{
    if(I2C_MasterModeEnable(&gI2c2Handle) != EWBMOK)
        return EWBMERR;

    return EWBMOK;
}

/**
  * @brief  Get the firmware revision of the humidity & temperature sensor.
  * @param  fwRev: pointer to a get the firmware revision
  * @retval if success, return EWBMOK value
  */
int HTS_FirmwareRev(uint8_t* fwRev)
{
    tI2C_MSG sndMsg;
    tI2C_MSG recvMsg;
    uint8_t sndBuf[2];
    uint8_t recvBuf = 0;

    memset(&sndMsg, 0, sizeof(sndMsg));
    memset(&recvMsg, 0, sizeof(recvMsg));

    sndBuf[0] = READ_FIRMWARE_REVISION_1ST;
    sndBuf[1] = READ_FIRMWARE_REVISION_2ST;
    sndMsg.slaveAddr = SI7021_SLAVE_ADDR;
    sndMsg.buf = sndBuf;
    sndMsg.len = 2;

    if(I2C_MasterSend(&gI2c2Handle, &sndMsg) == EWBMOK) {
        recvMsg.slaveAddr = SI7021_SLAVE_ADDR;
        recvMsg.buf = &recvBuf;
        recvMsg.len = 1;
        I2C_MasterRecv(&gI2c2Handle, &recvMsg);
    } else {
        return EWBMERR;
    }

    *fwRev = recvBuf;

    return EWBMOK;
}

/**
  * @brief  Get the serial number of the humidity & temperature sensor
  * @param  serialNum: pointer to a get the serial number
  * @retval if success, return EWBMOK value
  */
int HTS_CheckSN(uint8_t* serialNum)
{
    tI2C_MSG sndMsg;
    tI2C_MSG recvMsg;
    uint8_t sndBuf[2];
    uint8_t recvBuf = 0;

    memset(&sndMsg, 0, sizeof(tI2C_MSG));
    memset(&recvMsg, 0, sizeof(tI2C_MSG));

    sndBuf[0] = READ_ELECTRONIC_ID_2ST_1_BYTE;
    sndBuf[1] = READ_ELECTRONIC_ID_2ST_2_BYTE;
    sndMsg.slaveAddr = SI7021_SLAVE_ADDR;
    sndMsg.buf = sndBuf;
    sndMsg.len = 2;

    if(I2C_MasterSend(&gI2c2Handle, &sndMsg) == EWBMOK) {
        recvMsg.slaveAddr = SI7021_SLAVE_ADDR;
        recvMsg.buf = &recvBuf;
        recvMsg.len = 1;
        I2C_MasterRecv(&gI2c2Handle, &recvMsg);
    } else {
        return EWBMERR;
    }

    *serialNum = recvBuf;

    return EWBMOK;
}

/**
  * @brief  Reads the current temperature
  * @param  mode: temperature measure mode of humidity & temperature sensor selection
  * @param  buf: pointer to a get the receiver buffer for temperature mode
  * @param  readTM: pointer to a get the temperature measurement value
  * @retval if success, return EWBMOK value
  */
int HTS_ReadTemp(uint8_t mode, uint8_t* buf, uint16_t* readTM)
{
    tI2C_MSG sndMsg;
    tI2C_MSG recvMsg;
    uint8_t sndBuf;
    uint8_t recvBuf[2];

    memset(&sndMsg, 0, sizeof(tI2C_MSG));
    memset(&recvMsg, 0, sizeof(tI2C_MSG));

    sndBuf = mode;
    sndMsg.slaveAddr = SI7021_SLAVE_ADDR;
    sndMsg.buf = &sndBuf;
    sndMsg.len = 1;

    if(I2C_MasterSend(&gI2c2Handle, &sndMsg) == EWBMOK) {
        recvMsg.slaveAddr = SI7021_SLAVE_ADDR;
        recvMsg.buf = recvBuf;
        recvMsg.len = 2;
        I2C_MasterRecv(&gI2c2Handle, &recvMsg);
    } else {
        return EWBMERR;
    }

    buf[0] = recvMsg.buf[0];
    buf[1] = recvMsg.buf[1];
    *readTM = (uint16_t)((recvMsg.buf[0]<<8)|(recvMsg.buf[1]));

    return EWBMOK;
}


/**
  * @brief  Reads the current humidity
  * @param  mode: humidity measure mode of humidity & temperature sensor selection
  * @param  buf: pointer to a get the receiver buffer for humidity mode
  * @param  readHM: pointer to a get the humidity measure value
  * @retval if success, return EWBMOK value
  */
int HTS_ReadHumidity(uint8_t mode, uint8_t* buf, uint16_t* readHM)
{
    tI2C_MSG sndMsg;
    tI2C_MSG recvMsg;
    uint8_t sndBuf;
    uint8_t recvBuf[2];

    memset(&sndMsg, 0, sizeof(tI2C_MSG));
    memset(&recvMsg, 0, sizeof(tI2C_MSG));

    sndBuf = mode;
    sndMsg.slaveAddr = SI7021_SLAVE_ADDR;
    sndMsg.buf = &sndBuf;
    sndMsg.len = 1;

    if(I2C_MasterSend(&gI2c2Handle, &sndMsg) == EWBMOK) {
        recvMsg.slaveAddr = SI7021_SLAVE_ADDR;
        recvMsg.buf = recvBuf;
        recvMsg.len = 2;
        I2C_MasterRecv(&gI2c2Handle, &recvMsg);
    } else {
        return EWBMERR;
    }

    buf[0] = recvMsg.buf[0];
    buf[1] = recvMsg.buf[1];

    *readHM = (uint16_t)((recvMsg.buf[0]<<8)|(recvMsg.buf[1]));

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

