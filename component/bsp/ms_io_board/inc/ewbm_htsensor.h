/**
  ******************************************************************************
  * @file    ewbm_htsensor.h
  * @author  BNDTEK Team
  * @version V0.1
  * @date    22-November-2015
  * @brief   Header file of Humidity & Temperature BSP module.
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

#ifndef __EWBM_AMBLIGHT_H__
#define __EWBM_AMBLIGHT_H__

#include "ewbm_device.h"

/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_HTSENSOR
  * @{
  */ 

/** @defgroup HTSENSOR_External_Types HTSENSOR Exported Types
  * @{
  */

/**
  * @brief  SI7021(Humidity & Temperature sensor) register
  */
#define SI7021_SLAVE_ADDR                                      0x40  // 7bit addr
#define MEASURE_RELATIVE_HUMIDITY_HOLD_MASTER_MODE             0xE5
#define MEASURE_RELATIVE_HUMIDITY_NO_HOLD_MASTER_MODE          0xF5
#define MEASURE_TEMPERATURE_HOLD_MASTER_MODE                   0xE3
#define MEASURE_TEMPERATURE_NO_HOLD_MASTER_MODE                0xF3
#define READ_TEMPERATURE_VALUE_FROM_PREVIOUS_RH_MEASUREMENT    0xE0
#define SI7021_RESET                                           0xFE
#define WRITE_RH_T_USER_REGISTER1                              0xE6
#define READ_RH_T_USER_REGISTER1                               0xE7

#define READ_ELECTRONIC_ID_1ST_1_BYTE                          0xFA
#define READ_ELECTRONIC_ID_1ST_2_BYTE                          0x0F
#define READ_ELECTRONIC_ID_2ST_1_BYTE                          0xFC
#define READ_ELECTRONIC_ID_2ST_2_BYTE                          0xC9

#define READ_FIRMWARE_REVISION_1ST                             0x84
#define READ_FIRMWARE_REVISION_2ST                             0xB8

/**
  * @brief  Humidity & Temperature sensor frequency enumeration
  */
typedef enum {
    HTS_FREQ_100KHZ = 0,
    HTS_FREQ_200KHZ,
    HTS_FREQ_300KHZ,
    HTS_FREQ_400KHZ
} eHTS_CLKFREQ;

/**
  * @}
  */


/** @addtogroup HTSENSOR_External_Func
  * @{
  */

/** @addtogroup HTSENSOR_External_Func_G1
  * @{
  */

// init/deinit functions
int HTS_Init(eHTS_CLKFREQ freq);
int HTS_DeInit(void);

/**
  * @}
  */

/** @addtogroup HTSENSOR_External_Func_G2
  * @{
  */

// operation functions
int HTS_Enable(void);
int HTS_FirmwareRev(uint8_t* fwRev);
int HTS_CheckSN(uint8_t* serialNum);
int HTS_ReadTemp(uint8_t mode, uint8_t* buf, uint16_t* readTM);
int HTS_ReadHumidity(uint8_t mode, uint8_t* buf, uint16_t* readHM);

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

#endif // __EWBM_AMBLIGHT_H__

