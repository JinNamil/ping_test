/**
  ******************************************************************************
  * @file    ewbm_pmu.h
  * @author  eWBM
  * @version v0.1
  * @date    16-07-2016
  * @brief   Header file of PMU module.
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

#ifndef __EWBM_PMU_H__
#define __EWBM_PMU_H__

#include "ewbm_device.h"

/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @defgroup PMU_External_Types
  * @{
  */

/**
  * @brief  LPM enumeration
  */
typedef enum {
    LPM_NOMAL = 0,
    LPM_1,
    LPM_2,
    LPM_3
} ePMU_LPM;

/**
  * @brief  PMU handle structure definition
  */

typedef struct _tPMU_HANDLE {
    ePMU_LPM     lpm; /* Frame Format */
    tPMU_REG*    reg; /* base address */
} tPMU_HANDLE;

/**
  * @}
  */

/** @addtogroup PMU_External_Func
  * @{
  */

/** @addtogroup PMU_External_Func_G1
  * @{
  */

int PMU_Init(tPMU_HANDLE* handle);
int PMU_DeInit(tPMU_HANDLE* handle);
/**
  * @}
  */

/** @addtogroup PMU_External_Func_G2
  * @{
  */

void PMU_SetLowPowerMode(tPMU_HANDLE* handle, ePMU_LPM mode);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif // __EWBM_PMU_H__

