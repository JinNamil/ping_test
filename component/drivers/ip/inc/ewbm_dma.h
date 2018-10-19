/**
  ******************************************************************************
  * @file    ewbm_dma.h
  * @author  eWBM
  * @version v0.1
  * @date    16-06-2016
  * @brief   Header file of DMA module.
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

#ifndef __EWBM_DMA_H__
#define __EWBM_DMA_H__

#include "ewbm_device.h"

/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_DMA
  * @{
  */

/** @defgroup DMA_External_Types DMA Exported Types
  * @{
  */

/**
  * @brief  DMA callback function type
  */
typedef void (*DMACB)(void*);

/**
  * @brief  DMA channel enumeration
  */
typedef enum {
    DMA_CHANNEL_0 = 0,
    DMA_CHANNEL_1,
    DMA_CHANNEL_MAX
} eDMA_CHANNEL;

/**
  * @brief  DMA burst size enumeration
  */
typedef enum {
    DMA_BURST_SIZE_1 = 0,
    DMA_BURST_SIZE_4,
    DMA_BURST_SIZE_8,
    DMA_BURST_SIZE_16,
    DMA_BURST_SIZE_32,
    DMA_BURST_SIZE_64,
    DMA_BURST_SIZE_128,
    DMA_BURST_SIZE_256
} eDMA_BURST_SIZE;

/**
  * @brief  DMA transfer width enumeration
  */
typedef enum {
    DMA_TRANSFER_WIDTH_8BIT = 0,
    DMA_TRANSFER_WIDTH_16BIT,
    DMA_TRANSFER_WIDTH_32BIT
} eDMA_TRANSFER_WIDTH;

/**
  * @brief  DMA transfer type enumeration
  */
typedef enum {
    DMA_TRANSFER_TYPE_M2M = 0,  // Memory-to-memory, Controller : PrimeCell DMA
    DMA_TRANSFER_TYPE_M2P,  // Memory-to-peripheral, Controller : PrimeCell DMA
    DMA_TRANSFER_TYPE_P2M,  // Peripheral-to-memory, Controller : PrimeCell DMA
    DMA_TRANSFER_TYPE_P2P,  // Source peripheral-to-destination peripheral, Controller : PrimeCell DMA
    DMA_TRANSFER_TYPE_P2P_DP,  // Destination peripheral-to-source peripheral, Controller : Destination peripheral
    DMA_TRANSFER_TYPE_M2P_CP,  // Memory-to-peripheral, Controller : Peripheral
    DMA_TRANSFER_TYPE_P2M_CP,  // Peripheral-to-memory, Controller : Peripheral
    DMA_TRANSFER_TYPE_P2P_CP  // Source peripheral-to-destination peripheral, Controller : Source peripheral
} eDMA_TRANSFER_TYPE;

/**
  * @brief  DMA peripheral type enumeration
  */
typedef enum {
    DMA_SPI1_TX = 0,  // DMA1
    DMA_SPI1_RX,
    DMA_SPI2_TX,
    DMA_SPI2_RX,
    DMA_UART1_TX,
    DMA_UART1_RX,
    DMA_UART2_TX,
    DMA_UART2_RX,
    DMA_I2C1,
    DMA_I2C2,
    DMA_SDMMC = 9,
    DMA_I2C3,
    DMA_ARIA_TX = 10,
    DMA_I2C4,
    DMA_ARIA_RX = 11,
    DMA_SPI3_TX,
    DMA_SPI3_RX,
    DMA_UART3_TX,
    DMA_UART3_RX,
    DMA_PERI_MAX
} eDMA_PERI;

/**
  * @brief  DMA handle structure definition
  */
typedef struct _tDMA_HANDLE {
    eDMA_CHANNEL        channel;
    eDMA_TRANSFER_TYPE  transType;
    uint32_t            transLength;
    eDMA_BURST_SIZE     srcBurst;
    eDMA_BURST_SIZE     dstBurst;
    eDMA_TRANSFER_WIDTH srcWidth;
    eDMA_TRANSFER_WIDTH dstWidth;
    bool                srcIncrese;
    bool                dstIncrese;
    void*               srcAddr;
    void*               dstAddr;
    eDMA_PERI           srcPeri;
    eDMA_PERI           dstPeri;
} tDMA_HANDLE;

/**
  * @brief  DMA max transfer size definition
  */
#define MAX_TRANSFER_SIZE    4095

/**
  * @}
  */

/** @addtogroup DMA_External_Func
  * @{
  */

/** @addtogroup DMA_External_Func_G1
  * @{
  */

// init/deinit functions
int DMA_Init(tDMA_HANDLE* handle);
int DMA_DeInit(tDMA_HANDLE* handle);

/**
  * @}
  */

/** @addtogroup DMA_External_Func_G2
  * @{
  */

// operation functions
int DMA_EnableInterrupt(tDMA_HANDLE* handle, DMACB intCb, void* intCbParam);
int DMA_DisableInterrupt(tDMA_HANDLE* handle);
int DMA_ClearInterrupt(tDMA_HANDLE* handle);
int DMA_Start(tDMA_HANDLE* handle);

/**
  * @}
  */

/** @addtogroup DMA_External_Func_G3
  * @{
  */

// identification functions
uint16_t DMA_GetPartNum(tDMA_HANDLE* handle);
uint8_t  DMA_GetDesigner(tDMA_HANDLE* handle);
uint8_t  DMA_GetRevision(tDMA_HANDLE* handle);
uint8_t  DMA_GetConfiguration(tDMA_HANDLE* handle);
uint32_t DMA_GetId(tDMA_HANDLE* handle);

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

#endif // __EWBM_DMA_H__
