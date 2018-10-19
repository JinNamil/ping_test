/**
  ******************************************************************************
  * @file    ewbm_pmu.c
  * @author  eWBM
  * @version v0.1
  * @date    16-07-2016
  * @brief   PMU module driver
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

#include "ewbm_pmu.h"
#include "ewbm_isr.h"
#include "ewbm_delay.h"

#ifdef PMU_DBG_ENABLE
    #include "ewbm_uart.h"
    #define PMU_DBG(fmt,args...)    uart_printf(fmt, ##args)
#else
    #define PMU_DBG(fmt,args...)
#endif


/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup PMU_Static_Func_G1
  * @{
  */
  
/**
  * @brief  Get base address about channel of pmu
  * @param  none
  * @retval pointer to base address
  */
static void* pmu_get_baseaddr(void)
{
    return PMU;
}

/**
  * @}
  */

/** @addtogroup PMU_External_Func
  * @{
  */

/** @addtogroup PMU_External_Func_G1
  *  @brief    Initialization and Configuration functions
  * @{
  */

/**
  * @brief  Initializes the PMU according to the specified parameters
  * @param  handle: pointer to a tPMU_HANDLE structure that contains the configuration information for PMU module
  * @retval  if success, return EWBMOK value
  */
int PMU_Init(tPMU_HANDLE* handle)
{
    handle->reg = pmu_get_baseaddr();
    if ( handle->reg == NULL )
        return EWBMINVARG;

    return EWBMOK;
}

/**
  * @brief  DeInitializes the PMU
  * @param  handle: pointer to a tPMU_HANDLE structure that contains the configuration information for PMU
  * @retval  if success, return EWBMOK value
  */

int PMU_DeInit(tPMU_HANDLE* handle)
{
    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    handle->reg = NULL;

    return EWBMOK;
}

/**
  * @}
  */

/** @addtogroup PMU_External_Func_G2
  *  @brief    Initialization and Configuration functions
  * @{
  */

/**
  * @brief   the PMU
  * @param  handle: pointer to a tPMU_HANDLE structure that contains the configuration information for PMU
  * @param  mode: low power mode
  * @retval  if success, return EWBMOK value
  */

extern void clock_init(void);
void PMU_SetLowPowerMode(tPMU_HANDLE* handle, ePMU_LPM mode)
{
    //tPMU_REG* baseAddr = PMU;
    
    handle->reg->lpm_conf = 0x000001FF;

    switch ( mode )
    {
        case LPM_NOMAL:
            handle->reg->lpm_conf = LPM_CONF(0) | LPM_CONF_VOLT_DOMAIN_EN;
            break;

        case LPM_1:
            handle->reg->lpm_conf = LPM_CONF(1) | LPM_CONF_VOLT_DOMAIN_EN;
            break;

        case LPM_2:
            handle->reg->lpm_conf = LPM_CONF(2) | LPM_CONF_VOLT_DOMAIN_EN;
            break;

        case LPM_3:
            handle->reg->lpm_conf = LPM_CONF(3) | LPM_CONF_VOLT_DOMAIN_EN;
            break;
    }

    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    __WFI();
    SCB->SCR &= ~(SCB_SCR_SLEEPDEEP_Msk);
    
    clock_init();
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

