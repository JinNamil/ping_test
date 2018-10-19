/**
******************************************************************************
* @file    ewbm_ambientlight.c
* @author  BNDTEK Team
* @version V0.1
* @date    22-November-2015
* @brief   Ambient Light BSP module driver.
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

#include "ewbm_delay.h"
#include "ewbm_light.h"
#include "ewbm_i2c.h"


/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_AMBIENTLIGHT
  * @{
  */

/** @defgroup AMBIENTLIGHT_Internal_Types AMBIENTLIGHT Internal Types
  * @{
  */

/**
  * @brief  I2C handle for ambient light sensor
  */
static tI2C_HANDLE gI2CHandle;

/**
  * @}
  */

/** @addtogroup AMBIENTLIGHT_External_Func
  * @{
  */

/** @addtogroup AMBIENTLIGHT_External_Func_G1
  *  @brief    Initialization and Deinitialization functions
  * @{
  */

/**
  * @brief  Initializes the ambient light sensor according to the specified parameters
  * @param  freq: i2c frequency
  * @retval if success, return EWBMOK value
  */
int ALS_Init(eLIGHT_CLKFREQ freq)
{
    gI2CHandle.channel = I2C_CHANNEL_2;
    gI2CHandle.freq = (eI2C_CLKFREQ) freq;

    if(I2C_Init(&gI2CHandle) != EWBMOK)
        return EWBMERR;

    return EWBMOK;
}

/**
  * @brief  DeInitializes the ambient light sensor
  * @retval if success, return EWBMOK value
  */
int ALS_DeInit(void)
{
    if(I2C_DeInit(&gI2CHandle) != EWBMOK)
        return EWBMERR;

    return EWBMOK;
}

/**
  * @}
  */

/** @addtogroup AMBIENTLIGHT_External_Func_G2
  *  @brief    Ambient light sensor operation functions
  * @{
  */

/**
  * @brief  Enable the ambient light sensor
  * @retval if success, return EWBMOK value
  */
int ALS_Enable(void)
{
    if(I2C_MasterModeEnable(&gI2CHandle) != EWBMOK)
        return EWBMERR;

    return EWBMOK;
}

/**
  * @brief  Reads the resolution of lux
  * @param  mode: resolution mode of ambient light sensor selection
  * @param  readLux: pointer to a get the lux
  * @retval if success, return EWBMOK value
  */
int ALS_ReadResolution(uint8_t mode, uint16_t* readLux)
{
    tI2C_MSG sndMsg;
    tI2C_MSG recvMsg;
    uint8_t sndBuf;
    uint8_t recvBuf[2];

    memset(&sndMsg, 0, sizeof(tI2C_MSG));
    memset(&recvMsg, 0, sizeof(tI2C_MSG));

    sndBuf = mode;
    sndMsg.slaveAddr = BH1750_SLAVE_ADDR;
    sndMsg.buf = &sndBuf;
    sndMsg.len = 1;

    if(I2C_MasterSend(&gI2CHandle, &sndMsg) == EWBMOK) {
        __m_delay(180);  // Wait to complete 1st H-resolution mode measurement. (max 180ms)

        recvMsg.slaveAddr = BH1750_SLAVE_ADDR;
        recvMsg.buf = recvBuf;
        recvMsg.len = 2;

        if(I2C_MasterRecv(&gI2CHandle, &recvMsg) != EWBMOK)
            return EWBMERR;
    } else {
        return EWBMERR;
    }

    *readLux = (uint16_t)((recvMsg.buf[0]<<8) | (recvMsg.buf[1]));

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

