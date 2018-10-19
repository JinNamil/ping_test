/**
  ******************************************************************************
  * @file    ewbm_emmc.h
  * @author  eWBM
  * @version v0.1
  * @date    29-07-2016
  * @brief   Header file of EMMC module driver
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
  
#ifndef __EWBM_EMMC_H__
#define __EWBM_EMMC_H__

#include "ewbm_sdio_master.h"


/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_EMMC
  * @{
  */

/** @defgroup EMMC_External_Types EMMC Exported Types
  * @{
  */

/**
  * @brief  EMMC handle structure definition
  */
typedef struct _tEMMC_HANDLE {
    tSDIO_M_HANDLE  sdioHandle;
    uint32_t        clock;
    uint32_t        busWidth;
} tEMMC_HANDLE;

/**
  * @}
  */

/** @addtogroup EMMC_External_Func
  * @{
  */

/** @addtogroup EMMC_External_Func_G1
  * @{
  */

int EMMC_Init(tEMMC_HANDLE* handle);
int EMMC_DeInit(tEMMC_HANDLE* handle);

/**
  * @}
  */

/** @addtogroup EMMC_External_Func_G2
  * @{
  */

int EMMC_WriteBlock(tEMMC_HANDLE* handle, uint8_t* data, int blkAddr, int blkCnt);
int EMMC_ReadBlock(tEMMC_HANDLE* handle, uint8_t* data, int blkAddr, int blkCnt);

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

#endif /*__EWBM_EMMC_H__*/

