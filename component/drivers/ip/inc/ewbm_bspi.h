/**
  ******************************************************************************
  * @file    ewbm_bspi.h
  * @author  eWBM
  * @version  0.1
  * @date    2016-07-08
  * @brief   Header file of BSPI Device
  ******************************************************************************
  * @attention
  *
  * Copyright(c) 2015 ~ 2020 eWBM Korea , Ltd.
  * All rights reserved.
  * This software contains confidential information of eWBM Korea Co.,Ltd.
  * and unauthorized distribution of this software, or any portion of it, are
  * prohibited.
  *
  * Brief history
  * -------------
  *  2016-07-08 : Draft by Leon Cho
  *
  ******************************************************************************
  */

#ifndef __EWBM_BSPI_H__
#define __EWBM_BSPI_H__

#include "ewbm_device.h"

/** @addtogroup EWBM_Peripheral
 * @{
 */

/** @addtogroup EWBM_BSPI
 * @{
 */

/** @defgroup BSPI_External_Types BSPI Exported Types
 * @{
 */


/**
  * @brief  BSPI mode enumeration
  */
typedef enum
{
    BSPI_SINGLE_MODE = 0,
    BSPI_QUAD_MODE,
} eBSPI_MODE;

/**
  * @}
  */

/** @addtogroup BSPI_External_Func
  * @{
  */
int BSPI_FlashInit(eBSPI_MODE mode);
int BSPI_FlashWrite(uint32_t addr, const uint8_t* data, int size);
int BSPI_FlashRead(uint32_t addr, uint8_t* data, int size);
int BSPI_FlashBlockErase(uint32_t blknum);
int BSPI_FlashSectorErase(uint32_t sctnum);
int BSPI_FlashChipErase(void);

#endif

/**
  * @}
  */

/**
  * @}
  */