/**
  ******************************************************************************
  * @file    ewbm_sdio_slave.h
  * @author  eWBM
  * @version v0.1
  * @date    29-06-2016
  * @brief   Header file of SPI module.
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

#ifndef __EWBM_SDIO_SLAVE_H__
#define __EWBM_SDIO_SLAVE_H__

#include "ewbm_device.h"


/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_SDIO_SLAVE
  * @{
  */

/** @defgroup SDIO_SLAVE_External_Types Exported Types
  * @{
  */

/**
  * @brief  SDIO Slave callback function type
  */
typedef void (*SDIOSCB)(void*,int);

/**
  * @brief  CCCR Version enumeration
  */

typedef enum _eSDIO_S_CCCR_VER {
    CCCR_VERSION_100    = 0,
    CCCR_VERSION_110,
    CCCR_VERSION_200,
    CCCR_VERSION_300
} eSDIO_S_CCCR_VER;

/**
  * @brief  SDIO Version enumeration
  */

typedef enum _eSDIO_S_SDIO_VER {
    SDIO_VERSION_100    = 0,
    SDIO_VERSION_110,
    SDIO_VERSION_120,
    SDIO_VERSION_200,
    SDIO_VERSION_300
} eSDIO_S_SDIO_VER;

/**
  * @brief  SDIO Version enumeration
  */

typedef enum _eSDIO_S_SD_VER {
    SD_VERSION_101    = 0,
    SD_VERSION_110,
    SD_VERSION_200,
    SD_VERSION_300
} eSDIO_S_SD_VER;


/**
  * @brief  SDIO Slave handle structure definition
  */
typedef struct _tSDIO_S_HANDLE {    
    SDIOSCB               writeCb;
    SDIOSCB               readCb;
    uint32_t              ahbTranscnt;
    uint32_t              vddBank;
    uint32_t              vddPin;
} tSDIO_S_HANDLE;

/**
  * @}
  */

/** @addtogroup SDIO_SLAVE_External_Func
  * @{
  */

/** @addtogroup SDIO_SLAVE_External_Func_G1
  * @{
  */

int SDIO_S_Init(tSDIO_S_HANDLE* handle);
int SDIO_S_DeInit(tSDIO_S_HANDLE* handle);

void SDIO_S_SetADMATable(tSDIO_S_HANDLE* handle, uint32_t* table);

/**
  * @}
  */

/** @addtogroup SDIO_SLAVE_External_Func_G2
  * @{
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

/**
  * @}
  */

#endif /*__EWBM_SDIO_SLAVE_H__*/
