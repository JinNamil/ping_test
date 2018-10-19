/**
  ******************************************************************************
  * @file    ewbm_rtc.c
  * @author  eWBM
  * @version v0.1
  * @date    16-06-2016
  * @brief   RTC module driver
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

#include "ewbm_rtc.h"
#include "ewbm_isr.h"
#include "ewbm_delay.h"


#ifdef RTC_DBG_ENABLE
#include "ewbm_uart.h"
#define RTC_DBG(fmt,args...)    uart_printf(fmt, ##args)
#else
#define RTC_DBG(fmt,args...)
#endif


/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_RTC
  * @{
  */

/** @defgroup RTC_Internal_Types RTC Intenal Types
  * @{
  */

/**
  * @brief  RTC interrupt type enumeration
  */
typedef enum {
    RTC_INTERRUPT_ALARM = 0,
    RTC_INTERRUPT_WAKEUP,
    RTC_INTERRUPT_TSTMAP,
    RTC_INTERRUPT_MAX
}eRTC_INTRTYPE;

/**
  * @brief  tRTC_INTRCB structure that use to register interrupt
  */
typedef struct _tRTC_INTRCB {
    RTCCB  cb;
    void*   param;
}tRTC_INTRCB;

/**
  * @brief  Internal global arrangement variables that have a structure of tRTC_INTRCB
  */
static tRTC_INTRCB gRTCIntrArray[RTC_INTERRUPT_MAX];


/**
  * @}
  */


/** @addtogroup RTC_Static_Func
  * @{
  */

/** @addtogroup RTC_Static_Func_G1
  * @{
  */

/**
  * @brief  Get the Interrupt status
  * @param  addr: pointer to base address
  * @param  status: pointer to get interrupt status value
  * @retval  if success, return EWBMOK value
  */
static int rtc_get_intrsts(tRTC_REG* addr, uint32_t* status)
{
    *status = addr->intr_stsclr;
    return EWBMOK;
}

#if 0
/**
  * @brief  Clear the Interrupt status
  * @param  addr: pointer to base address
  * @param  status: clear interrupt status value
  * @retval  if success, return EWBMOK value
  */
static int rtc_clear_intrsts(tRTC_REG* addr, uint32_t status)
{
    addr->intr_stsclr = status;
    return EWBMOK;
}
#endif

/**
  * @brief  RTC interrupt callback. call the registered callback function according to inerrupt type
  * @param  priv: pointer to parameter of interrupt
  * @retval none
  */
static void rtc_intr_cb(void *priv)
{
    tRTC_REG* baseAddr = RTC;
    uint32_t sts = 0;
    uint32_t i = 0;

    // get interrupt status
    rtc_get_intrsts(baseAddr, &sts);

    for ( i=0; i<RTC_INTERRUPT_MAX; i++ )
    {
        if ( sts&(1<<i) )
        {
            if ( gRTCIntrArray[i].cb )
            {
                gRTCIntrArray[i].cb(gRTCIntrArray[i].param);
            }
        }
    }
}

/**
  * @}
  */

/**
  * @}
  */


/** @addtogroup RTC_External_Func
  * @{
  */

/** @addtogroup RTC_External_Func_G1
  *  @brief    Initialization and Configuration functions
  * @{
  */

/**
  * @brief  Initializes the RTC according to the specified parameters
  * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
  * @retval  if success, return EWBMOK value
  */
int RTC_Init(tRTC_HANDLE* handle)
{
    tRTC_REG* baseAddr = RTC;

    if ( handle == NULL )
        return EWBMINVARG;

//    if ( handle->prescale > 0 )
//    {
//        baseAddr->clk_div |= handle->prescale;
//        __io_delay(1000);
//    }

    baseAddr->ctrl |= RTC_CTRL_RESET;

    __m_delay(1);

    // set current time
    if ( (handle->rtc_time > 0) || (handle->rtc_date > 0) )
    {
        baseAddr->time = handle->rtc_time;
        baseAddr->date = handle->rtc_date;
        baseAddr->ctrl |= RTC_CTRL_INIT;
    }

//    switch ( handle->clksel )
//    {
//        case RTC_CLK_XTAL:
//            // using XTAL osc
//            baseAddr->ctrl &= ~RTC_CTRL_CLK_SEL;
//            break;
//
//        case RTC_CLK_APB:
//            // using APB clock
//            baseAddr->ctrl |= RTC_CTRL_CLK_SEL;
//            break;
//
//        default:
//            return EWBMINVARG;
//    }
//
//    // RTC XTAL Enable
//    baseAddr->ctrl |= RTC_CTRL_XTAL_EN;

    // disable all interrupt
    baseAddr->intr_en = 0;

    return EWBMOK;
}

/**
  * @brief  DeInitialize RTC
  * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
  * @retval  if success, return EWBMOK value
  */
int RTC_DeInit(tRTC_HANDLE* handle)
{
    tRTC_REG* baseAddr = RTC;

    if ( handle == NULL )
        return EWBMINVARG;

    // Stop RTC interrupt
    baseAddr->intr_en = 0;

    // Stop RTC control
    baseAddr->ctrl = 0;

    external_isr_clear((IRQn_Type)RTC_IRQn);

    return EWBMOK;
}


/**
  * @}
  */

/** @addtogroup RTC_External_Func_G2
  *  @brief    Operation functions
  * @{
  */

/**
  * @brief  Set date to RTC
  * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
  * @param  date: pointer to set the date information of RTC
  * @retval  if success, return EWBMOK value
  */
int RTC_SetDate(tRTC_HANDLE* handle, uint32_t* date)
{
    tRTC_REG* baseAddr = RTC;

    if ( handle == NULL )
        return EWBMINVARG;

    baseAddr->ctrl |= RTC_CTRL_INIT;

    baseAddr->date = *date;

    return EWBMOK;
}

/**
  * @brief  Get date info of RTC
  * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
  * @param  date: pointer to get the date information of RTC
  * @retval  if success, return EWBMOK value
  */
int RTC_GetDate(tRTC_HANDLE* handle, uint32_t* date)
{
    tRTC_REG* baseAddr = RTC;

    if ( handle == NULL )
        return EWBMINVARG;

    *date = baseAddr->date;

    return EWBMOK;
}

/**
  * @brief  Set time to RTC
  * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
  * @param  time: pointer to set the time information of RTC
  * @retval  if success, return EWBMOK value
  */
int RTC_SetTime(tRTC_HANDLE* handle, uint32_t* time)
{
    tRTC_REG* baseAddr = RTC;

    if ( handle == NULL )
        return EWBMINVARG;

    baseAddr->ctrl |= RTC_CTRL_INIT;

    baseAddr->time = *time;

    return EWBMOK;
}

/**
  * @brief  Get time info of RTC
  * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
  * @param  time: pointer to get the time information of RTC
  * @retval  if success, return EWBMOK value
  */
int RTC_GetTime(tRTC_HANDLE* handle, uint32_t* time)
{
    tRTC_REG* baseAddr = RTC;

    if ( handle == NULL )
        return EWBMINVARG;

    *time = baseAddr->time;

    return EWBMOK;
}

/**
  * @brief  Enable the interrupt of alarm
  * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
  * @param  intrCb: pointer to callback function when interrupt occurs
  * @param  intrInfo: pointer about information of interrupt
  * @retval  if success, return EWBMOK value
  */
int RTC_EnableAlarm(tRTC_HANDLE* handle, RTCCB intrCb, tRTC_INTR_INFO* intrInfo)
{
    tRTC_REG* baseAddr = RTC;

    if ( handle == NULL )
        return EWBMINVARG;

    if ( intrInfo->alarmen )
    {
        // Set alarm date
        baseAddr->alarm_date = intrInfo->alarm.alarm_date;

        // set alarm time
        baseAddr->alarm_time = intrInfo->alarm.alarm_time;

        // RTC alarm mask (to compare hour, min, sec)
        baseAddr->alarm_mask |= 0x78;

        // Enable alarm
        baseAddr->ctrl |= RTC_CTRL_ALARM_EN;

        // Enable alarm interrupt
        baseAddr->intr_en |= RTC_INTR_STS_ALARM;

        if ( intrCb )
        {
            // Register callback func to array of RTC interrupt
            gRTCIntrArray[RTC_INTERRUPT_ALARM].cb = intrCb;
            gRTCIntrArray[RTC_INTERRUPT_ALARM].param = NULL;
            if ( intrInfo->inten )
                external_isr_set((IRQn_Type)RTC_IRQn, rtc_intr_cb, (void*)handle);
        }
    }

    return EWBMOK;
}

/**
  * @brief  Disable the interrupt of alarm
  * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
  * @retval  if success, return EWBMOK value
  */
int RTC_DisableAlarm(tRTC_HANDLE* handle)
{
    tRTC_REG* baseAddr = RTC;

    if ( handle == NULL )
        return EWBMINVARG;

    // RTC interrupt status register
    baseAddr->intr_stsclr |= RTC_INTR_STS_ALARM;

    // Disable alarm
    baseAddr->ctrl &= ~RTC_CTRL_ALARM_EN;

    return EWBMOK;
}

/**
  * @brief  Get date of alarm
  * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
  * @param  alarmDate: pointer to get the date information of alarm
  * @retval  if success, return EWBMOK value
  */
int RTC_GetAlarmDate(tRTC_HANDLE* handle, uint32_t* alarmDate)
{
    tRTC_REG* baseAddr = RTC;

    if ( handle == NULL )
        return EWBMINVARG;

    *alarmDate = baseAddr->alarm_date;

    return EWBMOK;
}

/**
  * @brief  Get time of alarm
  * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
  * @param  alarmTime: pointer to get the time information of alarm
  * @retval  if success, return EWBMOK value
  */
int RTC_GetAlarmTime(tRTC_HANDLE* handle, uint32_t* alarmTime)
{
    tRTC_REG* baseAddr = RTC;

    if ( handle == NULL )
        return EWBMINVARG;

    *alarmTime = baseAddr->alarm_time;

    return EWBMOK;
}

/**
  * @brief  Enable the interrupt of wakeup
  * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
  * @param  intrCb: pointer to callback function when interrupt occurs
  * @param  intrInfo: pointer about information of interrupt
  * @retval  if success, return EWBMOK value
  */
int RTC_EnableWakeup(tRTC_HANDLE* handle, RTCCB intrCb, tRTC_INTR_INFO* intrInfo)
{
    tRTC_REG* baseAddr = RTC;

    if ( handle == NULL )
        return EWBMINVARG;

    if ( intrInfo->wakeupen )
    {

        // Enable wakeup Enable
        baseAddr->wakeup_cnt = intrInfo->wakeup.wakeup_cnt;

        // Enable wakeup interrupt
        baseAddr->intr_en |= RTC_INTR_STS_WAKEUP;

        // Enable wakeup timer
        baseAddr->ctrl |= RTC_CTRL_WAKEUP_EN;

        if ( intrCb )
        {
            // Register callback func to array of RTC interrupt
            gRTCIntrArray[RTC_INTERRUPT_WAKEUP].cb = intrCb;
            gRTCIntrArray[RTC_INTERRUPT_WAKEUP].param = NULL;
            if ( intrInfo->inten )
                external_isr_set((IRQn_Type)RTC_IRQn, rtc_intr_cb, (void*)handle);
        }
    }

    return EWBMOK;
}

/**
  * @brief  Disable the interrupt of wakeup
  * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
  * @retval  if success, return EWBMOK value
  */
int RTC_DisableWakeup(tRTC_HANDLE* handle)
{
    tRTC_REG* baseAddr = RTC;

    if ( handle == NULL )
        return EWBMINVARG;

    // RTC interrupt status register
    baseAddr->intr_stsclr |= RTC_INTR_STS_WAKEUP;

    // Disable wakeup
    baseAddr->ctrl &= ~RTC_CTRL_WAKEUP_EN;

    return EWBMOK;
}

/**
  * @brief  Enable the interrupt of timestamp
  * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
  * @param  intrCb: pointer to callback function when interrupt occurs
  * @param  intrInfo: pointer about information of interrupt
  * @retval  if success, return EWBMOK value
  */
int RTC_EnableTimestamp(tRTC_HANDLE* handle, RTCCB intrCb, tRTC_INTR_INFO* intrInfo)
{
    tRTC_REG* baseAddr = RTC;

    if ( handle == NULL )
        return EWBMINVARG;

    if ( intrInfo->tsmpen )
    {
        // Enable wakeup interrupt
        baseAddr->intr_en |= RTC_INTR_STS_TSTAMP_TRIG;

        // Enable timestamp
        baseAddr->ctrl |= RTC_CTRL_TSTAMP_EN;

        if ( intrCb )
        {
            // Register callback func to array of RTC interrupt
            gRTCIntrArray[RTC_INTERRUPT_WAKEUP].cb = intrCb;
            gRTCIntrArray[RTC_INTERRUPT_WAKEUP].param = NULL;

            if ( intrInfo->inten )
                external_isr_set((IRQn_Type)RTC_IRQn, rtc_intr_cb, (void*)handle);
        }
    }

    return EWBMOK;
}

/**
  * @brief  Disable the interrupt of timestamp
  * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
  * @retval  if success, return EWBMOK value
  */
int RTC_DisableTimestamp(tRTC_HANDLE* handle)
{
    tRTC_REG* baseAddr = RTC;

    if ( handle == NULL )
        return EWBMINVARG;

    // RTC interrupt status register
    baseAddr->intr_stsclr |= RTC_INTR_STS_TSTAMP_TRIG;

    // Disable timestamp
    baseAddr->ctrl &= ~RTC_CTRL_TSTAMP_EN;

    return EWBMOK;
}

/**
  * @brief  Get the date of timestamp
  * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
  * @param  tstampDate: pointer to get the date information of timestamp
  * @retval  if success, return EWBMOK value
  */
int RTC_GetTimestampDate(tRTC_HANDLE* handle, uint32_t* tstampDate)
{
    tRTC_REG* baseAddr = RTC;

    if ( handle == NULL )
        return EWBMINVARG;

    *tstampDate = baseAddr->ts_date;

    return EWBMOK;
}

/**
  * @brief  Get the time of timestamp
  * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
  * @param  tstampDate: pointer to get the time information of timestamp
  * @retval  if success, return EWBMOK value
  */
int RTC_GetTimestampTime(tRTC_HANDLE* handle, uint32_t* tstampTime)
{
    tRTC_REG* baseAddr = RTC;

    if ( handle == NULL )
        return EWBMINVARG;

    *tstampTime = baseAddr->ts_time;

    return EWBMOK;
}

/**
  * @brief  Get the interrupt status
  * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
  * @param  status: pointer to get the interrupt status
  * @retval  if success, return EWBMOK value
  */
int RTC_GetStatus(tRTC_HANDLE* handle, uint32_t* status)
{
    tRTC_REG* baseAddr = RTC;

    if ( handle == NULL )
        return EWBMINVARG;

    *status = baseAddr->intr_stsclr;

    return EWBMOK;
}


/**
 * @brief  Get part number of RTC
 * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
 * @retval  part number
 */
uint16_t RTC_GetPartNum(tRTC_HANDLE* handle)
{
    if ( handle == NULL )
        return EWBMINVARG;

    tRTC_REG* baseaddr = RTC;

    return baseaddr->peri_id & RTC_PERI_ID_PART_NUM;
}

/**
 * @brief  Get configuration number of RTC
 * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
 * @retval  part number
 */
uint16_t RTC_GetConfiguration(tRTC_HANDLE* handle)
{
    if ( handle == NULL )
        return EWBMINVARG;

    tRTC_REG* baseaddr = RTC;

    return ((baseaddr->peri_id >> 12) & RTC_PERI_ID_CONFIG_NUM);
}

/**
 * @brief  Get major revision of RTC
 * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
 * @retval  part number
 */
uint16_t RTC_GetMajRevision(tRTC_HANDLE* handle)
{
    if ( handle == NULL )
        return EWBMINVARG;

    tRTC_REG* baseaddr = RTC;

    return ((baseaddr->peri_id >> 16) & RTC_PERI_ID_MAJ_REV);
}

/**
 * @brief  Get minor revision of RTC
 * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
 * @retval  part number
 */
uint16_t RTC_GetMinRevision(tRTC_HANDLE* handle)
{
    if ( handle == NULL )
        return EWBMINVARG;

    tRTC_REG* baseaddr = RTC;

    return ((baseaddr->peri_id >> 20) & RTC_PERI_ID_MIN_REV);
}

/**
 * @brief  Get Reserved number of RTC
 * @param  handle: pointer to a tRTC_HANDLE structure that contains the configuration information for RTC module
 * @retval  part number
 */
uint16_t RTC_GetReservedNum(tRTC_HANDLE* handle)
{
    if ( handle == NULL )
        return EWBMINVARG;

    tRTC_REG* baseaddr = RTC;

    return ((baseaddr->peri_id >> 24) & RTC_PERI_ID_RSVD_NUM);
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

