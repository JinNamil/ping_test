/**
******************************************************************************
* @file    ewbm_gpio_led.c
* @author  eWBM
* @version V0.1
* @date    29-07-2016
* @brief   GPIO LED module driver
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

#include "ewbm_gpio_led.h"
#include "ewbm_gpio.h"

#define GPIO_LED_DBG_ENABLE
#ifdef GPIO_LED_DBG_ENABLE
#include "ewbm_uart.h"
#define GPIO_LED_DBG(fmt,args...)        HAL_UART_Printf(fmt,##args)
#else
#define GPIO_LED_DBG(fmt,args...)
#endif

/** @addtogroup EWBM_GPIO_Peripheral
  * @{
  */

/** @addtogroup EWBM_GPIO_LED
  * @{
  */

/** @defgroup LED_Internal_Types LED Internal Types
  * @{
  */

/**
  * @brief  GPIO handle for LED
  */
static tGPIO_HANDLE gGPIOHandle;

/**
  * @brief  Set the GPIO port
  */
static uint32_t gGpioPort = GPIO_BANK_3;

/**
  * @brief  GPIO pin buffer
  */
static uint16_t gGpioPin[LED_MAX+1] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_ALL};

/**
  * @}
  */

/** @addtogroup LED_External_Func
  * @{
  */

/** @addtogroup LED_External_Func_G1
  *  @brief    Initialization and Deinitialization functions
  * @{
  */

/**
  * @brief  Initializes the leds
  * @retval if success, return EWBMOK value
  */
int GPIO_LED_Init(void)
{
    int result = EWBMINVARG;

    gGPIOHandle.bank = gGpioPort;
    result = GPIO_Init(&gGPIOHandle);
    switch(result) {
        case EWBMERR :
            break;
        case EWBMOK :
            {
                GPIO_SetDir(&gGPIOHandle, GPIO_PIN_ALL, 0, SET);
                GPIO_WritePin(&gGPIOHandle, GPIO_PIN_ALL, RESET);
            }
            break;
        default :
            break;
    }
    return result;
}

/**
  * @brief  DeInitializes the leds
  * @retval if success, return EWBMOK value
  */
int GPIO_LED_DeInit(void)
{
    if(GPIO_DeInit(&gGPIOHandle) != EWBMOK)
        return EWBMERR;

    return EWBMOK;
}

/**
  * @}
  */

/** @addtogroup LED_External_Func_G2
  *  @brief    LED operation functions
  * @{
  */

/**
  * @brief  Enable the specified leds
  * @param  num: specifies the leds to be enabled
  * @retval if success, return EWBMOK value
  */
int GPIO_LED_On(eLED_NUMBER num)
{
    if(GPIO_SetDir(&gGPIOHandle, gGpioPin[num], 0, SET) == EWBMOK) {
        GPIO_WritePin(&gGPIOHandle, gGpioPin[num], LED_SET);
    } else {
        return EWBMERR;
    }

    return EWBMOK;
}

/**
  * @brief  Disable the specified leds
  * @param  num: specifies the leds to be disabled
  * @retval if success, return EWBMOK value
  */
int GPIO_LED_Off(eLED_NUMBER num)
{
    if(GPIO_WritePin(&gGPIOHandle, gGpioPin[num], LED_RESET) != EWBMOK)
        return EWBMERR;

    return EWBMOK;
}

/**
  * @brief  Toggles the specified leds
  * @param  num: specifies the leds to be toggled
  * @retval if success, return EWBMOK value
  */
int GPIO_LED_Toggle(eLED_NUMBER num)
{
    if(GPIO_TogglePin(&gGPIOHandle, gGpioPin[num]) != EWBMOK)
        return EWBMERR;

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
