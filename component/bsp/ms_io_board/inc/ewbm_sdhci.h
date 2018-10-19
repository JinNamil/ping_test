/**
  ******************************************************************************
  * @file    ewbm_sdhci.h
  * @author  eWBM
  * @version v0.1
  * @date    12-07-2016
  * @brief   SDHCI module driver
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

#ifndef __EWBM_SDHCI_H__
#define __EWBM_SDHCI_H__

#include "ewbm_sdio_master.h"

/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_SDHCI
  * @{
  */

/**
  * @brief  SDHCI Vdd voltage defines
  */

#define SDHCI_VDD_165_195   0x00000080    /* VDD voltage 1.65 - 1.95 */
#define SDHCI_VDD_32_33     0x00100000    /* VDD voltage 3.2 ~ 3.3 */
#define SDHCI_VDD_33_34     0x00200000    /* VDD voltage 3.3 ~ 3.4 */

/** @defgroup SDHCI_External_Types
  * @{
  */

/**
  * @brief  SDHCI handle structure definition
  */
typedef struct _tSDHCI_HANDLE {
    tSDIO_M_HANDLE  sdioHandle;
    uint32_t        clock;
    uint32_t        busWidth;
    uint32_t        version;
    uint32_t        voltages;
    uint32_t        ocr;
    uint32_t        rca;
    uint32_t        csd[4];
    uint32_t        cid[4];
    uint32_t        high_capacity;
    uint32_t        dsr;
    uint32_t        dsr_imp;
    uint32_t        write_bl_len;
    uint32_t        read_bl_len;
} tSDHCI_HANDLE;

/**
  * @}
  */

/** @addtogroup SDHCI_External_Func
  * @{
  */

/** @addtogroup SDHCI_External_Func_G1
  * @{
  */

int SDHCI_Init(tSDHCI_HANDLE* handle);
int SDHCI_DeInit(tSDHCI_HANDLE* handle);

/**
  * @}
  */

/** @addtogroup SDHCI_External_Func_G2
  * @{
  */

int SDHCI_WriteBlock(tSDHCI_HANDLE* handle, uint8_t* data, int blkAddr, int blkCnt);
int SDHCI_ReadBlock(tSDHCI_HANDLE* handle, uint8_t* data, int blkAddr, int blkCnt);

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

#endif /*__EWBM_SDHCI_H__*/

