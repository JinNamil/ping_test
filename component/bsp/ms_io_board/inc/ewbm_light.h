/**
  ******************************************************************************
  * @file    ewbm_ambientlight.h
  * @author  BNDTEK Team
  * @version V0.1
  * @date    22-November-2015
  * @brief   Header file of Ambient Light BSP module.
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

#ifndef __EWBM_AMBIENTLIGHT_H__
#define __EWBM_AMBIENTLIGHT_H__

#include "ewbm_device.h"

/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_AMBIENTLIGHT
  * @{
  */ 

/** @defgroup AMBIENTLIGHT_External_Types AMBIENTLIGHT Exported Types
  * @{
  */

//BH1750 sensor
#define BH1750_SLAVE_ADDR                  0x23  // 7bit addr
#define BH1750_POWERDOWN                   0x00
#define BH1750_POWERON                     0x01
#define BH1750_RESET                       0x07
#define CONTINUOUSLY_H_RESOLUTION_MODE     0x10
#define CONTINUOUSLY_H_RESOLUTION_MODE2    0x11
#define CONTINUOUSLY_L_RESOLUTION_MODE     0x13
#define ONETIME_H_RESOLUTION_MODE          0x20
#define ONETIME_H_RESOLUTION_MODE2         0x21
#define ONETIME_L_RESOLUTION_MODE          0x23

/**
  * @brief  Ambient Light sensor frequency enumeration
  */
typedef enum {
    LIGHT_FREQ_100KHZ = 0,
    LIGHT_FREQ_200KHZ,
    LIGHT_FREQ_300KHZ,
    LIGHT_FREQ_400KHZ
} eLIGHT_CLKFREQ;

/**
  * @}
  */


/** @addtogroup AMBIENTLIGHT_External_Func
  * @{
  */

/** @addtogroup AMBIENTLIGHT_External_Func_G1
  * @{
  */

// init/deinit functions
int ALS_Init(eLIGHT_CLKFREQ freq);
int ALS_DeInit(void);

/**
  * @}
  */

/** @addtogroup AMBIENTLIGHT_External_Func_G2
  * @{
  */

// operation functions
int ALS_Enable(void);
int ALS_ReadResolution(uint8_t mode, uint16_t* readLux);

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

#endif // __EWBM_AMBIENTLIGHT_H__

