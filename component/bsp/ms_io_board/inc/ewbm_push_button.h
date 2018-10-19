/**
  ******************************************************************************
  * @file    ewbm_bsp_led.h
  * @author  eWBM
  * @version V0.1
  * @date    29-07-2016
  * @brief   Header file of LED module
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

/** @addtogroup EWBM_PUSH_BUTTON
  * @{
  */

/** @defgroup PUSH_BUTTON_Internal_Types PUSH_BUTTON Exported Types
  * @{
  */

/**
  * @brief  PUSH_BUTTON status enumeration
  */
typedef enum _eLED_STATUS {
    PUSH_BTN_SET   = 0,
    PUSH_BTN_RESET = 1
} eLED_STATUS;

/**
  * @}
  */


/** @addtogroup PUSH_BUTTON_External_Func
  * @{
  */

/** @addtogroup PUSH_BUTTON_External_Func_G1
  * @{
  */

// init/deinit functions
int PUSH_BTN_Init(void);
int PUSH_BTN_DeInit(void);

/**
  * @}
  */

/** @addtogroup PUSH_BUTTON_External_Func_G2
  * @{
  */

// operation functions
uint16_t PUSH_BTN_State(void);


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

