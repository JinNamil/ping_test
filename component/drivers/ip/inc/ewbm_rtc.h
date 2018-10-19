/**
  ******************************************************************************
  * @file    ewbm_rtc.h
  * @author  eWBM
  * @version v0.1
  * @date    16-06-2016
  * @brief   Header file of RTC module.
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

#ifndef __EWBM_RTC_H__
#define __EWBM_RTC_H__

#include "ewbm_device.h"

/**
  * @brief  RTC Type define
  */
#define __RO      volatile const  // read only
#define __WO      volatile        // write only
#define __RW      volatile        // read-write
#define __ROAC    volatile        // read only, automatic clear
#define __WOAC    volatile        // write only, automatic clear
#define __RWAC    volatile        // read-write, automatic clear
#define __RW1C    volatile        // read only, Write-1-to-clear

/**
  * @brief  RTC callback function type
  */
typedef void (*RTCCB)(void*);

/**
  * @brief  RTC clock selection enumeration
  */
typedef enum _eRTC_CLKSEL {
    RTC_CLK_XTAL = 0,
    RTC_CLK_APB
} eRTC_CLKSEL;

/**
  * @brief  Alarm information structure definition
  */
typedef struct _tRTC_ALARM {
    uint32_t alarm_time;
    uint32_t alarm_date;
} tRTC_ALARM;

/**
  * @brief  Wakeup information structure definition
  */
typedef struct _tRTC_WAKEUP {
    uint32_t wakeup_cnt;
} tRTC_WAKEUP;

/**
  * @brief  Timestamp information structure definition
  */
typedef struct _tRTC_TSTAMP {
    uint32_t ts_time;
    uint32_t ts_date;
} tRTC_TSTAMP;

/**
  * @brief  RTC information structure definition
  */
typedef struct _tRTC_INTR_INFO {
    bool        inten;
    bool        alarmen;
    bool        wakeupen;
    bool        tsmpen;
    tRTC_ALARM  alarm;
    tRTC_WAKEUP wakeup;
    tRTC_TSTAMP tstamp;
} tRTC_INTR_INFO;

/**
  * @brief  RTC handle structure definition
  */
typedef struct _tRTC_HANDLE {
    uint32_t    prescale;
    eRTC_CLKSEL clksel;
    uint32_t    rtc_time;
    uint32_t    rtc_date;
} tRTC_HANDLE;


/**
  * @}
  */

/** @addtogroup RTC_External_Func
  * @{
  */

/** @addtogroup RTC_External_Func_G1
  * @{
  */

// init/de-init functions
int RTC_Init(tRTC_HANDLE* handle);
int RTC_DeInit(tRTC_HANDLE* handle);


/**
  * @}
  */

/** @addtogroup RTC_External_Func_G2
  * @{
  */

// operation functions
int RTC_SetDate(tRTC_HANDLE* handle, uint32_t* date);
int RTC_GetDate(tRTC_HANDLE* handle, uint32_t* date);
int RTC_SetTime(tRTC_HANDLE* handle, uint32_t* time);
int RTC_GetTime(tRTC_HANDLE* handle, uint32_t* time);

int RTC_EnableAlarm(tRTC_HANDLE* handle, RTCCB intrCb, tRTC_INTR_INFO* intrInfo);
int RTC_DisableAlarm(tRTC_HANDLE* handle);
int RTC_GetAlarmDate(tRTC_HANDLE* handle, uint32_t* alarmDate);
int RTC_GetAlarmTime(tRTC_HANDLE* handle, uint32_t* alarmTime);

int RTC_EnableWakeup(tRTC_HANDLE* handle, RTCCB intrCb, tRTC_INTR_INFO* intrInfo);
int RTC_DisableWakeup(tRTC_HANDLE* handle);

int RTC_EnableTimestamp(tRTC_HANDLE* handle, RTCCB intrCb, tRTC_INTR_INFO* intrInfo);
int RTC_DisableTimestamp(tRTC_HANDLE* handle);
int RTC_GetTimestampDate(tRTC_HANDLE* handle, uint32_t* tstampDate);
int RTC_GetTimestampTime(tRTC_HANDLE* handle, uint32_t* tstampTime);

int RTC_GetStatus(tRTC_HANDLE* handle, uint32_t* status);

uint16_t RTC_GetPartNum(tRTC_HANDLE* handle);
uint16_t RTC_GetConfiguration(tRTC_HANDLE* handle);
uint16_t RTC_GetMajRevision(tRTC_HANDLE* handle);
uint16_t RTC_GetMinRevision(tRTC_HANDLE* handle);
uint16_t RTC_GetReservedNum(tRTC_HANDLE* handle);


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

#endif // __EWBM_RTC_H__
