/**
  ******************************************************************************
  * @file    ewbm_wdt.h
  * @author  eWBM
  * @version v0.1
  * @date    16-06-2016
  * @brief   Header file of WDT module.
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

#ifndef __EWBM_WDT_H__
#define __EWBM_WDT_H__

#include "ewbm_device.h"

/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_WDT
  * @{
  */

/** @defgroup WDT_External_Types WDT Exported Types
  * @{
  */

/**
  * @brief  WDT callback function type
  */
typedef void (*WDTCB)(void *);

/**
  * @brief  WDT handle structure definition
  */
typedef struct _tWDT_HANDLE
{
    uint8_t  precale;  // timer base value
    uint32_t load;     // Timer Load Value
    uint32_t window;   // Window Value
    uint32_t ewi;      // Early Wakeup Interrupt Value
}tWDT_HANDLE;

/**
  * @}
  */

/** @addtogroup WDT_External_Func
  * @{
  */

/** @addtogroup WDT_External_Func_G1
  * @{
  */

// init/de-init functions
int WDT_Init(tWDT_HANDLE* handle);
int WDT_DeInit(tWDT_HANDLE* handle);

/**
  * @}
  */

/** @addtogroup WDT_External_Func_G2
  * @{
  */

// operation
int WDT_SetLoadValue(tWDT_HANDLE* handle, uint16_t val);
int WDT_GetLoadValue(tWDT_HANDLE* handle, uint16_t* val);
int WDT_Enable(tWDT_HANDLE* handle);
int WDT_Disable(tWDT_HANDLE* handle);

int WDT_SetWindowValue(tWDT_HANDLE* handle, uint16_t val);
int WDT_GetWindowValue(tWDT_HANDLE* handle, uint16_t* val);
int WDT_EnableWindow(tWDT_HANDLE* handle);
int WDT_DisableWindow(tWDT_HANDLE* handle);

int WDT_SetEWIValue(tWDT_HANDLE* handle, uint16_t val);
int WDT_GetEWIValue(tWDT_HANDLE* handle, uint16_t* val);
int WDT_EnableEWI(tWDT_HANDLE* handle, WDTCB intrCb, void* intrCbParam);
int WDT_DisableEWI(tWDT_HANDLE* handle);
uint32_t WDT_GetStatus(tWDT_HANDLE* handle);

uint16_t WDT_GetPartNum(tWDT_HANDLE* handle);
uint16_t WDT_GetConfiguration(tWDT_HANDLE* handle);
uint16_t WDT_GetMajRevision(tWDT_HANDLE* handle);
uint16_t WDT_GetMinRevision(tWDT_HANDLE* handle);
uint16_t WDT_GetReservedNum(tWDT_HANDLE* handle);
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

#endif // __EWBM_WDT_H__
