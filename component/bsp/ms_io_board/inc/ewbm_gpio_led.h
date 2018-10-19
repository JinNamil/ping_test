/**
  ******************************************************************************
  * @file    ewbm_bsp_led.h
  * @author  eWBM
  * @version V0.1
  * @date    29-07-2016
  * @brief   Header file of GPIO LED module.
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

#ifndef __EWBM_GPIO_LED_H__
#define __EWBM_GPIO_LED_H__

#include "ewbm_device.h"


/** @addtogroup EWBM_GPIO_Peripheral
  * @{
  */

/** @addtogroup EWBM_GPIO_LED
  * @{
  */

/** @defgroup LED_External_Types LED Exported Types
  * @{
  */

/**
  * @brief  LED number enumeration
  */
typedef enum _eLED_NUMBER {
    LED_1 = 0,
    LED_2,
    LED_3,
    LED_4,
    LED_5,
    LED_6,
    LED_7,
    LED_8,
    LED_MAX
} eLED_NUMBER;


/**
  * @brief  LED status enumeration
  */
typedef enum _eLED_STATUS {
    LED_SET   = 0,
    LED_RESET = 1
} eLED_STATUS;

/**
  * @}
  */


/** @addtogroup LED_External_Func
  * @{
  */

/** @addtogroup LED_External_Func_G1
  * @{
  */
int BSP_LED_Init(void);
int BSP_LED_DeInit(void);

/**
  * @}
  */

/** @addtogroup LED_External_Func_G2
  * @{
  */
int GPIO_LED_Init(void);
int GPIO_LED_On(eLED_NUMBER num);
int GPIO_LED_Off(eLED_NUMBER num);
int GPIO_LED_Toggle(eLED_NUMBER num);

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

#endif // __EWBM_GPIO_LED_H__

