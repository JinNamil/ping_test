/**
******************************************************************************
* @file    ewbm_push_button.c
* @author  eWBM
* @version V0.1
* @date    29-07-2016
* @brief   Push Button driver
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

#include "ewbm_push_button.h"
#include "ewbm_gpio.h"

#define PUSH_BUTTON_DBG_ENABLE
#ifdef PUSH_BUTTON_DBG_ENABLE
#include "ewbm_uart.h"
#define PUSH_BUTTON_DBG(fmt,args...)        HAL_UART_Printf(fmt,##args)
#else
#define PUSH_BUTTON_DBG(fmt,args...)
#endif

/** @addtogroup EWBM_GPIO_Peripheral
  * @{
  */

/** @addtogroup EWBM_PUSH_BUTTON
  * @{
  */

/** @defgroup PUSH_BUTTON_Internal_Types PUSH_BUTTON Internal Types
  * @{
  */

/**
  * @brief  GPIO handle for PUSH_BUTTON
  */
static tGPIO_HANDLE gGPIOHandle;

/**
  * @brief  Set the GPIO port
  */
static uint32_t gGpioPort = GPIO_BANK_3;

/**
  * @brief  GPIO pin buffer
  */
//static uint16_t gGpioPin[LED_MAX+1] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_ALL};

/**
  * @}
  */

/** @addtogroup PUSH_BUTTON_External_Func
  * @{
  */

/** @addtogroup PUSH_BUTTON_External_Func_G1
  *  @brief    Initialization and Deinitialization functions
  * @{
  */

/**
  * @brief  Initializes the push button
  * @retval if success, return EWBMOK value
  */
int PUSH_BTN_Init(void)
{
    int result = EWBMINVARG;

    gGPIOHandle.bank = gGpioPort;
    result = GPIO_Init(&gGPIOHandle);
    switch(result) {
        case EWBMERR :
            break;
        case EWBMOK :
            {
                GPIO_SetDir(&gGPIOHandle, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 1, RESET);
            }
            break;
        default :
            break;
    }
    return result;
}

/**
  * @brief  DeInitializes the push button
  * @retval if success, return EWBMOK value
  */
int PUSH_BTN_DeInit(void)
{
    if(GPIO_DeInit(&gGPIOHandle) != EWBMOK)
        return EWBMERR;

    return EWBMOK;
}

/**
  * @}
  */

/** @addtogroup PUSH_BUTTON_External_Func_G2
  *  @brief    PUSH_BUTTON operation functions
  * @{
  */

/**
  * @brief  Enable the specified push button
  * @retval uint16_t value of GPIO_DATA register
  */
uint16_t PUSH_BTN_State(void)
{
       return GPIO_ReadData(&gGPIOHandle)&0xF;
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
