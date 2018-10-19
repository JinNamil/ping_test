/**
  ******************************************************************************
  * @file    ewbm_gpio.h
  * @author  eWBM
  * @version v0.1
  * @date    16-06-2016
  * @brief   Header file of GPIO module.
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

#ifndef __EWBM_GPIO_H__
#define __EWBM_GPIO_H__

/* Includes */
#include "ewbm_device.h"


/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_GPIO
  * @{
  */

/** @defgroup GPIO_External_Types GPIO Exported Types
  * @{
  */

/**
  * @brief  GPIO callback function type
  */
typedef void (*GPIOCB)(void *);

/**
  * @brief  GPIO bank enumeration
  */
typedef enum {
    GPIO_BANK_1 = 0,
    GPIO_BANK_2,
    GPIO_BANK_3,
    GPIO_BANK_MAX
} eGPIO_BANK;

typedef enum 
{
    RESET       = 0,
    SET         = 1,
} eGPIO_FLAGS;

/**
  * @brief  GPIO pin index enumeration
  */
typedef enum _eGPIO_PIN {
    GPIO_PIN_0   = 0x0001,
    GPIO_PIN_1   = 0x0002,
    GPIO_PIN_2   = 0x0004,
    GPIO_PIN_3   = 0x0008,
    GPIO_PIN_4   = 0x0010,
    GPIO_PIN_5   = 0x0020,
    GPIO_PIN_6   = 0x0040,
    GPIO_PIN_7   = 0x0080,
    GPIO_PIN_8   = 0x0100,
    GPIO_PIN_9   = 0x0200,
    GPIO_PIN_10  = 0x0400,
    GPIO_PIN_11  = 0x0800,
    GPIO_PIN_12  = 0x1000,
    GPIO_PIN_13  = 0x2000,
    GPIO_PIN_14  = 0x4000,
    GPIO_PIN_15  = 0x8000,
    GPIO_PIN_ALL = 0xFFFF
} eGPIO_PIN;
/**
  * @brief  GPIO interrupt type enumeration
  */
typedef enum _eGPIO_INTTYPE {
    GPIO_INT_TYPE_EDGE_TRIGGER = 0,
    GPIO_INT_TYPE_LEVEL_SENSITIVE = 1
} eGPIO_INTTYPE;

/**
  * @brief  GPIO interrupt polarity for edge-trigger mode enumeration
  */
typedef enum _eGPIO_INTPOL_EDGE_TRIGGER_MODE {
    GPIO_INT_POL_FALLINGEDGE = 0,
    GPIO_INT_POL_RISINGEDGE = 1
} eGPIO_INTPOL_EDGE_TRIGGER_MODE;


/**
  * @brief  GPIO interrupt polarity for level sensitive mode enumeration
  */
typedef enum _eGPIO_INTPOL_LEVEL_SENSITIVE_MODE {
    GPIO_INT_POL_LOW = 0,
    GPIO_INT_POL_HIGH = 1
} eGPIO_INTPOL_LEVEL_SENSITIVE_MODE;

/**
  * @brief  GPIO interrupt both edge of interrupt polarity enable enumeration
  */
typedef enum _eGPIO_INTPOL_BOTHEDGE {
    GPIO_BOTHEDGE_DISABLE = 0,
    GPIO_BOTHEDGE_ENABLE = 1
} eGPIO_INTPOL_BOTHEDGE;

/**
  * @brief  GPIO interrupt combining mode enumeration
  */
typedef enum _eGPIO_INT_COMB_MODE {
    GPIO_INT_COMB_OR = 0,
    GPIO_INT_COMB_AND = 1
} eGPIO_INT_COMB_MODE;

/**
  * @brief  GPIO handle structure definition
  */
typedef struct _tGPIO_HANDLE {
    uint8_t     bank;
    tGPIO_REG*  reg;
} tGPIO_HANDLE;

/**
  * @}
  */

/** @addtogroup GPIO_External_Func
  * @{
  */

/** @addtogroup GPIO_External_Func_G1
  * @{
  */

/* Initialization and de-initialization functions */
int GPIO_Init(tGPIO_HANDLE* handle);
int GPIO_DeInit(tGPIO_HANDLE* handle);

/**
  * @}
  */

/** @addtogroup GPIO_External_Func_G2
  * @{
  */

/* operation functions */
uint16_t     GPIO_ReadMode(tGPIO_HANDLE* handle);
int GPIO_SetDir(tGPIO_HANDLE* handle, uint32_t pin, uint32_t type, uint8_t enable);
uint8_t      GPIO_ReadPin(tGPIO_HANDLE* handle, uint32_t pin);
int GPIO_WritePin(tGPIO_HANDLE* handle, uint32_t pin, uint8_t enable);
uint16_t     GPIO_ReadData(tGPIO_HANDLE* handle);
int GPIO_TogglePin(tGPIO_HANDLE* handle, uint32_t pin);
int GPIO_SetReset(tGPIO_HANDLE* handle, uint32_t resetPin, uint32_t setPin);
uint16_t     GPIO_InterruptReadRawStatus(tGPIO_HANDLE* handle);
uint32_t     GPIO_InterruptReadEnable(tGPIO_HANDLE* handle);
int GPIO_InterruptSetEnable(tGPIO_HANDLE* handle, uint32_t intrPin, uint32_t intrCombMode);
uint16_t     GPIO_InterruptReadStatus(tGPIO_HANDLE* handle);
int GPIO_InterruptClear(tGPIO_HANDLE* handle, uint16_t intrPin);
int GPIO_InterruptStart(tGPIO_HANDLE* handle, uint16_t intrType, uint16_t intrPol, uint32_t intrBothEdge, uint16_t intrPin, uint32_t intrCombMode, GPIOCB intrCb, void* intrCbParam);
int GPIO_InterruptStop(tGPIO_HANDLE* handle, uint16_t intrPin);

uint16_t GPIO_GetPartNum(tGPIO_HANDLE* handle);
uint16_t GPIO_GetConfiguration(tGPIO_HANDLE* handle);
uint16_t GPIO_GetMajRevision(tGPIO_HANDLE* handle);
uint16_t GPIO_GetMinRevision(tGPIO_HANDLE* handle);
uint16_t GPIO_GetReservedNum(tGPIO_HANDLE* handle);

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

#endif // __EWBM_GPIO_H__

