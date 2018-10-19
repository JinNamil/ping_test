/**
  ******************************************************************************
  * @file    ewbm_i2c.h
  * @author  eWBM
  * @version v0.1
  * @date    16-06-2016
  * @brief   Header file of I2C module.
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

#ifndef __EWBM_I2C_H__
#define __EWBM_I2C_H__

#include "ewbm_device.h"

/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_I2C
  * @{
  */

/** @defgroup I2C_External_Types I2C Exported Types
  * @{
  */

/**
  * @brief  I2C Channel enumeration
  */
typedef enum {
    I2C_CHANNEL_1 = 0,
    I2C_CHANNEL_2,
    I2C_CHANNEL_3,
    I2C_CHANNEL_4
} eI2C_CHANNEL;

/**
  * @brief  I2C frequency enumeration
  */
typedef enum {
    I2C_TRANS_WRITE = 0x0,
    I2C_TRANS_READ = 0x1
} eI2C_TRANSFERFLAG;


/**
  * @brief  I2C frequency enumeration
  */
typedef enum {
    I2C_FREQ_100KHZ,
    I2C_FREQ_200KHZ,
    I2C_FREQ_300KHZ,
    I2C_FREQ_400KHZ
} eI2C_CLKFREQ;

/**
  * @brief I2C message definition
  */
typedef struct _tI2C_MSG {
    uint8_t  slaveAddr;  // 7bit address
    uint8_t* buf;        // pointer to msg data
    uint8_t  len;        // msg length
} tI2C_MSG;

/**
  * @brief  I2C handle structure definition
  */
typedef struct _tI2C_HANDLE {
    eI2C_CLKFREQ  freq;
    eI2C_CHANNEL  channel;
    tI2C_REG*     reg;
} tI2C_HANDLE;

/**
  * @brief  I2C slave address definition
  */
#define I2C_SLAVE_ADDRESS   0x7c  // must be 7bit

/**
  * @}
  */

/** @addtogroup I2C_External_Func
  * @{
  */

/** @addtogroup I2C_External_Func_G1
  * @{
  */
// init/de-init functions
int I2C_Init(tI2C_HANDLE* handle);
int I2C_DeInit(tI2C_HANDLE* handle);
/**
  * @}
  */

/** @addtogroup I2C_External_Func_G2
  * @{
  */
// operation functions
int I2C_MasterModeEnable(tI2C_HANDLE* handle);
int I2C_MasterModeDisable(tI2C_HANDLE* handle);
int I2C_SlaveModeEnable(tI2C_HANDLE* handle);
int I2C_SlaveModeDisable(tI2C_HANDLE* handle);
int I2C_MasterSend(tI2C_HANDLE* handle, tI2C_MSG* msg);
int I2C_MasterRecv(tI2C_HANDLE* handle, tI2C_MSG* msg);
int I2C_SlaveSend(tI2C_HANDLE* handle, tI2C_MSG* msg);
int I2C_SlaveRecv(tI2C_HANDLE* handle, tI2C_MSG* msg);
int I2C_DMAEnable(tI2C_HANDLE* handle);

uint16_t I2C_GetPartNumId0(tI2C_HANDLE* handle);
uint16_t I2C_GetPartNumId1(tI2C_HANDLE* handle);
uint16_t I2C_GetConfiguration(tI2C_HANDLE* handle);
uint16_t I2C_GetMajRevision(tI2C_HANDLE* handle);
uint16_t I2C_GetMinRevision(tI2C_HANDLE* handle);
uint16_t I2C_GetReservedNum(tI2C_HANDLE* handle);


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

#endif // __EWBM_I2C_H__

