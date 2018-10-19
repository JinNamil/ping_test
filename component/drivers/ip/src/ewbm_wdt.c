/**
  ******************************************************************************
  * @file    ewbm_wdt.c
  * @author  eWBM
  * @version v0.1
  * @date    16-06-2016
  * @brief   WDT module driver
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

/* Header File */
#include "ewbm_wdt.h"
#include "ewbm_isr.h"


#ifdef WDT_DBG_ENABLE
    #include "ewbm_uart.h"
    #define WDT_DBG(fmt,args...)        UART_Printf(fmt,##args)
#else
    #define WDT_DBG(fmt,args...)
#endif

/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_WDT
  * @{
  */

/** @defgroup WDT_Internal_Types WDT Intenal Types
  * @{
  */

/**
  * @brief  tWDT_INTRCB structure that use to register interrupt
  */
typedef struct _tWDT_INTRCB{
    WDTCB  cb;
    void*   param;
}tWDT_INTRCB;

/**
  * @brief  Internal global arrangement variables that have a structure of tWDT_INTRCB
  */
static tWDT_INTRCB gWDTIntr;

/**
  * @}
  */


/** @addtogroup WDT_Static_Func
  * @{
  */

/** @addtogroup WDT_Static_Func_G1
  * @{
  */

/**
  * @brief  Get the Interrupt status
  * @param  addr: pointer to base address
  * @param  status: pointer to get interrupt status value
  * @retval  if success, return EWBMOK value
  */
static int wdt_get_intrsts(tWDT_REG* addr, uint32_t* status)
{
    *status = addr->sts;
    return EWBMOK;
}

/**
  * @brief  Early wakeup interrupt callback. call the registered callback function according to tWDT_INTRCB structure
  * @param  priv: pointer to parameter of interrupt
  * @retval none
  */
static void wdt_intr_cb(void *priv)
{
    tWDT_REG* baseAddr = WDT;
    uint32_t status = 0;

    // interrupt clear (auto read clear)
    wdt_get_intrsts(baseAddr, &status);

    // call the registered callback function
    if ( gWDTIntr.cb )
        gWDTIntr.cb(gWDTIntr.param);
}


/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup WDT_External_Func
  * @{
  */

/** @addtogroup WDT_External_Func_G1
  *  @brief    Initialization and Configuration functions
  * @{
  */


/**
  * @brief  Initializes the Watchdog timer according to the specified parameters
  * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watch dog timer module
  * @retval  if success, return EWBMOK value
  */
int WDT_Init(tWDT_HANDLE* handle)
{
    tWDT_REG* baseAddr = WDT;
    uint32_t tmrbase = 0;

    if ( handle == NULL )
        return EWBMINVARG;

    // wdt control reset value
    baseAddr->ctrl = 0;

    // wachtdog timer base clock
    tmrbase = WDT_CTRL_TMR_BASE(handle->precale);
    if ( WDT_CTRL_TMR_BASE(0xc) > WDT_CTRL_TMR_BASE_MAX )
        return EWBMINVARG;

    baseAddr->ctrl |= tmrbase;

    baseAddr->ewi = (handle->ewi&WDT_EWI_FLAG);
    baseAddr->win = (handle->window&WDT_WIN_FLAG);
    baseAddr->load = (handle->load&WDT_LOAD_FLAG);

    // initialize callback/param
    gWDTIntr.cb = NULL;
    gWDTIntr.param = NULL;

    // register isr handle
    external_isr_set((IRQn_Type)WDT_IRQn, wdt_intr_cb, (void*)handle);

    return EWBMOK;
}

/**
  * @brief  DeInitializes the Watchdog timer
  * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watch dog module
  * @retval  if success, return EWBMOK value
  */
int WDT_DeInit(tWDT_HANDLE* handle)
{
    tWDT_REG* baseAddr = WDT;

    if ( handle == NULL )
        return EWBMINVARG;

    // disable wdt control and value
    baseAddr->ctrl = 0;
    baseAddr->load = 0;
    baseAddr->ewi = 0;
    baseAddr->win = 0;

    // clear wdt irq
    external_isr_clear((IRQn_Type)WDT_IRQn);

    return EWBMOK;
}

/**
  * @}
  */

/** @addtogroup WDT_External_Func_G2
  *  @brief    Operation functions
  * @{
  */

/**
  * @brief  Set a load value for the Watchdog timer base
  * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watch dog timer module
  * @param  val: setting a load value
  * @retval  if success, return EWBMOK value
  */
int WDT_SetLoadValue(tWDT_HANDLE* handle, uint16_t val)
{
    tWDT_REG* baseAddr = WDT;

    if ( handle == NULL )
        return EWBMINVARG;

    baseAddr->load = (val&WDT_LOAD_FLAG);
    return EWBMOK;
}

/**
  * @brief  Get a load value for the Watchdog timer base
  * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watch dog timer module
  * @param  val: pointer to a load value
  * @retval  if success, return EWBMOK value
  */
int WDT_GetLoadValue(tWDT_HANDLE* handle, uint16_t* val)
{
    tWDT_REG* baseAddr = WDT;

    if ( handle == NULL )
        return EWBMINVARG;

    *val = (baseAddr->load&WDT_LOAD_FLAG);
    return EWBMOK;
}

/**
  * @brief  Enable a watchdog timer base. if watchdog timer load value is reached to zero, occur system reset.
  * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watchdog timer module
  * @retval  if success, return EWBMOK value
  */
int WDT_Enable(tWDT_HANDLE* handle)
{
    tWDT_REG* baseAddr = WDT;

    if ( handle == NULL )
        return EWBMINVARG;

    baseAddr->ctrl |= WDT_CTRL_WDT_EN;

    return EWBMOK;
}

/**
  * @brief  Disable a watchdog timer base
  * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watchdog timer module
  * @retval  if success, return EWBMOK value
  */
int WDT_Disable(tWDT_HANDLE* handle)
{
    tWDT_REG* baseAddr = WDT;

    if ( handle == NULL )
        return EWBMINVARG;

    baseAddr->ctrl &= ~WDT_CTRL_WDT_EN;

    return EWBMOK;
}

/**
  * @brief  Set a window value for watchdog timer window. if the window wdt used with a ewi wdt, window value should be more than ewi value
  * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watch dog timer module
  * @param  val: setting a window value.
  * @retval  if success, return EWBMOK value
  */
int WDT_SetWindowValue(tWDT_HANDLE* handle, uint16_t val)
{
    tWDT_REG* baseAddr = WDT;

    if ( handle == NULL )
        return EWBMINVARG;

    baseAddr->win = (val&WDT_WIN_FLAG);
    return EWBMOK;
}

/**
  * @brief  Get a window value for watchdog timer window
  * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watch dog timer module
  * @param  val: pointer to a window value
  * @retval  if success, return EWBMOK value
  */
int WDT_GetWindowValue(tWDT_HANDLE* handle, uint16_t* val)
{
    tWDT_REG* baseAddr = WDT;

    if ( handle == NULL || val == NULL )
        return EWBMINVARG;

    *val = (baseAddr->win&WDT_WIN_FLAG);
    return EWBMOK;
}

/**
  * @brief  Enable the Windowed Watchdog timer. The watchdog timer window is used in order to prevent often the WDT count reset
  * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watchdog timer module
  * @retval  if success, return EWBMOK value
  */
int WDT_EnableWindow(tWDT_HANDLE* handle)
{
    tWDT_REG* baseAddr = WDT;

    if ( handle == NULL )
        return EWBMINVARG;

    baseAddr->ctrl |= WDT_CTRL_WWDT_EN;
    return EWBMOK;
}

/**
  * @brief  Disable the Windowedwatchdog timer base
  * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watchdog timer module
  * @retval  if success, return EWBMOK value
  */
int WDT_DisableWindow(tWDT_HANDLE* handle)
{
    tWDT_REG* baseAddr = WDT;

    if ( handle == NULL )
        return EWBMINVARG;

    baseAddr->ctrl &= ~WDT_CTRL_WWDT_EN;

    return EWBMOK;
}

/**
  * @brief  Set a EWI value for early wakeup intrrupt. ewi value should be less than load value
  * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watch dog timermodule
  * @param  val: setting a EWI value. ewi value should be less than load value
  * @retval  if success, return EWBMOK value
  */
int WDT_SetEWIValue(tWDT_HANDLE* handle, uint16_t val)
{
    tWDT_REG* baseAddr = WDT;

    if ( handle == NULL )
        return EWBMINVARG;

    baseAddr->ewi = (val&WDT_EWI_FLAG);
    return EWBMOK;
}

/**
  * @brief  Get a load value for early wakeup intrrupt
  * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watch dog timer module
  * @param  val: pointer to a EWI value
  * @retval  if success, return EWBMOK value
  */
int WDT_GetEWIValue(tWDT_HANDLE* handle, uint16_t* val)
{
    tWDT_REG* baseAddr = WDT;

    if ( handle == NULL || val == NULL )
        return EWBMINVARG;

    *val = (baseAddr->ewi&WDT_EWI_FLAG);
    return EWBMOK;
}

/**
  * @brief  Enable a EWI of watchdog timer. Usually, the watchdog timer count initialize when occurred early wakeup interrupt
  * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watchdog timer module
  * @param  intrCb: pointer to callback function when interrupt occurs
  * @param  intrCbParam: pointer to parameter of callback function
  * @retval  if success, return EWBMOK value
  */
int WDT_EnableEWI(tWDT_HANDLE* handle, WDTCB intrCb, void* intrCbParam)
{
    tWDT_REG* baseAddr = WDT;

    if ( handle == NULL )
        return EWBMINVARG;

    if ( intrCb )
    {
        gWDTIntr.cb = intrCb;
        gWDTIntr.param = intrCbParam;
    }

    baseAddr->ctrl |= WDT_CTRL_EWI_EN;
    return EWBMOK;
}

/**
  * @brief  Disable a EWI of watchdog timer
  * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watchdog timer module
  * @retval  if success, return EWBMOK value
  */
int WDT_DisableEWI(tWDT_HANDLE* handle)
{
    tWDT_REG* baseAddr = WDT;

    if ( handle == NULL )
        return EWBMINVARG;

    gWDTIntr.cb = NULL;
    gWDTIntr.param = NULL;

    baseAddr->ctrl &= ~WDT_CTRL_EWI_EN;
    return EWBMOK;
}

/**
 * @brief  Get status of watchdog
 * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watchdog timer module
 * @retval  status
 */
uint32_t WDT_GetStatus(tWDT_HANDLE* handle)
{
    tWDT_REG* baseaddr = WDT;
    uint32_t status;
    
    if ( handle == NULL )
        return EWBMINVARG;

    status = baseaddr->sts;

    return status;
}

/**
 * @brief  Get part number of watchdog
 * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watchdog timer module
 * @retval  part number
 */
uint16_t WDT_GetPartNum(tWDT_HANDLE* handle)
{
    tWDT_REG* baseaddr = WDT;

    if ( handle == NULL )
        return EWBMINVARG;

    return baseaddr->peri_id0 & WDT_PERI_ID0_PART_NUM;
}

/**
 * @brief  Get configuration number of watchdog
 * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watchdog timer module
 * @retval  part number
 */
uint16_t WDT_GetConfiguration(tWDT_HANDLE* handle)
{
    tWDT_REG* baseaddr = WDT;

    if ( handle == NULL )
        return EWBMINVARG;

    return ((baseaddr->peri_id0 >> 12) & WDT_PERI_ID0_CONFIG_NUM);
}

/**
 * @brief  Get major revision of watchdog
 * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watchdog timer module
 * @retval  part number
 */
uint16_t WDT_GetMajRevision(tWDT_HANDLE* handle)
{
    tWDT_REG* baseaddr = WDT;

    if ( handle == NULL )
        return EWBMINVARG;

    return baseaddr->peri_id1 & WDT_PERI_ID1_MAJ_REV;
}

/**
 * @brief  Get minor revision of watchdog
 * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watchdog timer module
 * @retval  part number
 */
uint16_t WDT_GetMinRevision(tWDT_HANDLE* handle)
{
    tWDT_REG* baseaddr = WDT;

    if ( handle == NULL )
        return EWBMINVARG;

    return ((baseaddr->peri_id1 >> 4) & WDT_PERI_ID1_MIN_REV);
}

/**
 * @brief  Get Reserved number of watchdog
 * @param  handle: pointer to a tWDT_HANDLE structure that contains the configuration information for watchdog timer module
 * @retval  part number
 */
uint16_t WDT_GetReservedNum(tWDT_HANDLE* handle)
{
    tWDT_REG* baseaddr = WDT;

    if ( handle == NULL )
        return EWBMINVARG;

    return ((baseaddr->peri_id1 >> 8) & WDT_PERI_ID1_RSVD_NUM);
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

