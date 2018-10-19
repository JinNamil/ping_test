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

#ifndef __EWBM_SDIO_MASTER_H__
#define __EWBM_SDIO_MASTER_H__

#include "ewbm_device.h"


/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_SDIO_MASTER
  * @{
  */

/** @defgroup SDIO_MASTER_External_Types Exported Types
  * @{
  */


/**
  * @brief  SDIO Master Response enumeration
  */
typedef enum {
    SDIO_M_RESP1 = 0,
    SDIO_M_RESP2,
    SDIO_M_RESP3,
    SDIO_M_RESP4
} eSDIO_M_RESP;

/**
  * @brief  SDIO Master Reset enumeration
  */
typedef enum {
    SDIO_M_RESET_ALL    = 0x00,
    SDIO_M_RESET_CMD    = 0x02,
    SDIO_M_RESET_DATA   = 0x04
} eSDIO_M_RESET;

/**
  * @brief  SDIO Master Transfer mode enumeration
  */
typedef enum {
    SDIO_M_TRANS_WRITE      = 0x00,
    SDIO_M_TRANS_ACMD12     = 0x04,
    SDIO_M_TRANS_ACMD23     = 0x08,
    SDIO_M_TRANS_READ       = 0x10,
    SDIO_M_TRANS_MULTI_BLK  = 0x20,
} eSDIO_M_TRANS;


/**
  * @brief  SDIO Master Response type enumeration
  */
#define SDIO_M_RESP_NONE            (0)
#define SDIO_M_RESP_136             (1)
#define SDIO_M_RESP_48              (2)
#define SDIO_M_RESP_48_BUSY         (3)
#define SDIO_M_CMD_CRC              (1<<3)
#define SDIO_M_CMD_IDX              (1<<4)
#define SDIO_M_CMD_DATA             (1<<5)

#define SDIO_M_RSP_NONE             (SDIO_M_RESP_NONE)
#define SDIO_M_RSP_R1               (SDIO_M_RESP_48      | SDIO_M_CMD_CRC | SDIO_M_CMD_IDX)
#define SDIO_M_RSP_R1b              (SDIO_M_RESP_48_BUSY | SDIO_M_CMD_CRC | SDIO_M_CMD_IDX)
#define SDIO_M_RSP_R2               (SDIO_M_RESP_136     | SDIO_M_CMD_CRC)
#define SDIO_M_RSP_R3               (SDIO_M_RESP_48)
#define SDIO_M_RSP_R4               (SDIO_M_RESP_48)
#define SDIO_M_RSP_R5               (SDIO_M_RESP_48      | SDIO_M_CMD_CRC | SDIO_M_CMD_IDX)
#define SDIO_M_RSP_R6               (SDIO_M_RESP_48      | SDIO_M_CMD_CRC | SDIO_M_CMD_IDX)
#define SDIO_M_RSP_R7               (SDIO_M_RESP_48      | SDIO_M_CMD_CRC | SDIO_M_CMD_IDX)


/**
  * @brief  SDIO Master command structure definition
  */
typedef struct _tSDIO_M_CMD {
    uint8_t     cmdIndex;
    uint32_t    respType;
    uint32_t    cmdArgument;
} tSDIO_M_CMD;

/**
  * @brief  SDIO Master data structure definition
  */
typedef struct _tSDIO_M_DATA {
    uint8_t*    data;
    uint32_t    transMode;
    uint32_t    blocks;
    uint32_t    blockSize;
} tSDIO_M_DATA;


/**
  * @brief  SDIO Master handle structure definition
  */
typedef struct _tSDIO_M_HANDLE {
    uint32_t    maxClock;
    uint32_t    minClock;
    uint32_t    clock;
    uint32_t    busWidth;
    uint32_t    vddBank;
    uint32_t    vddPin;
} tSDIO_M_HANDLE;

/**
  * @}
  */

/** @addtogroup SDIO_MASTER_External_Func
  * @{
  */

/** @addtogroup SDIO_MASTER_External_Func_G1
  * @{
  */

int SDIO_M_Init(tSDIO_M_HANDLE* handle);
int SDIO_M_DeInit(tSDIO_M_HANDLE* handle);
void         SDIO_M_SetUHS(tSDIO_M_HANDLE* handle);
void         SDIO_M_Reset(tSDIO_M_HANDLE* handle, eSDIO_M_RESET mask);

int SDIO_M_SetClock(tSDIO_M_HANDLE* handle, uint32_t clock);
int SDIO_M_SetBusWidth(tSDIO_M_HANDLE* handle, uint32_t busWidth);
uint32_t     SDIO_M_GetResponse(tSDIO_M_HANDLE* handle, eSDIO_M_RESP resp);
int SDIO_M_SendCommand(tSDIO_M_HANDLE* handle, tSDIO_M_CMD* cmd, tSDIO_M_DATA* data);
int SDIO_M_WaitSendCommand(tSDIO_M_HANDLE* handle, tSDIO_M_CMD* cmd, tSDIO_M_DATA* data, bool busy);
void         SDIO_M_WaitCardInt(tSDIO_M_HANDLE* handle);

/**
  * @}
  */

/** @addtogroup SDIO_MASTER_External_Func_G2
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

#endif /*__EWBM_SDIO_MASTER_H__*/
