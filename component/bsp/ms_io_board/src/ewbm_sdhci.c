/**
  ******************************************************************************
  * @file    ewbm_sdhci.c
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

#include "ewbm_sdhci.h"
#include "ewbm_delay.h"
#include "ewbm_gpio.h"

#ifdef SDHCI_DBG_ENABLE
    #include "ewbm_uart.h"
    #define SDHCI_DBG(fmt,args...)        UART_Printf(fmt,##args)
#else
    #define SDHCI_DBG(fmt,args...)
#endif


/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_SDHCI
  * @{
  */

/** @defgroup SDHCI_Internal_Types
  * @{
  */

/**
  * @brief  SD Version bit defines
  */
#define SD_VERSION_SD    (1U << 31)

#define MAKE_SDMMC_VERSION(a, b, c)    \
    ((((uint32_t)(a)) << 16) | ((uint32_t)(b) << 8) | (uint32_t)(c))
#define MAKE_SD_VERSION(a, b, c)    \
    (SD_VERSION_SD | MAKE_SDMMC_VERSION(a, b, c))


#define SD_VERSION_3            MAKE_SD_VERSION(3, 0, 0)
#define SD_VERSION_2            MAKE_SD_VERSION(2, 0, 0)
#define SD_VERSION_1_0          MAKE_SD_VERSION(1, 0, 0)
#define SD_VERSION_1_10         MAKE_SD_VERSION(1, 10, 0)

/**
  * @brief  SD Status bit defines
  */

#define SDHCI_STATUS_MASK           (~0x0206BF7F)
#define SDHCI_STATUS_SWITCH_ERROR   (1 << 7)
#define SDHCI_STATUS_RDY_FOR_DATA   (1 << 8)
#define SDHCI_STATUS_CURR_STATE     (0xf << 9)
#define SDHCI_STATUS_ERROR          (1 << 19)
#define SDHCI_STATE_PRG              (7 << 9)

#define OCR_BUSY                (0x80000000)
#define OCR_HCS                 (0x40000000)
#define OCR_VOLTAGE_MASK        (0x007FFF80)
#define OCR_ACCESS_MODE         (0x60000000)

/**
  * @brief  SDHCI Max Block length bit defines
  */

#define SDHCI_MAX_BLOCK_LEN    512

/**
  * @}
  */

/** @addtogroup SDHCI_Internal_Func
  * @{
  */

/**
  * @brief  go to idle
  * @param  handle: pointer to a tSDHCI_HANDLE structure that contains the configuration information for sdhci module
  * @retval none
  */
static void sdhci_go_idle(tSDHCI_HANDLE* handle)
{
    tSDIO_M_CMD     sdioCmd;

    sdioCmd.cmdArgument = 0;
    sdioCmd.cmdIndex    = 0;
    sdioCmd.respType    = SDIO_M_RSP_NONE;
    SDIO_M_SendCommand(&handle->sdioHandle, &sdioCmd, NULL);
    __m_delay(1);
}

/**
  * @brief  send if cond
  * @param  handle: pointer to a tSDHCI_HANDLE structure that contains the configuration information for sdhci module
  * @retval if success, return zero
  */
static int sdhci_send_if_cond(tSDHCI_HANDLE* handle)
{
    tSDIO_M_CMD sdioCmd;
    uint32_t    resp;

    sdioCmd.cmdArgument = ((handle->voltages & 0xFF8000) != 0) << 8 | 0xAA;
    sdioCmd.cmdIndex    = 8;
    sdioCmd.respType    = SDIO_M_RSP_R7;
    SDIO_M_SendCommand(&handle->sdioHandle, &sdioCmd, NULL);
    SDIO_M_WaitSendCommand(&handle->sdioHandle, &sdioCmd, NULL, false);
    resp = SDIO_M_GetResponse(&handle->sdioHandle, SDIO_M_RESP1);

    if ( (resp & 0xFF) != 0xAA )
        return -1;
    else
        handle->version = SD_VERSION_2;

    __m_delay(1);

    return 0;
}

/**
  * @brief  send status
  * @param  handle: pointer to a tSDHCI_HANDLE structure that contains the configuration information for sdhci module
  * @retval if success, return zero
  */
static int sdhci_send_status(tSDHCI_HANDLE* handle)
{
    tSDIO_M_CMD sdioCmd;
    uint32_t    resp;

    sdioCmd.cmdArgument = handle->rca << 16;
    sdioCmd.cmdIndex    = 13;
    sdioCmd.respType    = SDIO_M_RSP_R1;

    while ( 1 )
    {
        SDIO_M_SendCommand(&handle->sdioHandle, &sdioCmd, NULL);
        SDIO_M_WaitSendCommand(&handle->sdioHandle, &sdioCmd, NULL, false);
        resp = SDIO_M_GetResponse(&handle->sdioHandle, SDIO_M_RESP1);

        if (( resp & SDHCI_STATUS_RDY_FOR_DATA ) && ( resp & SDHCI_STATUS_CURR_STATE ) != SDHCI_STATE_PRG )
        {
            break;
        }
        else
        if ( resp & SDHCI_STATUS_MASK )
        {
            SDHCI_DBG("Status Error: 0x%08X\n", resp);
            return -1;
        }

        __m_delay(1);
    }

    return 0;

}

/**
  * @brief  send of cond
  * @param  handle: pointer to a tSDHCI_HANDLE structure that contains the configuration information for sdhci module
  * @retval if success, return zero
  */
static int sdhci_send_op_cond(tSDHCI_HANDLE* handle)
{
    tSDIO_M_CMD sdioCmd;
    uint32_t    resp;

    while ( 1 )
    {
        sdioCmd.cmdArgument = 0;
        sdioCmd.cmdIndex    = 55;
        sdioCmd.respType    = SDIO_M_RSP_R1;
        SDIO_M_SendCommand(&handle->sdioHandle, &sdioCmd, NULL);
        SDIO_M_WaitSendCommand(&handle->sdioHandle, &sdioCmd, NULL, false);
        __m_delay(1);

        sdioCmd.cmdIndex    = 41;
        sdioCmd.respType    = SDIO_M_RSP_R3;
        sdioCmd.cmdArgument = (handle->voltages & 0xFF8000);
        if ( handle->version == SD_VERSION_2 )
            sdioCmd.cmdArgument |= OCR_HCS;
        SDIO_M_SendCommand(&handle->sdioHandle, &sdioCmd, NULL);
        SDIO_M_WaitSendCommand(&handle->sdioHandle, &sdioCmd, NULL, false);
        resp = SDIO_M_GetResponse(&handle->sdioHandle, SDIO_M_RESP1);
        if ( resp & OCR_BUSY )
            break;

        __m_delay(1);
    }

    if ( handle->version != SD_VERSION_2 )
        handle->version = SD_VERSION_1_0;

    handle->ocr = resp;
    handle->high_capacity = ((handle->ocr & OCR_HCS) == OCR_HCS);
    handle->rca = 0;
    __m_delay(1);

    return 0;
}

/**
  * @brief  startup
  * @param  handle: pointer to a tSDHCI_HANDLE structure that contains the configuration information for sdhci module
  * @retval if success, return zero
  */
static int sdhci_startup(tSDHCI_HANDLE* handle)
{
    tSDIO_M_CMD sdioCmd;
    uint32_t    resp;

    sdioCmd.cmdArgument = 0;
    sdioCmd.cmdIndex    = 2;
    sdioCmd.respType    = SDIO_M_RSP_R2;
    SDIO_M_SendCommand(&handle->sdioHandle, &sdioCmd, NULL);
    SDIO_M_WaitSendCommand(&handle->sdioHandle, &sdioCmd, NULL, false);
    handle->cid[0] = SDIO_M_GetResponse(&handle->sdioHandle, SDIO_M_RESP1);
    handle->cid[1] = SDIO_M_GetResponse(&handle->sdioHandle, SDIO_M_RESP2);
    handle->cid[2] = SDIO_M_GetResponse(&handle->sdioHandle, SDIO_M_RESP3);
    handle->cid[3] = SDIO_M_GetResponse(&handle->sdioHandle, SDIO_M_RESP4);
    __m_delay(1);

    sdioCmd.cmdArgument = handle->rca << 16;
    sdioCmd.cmdIndex    = 3;
    sdioCmd.respType    = SDIO_M_RSP_R6;
    SDIO_M_SendCommand(&handle->sdioHandle, &sdioCmd, NULL);
    SDIO_M_WaitSendCommand(&handle->sdioHandle, &sdioCmd, NULL, false);
    resp = SDIO_M_GetResponse(&handle->sdioHandle, SDIO_M_RESP1);
    handle->rca = (resp >> 16) & 0xFFFF;
    __m_delay(1);

    sdioCmd.cmdArgument = handle->rca << 16;
    sdioCmd.cmdIndex    = 9;
    sdioCmd.respType    = SDIO_M_RSP_R2;
    SDIO_M_SendCommand(&handle->sdioHandle, &sdioCmd, NULL);
    SDIO_M_WaitSendCommand(&handle->sdioHandle, &sdioCmd, NULL, false);
    handle->csd[0] = SDIO_M_GetResponse(&handle->sdioHandle, SDIO_M_RESP1);
    handle->csd[1] = SDIO_M_GetResponse(&handle->sdioHandle, SDIO_M_RESP2);
    handle->csd[2] = SDIO_M_GetResponse(&handle->sdioHandle, SDIO_M_RESP3);
    handle->csd[3] = SDIO_M_GetResponse(&handle->sdioHandle, SDIO_M_RESP4);

    sdhci_send_status(handle);
    
    __m_delay(1);


    handle->dsr          = 0xFFFFFFFF;
    handle->dsr_imp      = ((handle->csd[1] >> 12) & 0x1);
    handle->read_bl_len  = 1 << ((handle->csd[1] >> 16) & 0xF);
    handle->write_bl_len = handle->read_bl_len;

    if ( handle->read_bl_len > SDHCI_MAX_BLOCK_LEN )
        handle->read_bl_len = SDHCI_MAX_BLOCK_LEN;

    if ( handle->write_bl_len > SDHCI_MAX_BLOCK_LEN )
        handle->write_bl_len = SDHCI_MAX_BLOCK_LEN;

    if ( (handle->dsr_imp) && (0xFFFFFFFF != handle->dsr) )
    {
        sdioCmd.cmdArgument = handle->rca << 16;
        sdioCmd.cmdIndex    = 4;
        sdioCmd.respType    = SDIO_M_RSP_NONE;
        SDIO_M_SendCommand(&handle->sdioHandle, &sdioCmd, NULL);
         __m_delay(1);
    }

    sdioCmd.cmdArgument = handle->rca << 16;
    sdioCmd.cmdIndex    = 7;
    sdioCmd.respType    = SDIO_M_RSP_R1;
    SDIO_M_SendCommand(&handle->sdioHandle, &sdioCmd, NULL);
    SDIO_M_WaitSendCommand(&handle->sdioHandle, &sdioCmd, NULL, false);
     __m_delay(1);

    if ( handle->busWidth == 4 )
    {
        sdioCmd.cmdArgument = handle->rca << 16;
        sdioCmd.cmdIndex    = 55;
        sdioCmd.respType    = SDIO_M_RSP_R1;
        SDIO_M_SendCommand(&handle->sdioHandle, &sdioCmd, NULL);
        SDIO_M_WaitSendCommand(&handle->sdioHandle, &sdioCmd, NULL, false);
        __m_delay(1);


        sdioCmd.cmdArgument = 2;
        sdioCmd.cmdIndex    = 6;
        sdioCmd.respType    = SDIO_M_RSP_R1;
        SDIO_M_SendCommand(&handle->sdioHandle, &sdioCmd, NULL);
        SDIO_M_WaitSendCommand(&handle->sdioHandle, &sdioCmd, NULL, false);
        __m_delay(1);

        SDIO_M_SetBusWidth(&handle->sdioHandle, handle->busWidth);
    }

    SDIO_M_SetClock(&handle->sdioHandle, handle->clock);
    __m_delay(1);

    return 0;
}
/**
  * @}
  */

/** @addtogroup SDHCI_External_Func_G1
  * @{
  */

/**
  * @brief  Initializes the SDHCI
  * @param  handle: pointer to a tSDHCI_HANDLE structure that contains the configuration information for sdhci module
  * @retval if success, return EWBMOK value
  */
int SDHCI_Init(tSDHCI_HANDLE* handle)
{
    tSDIO_M_HANDLE* sdioHandle;

    if ( handle == NULL )
        return EWBMINVARG;

    sdioHandle = &handle->sdioHandle;

    sdioHandle->maxClock    = 96000000;
    sdioHandle->minClock    = 192000;
    sdioHandle->clock       = 192000;
    sdioHandle->busWidth    = handle->busWidth;
    sdioHandle->vddBank     = GPIO_BANK_1;
    sdioHandle->vddPin      = GPIO_PIN_4;
    SDIO_M_Init(sdioHandle);

    sdhci_go_idle(handle);
    sdhci_send_if_cond(handle);
    sdhci_send_op_cond(handle);

    sdhci_startup(handle);


    return EWBMOK;

}

/**
  * @brief  de-initializes the SDHCI
  * @param  handle: pointer to a tSDHCI_HANDLE structure that contains the configuration information for sdhci module
  * @retval if success, return EWBMOK value
  */
int SDHCI_DeInit(tSDHCI_HANDLE* handle)
{
    if ( handle == NULL )
        return EWBMINVARG;

    return EWBMOK;

}
/**
  * @}
  */

/** @addtogroup SDHCI_External_Func_G2
  * @{
  */

/**
  * @brief  Write the SDHCI
  * @param  handle: pointer to a tSDHCI_HANDLE structure that contains the configuration information for sdhci module
  * $param  data  : write data buffer
  * $param  blkCnt: write block count
  * @retval if success, return EWBMOK value
  */
int SDHCI_WriteBlock(tSDHCI_HANDLE* handle, uint8_t* data, int blkAddr, int blkCnt)
{
    tSDIO_M_HANDLE* sdioHandle;
    tSDIO_M_CMD     sdioCmd;
    tSDIO_M_DATA    sdioData;

    if ( handle == NULL )
        return EWBMINVARG;

    sdioHandle = &handle->sdioHandle;

    sdioCmd.cmdArgument = handle->write_bl_len;
    sdioCmd.cmdIndex    = 16;
    sdioCmd.respType    = SDIO_M_RSP_R1;
    SDIO_M_SendCommand(sdioHandle, &sdioCmd, NULL);
    SDIO_M_WaitSendCommand(sdioHandle, &sdioCmd, NULL, false);
    
//    SDIO_M_SetClockDelay(sdioHandle, 0x3F, 0, 1);
    
    sdioData.blocks     = blkCnt;
    sdioData.blockSize  = 512;
    sdioData.data       = data;
    sdioData.transMode  = SDIO_M_TRANS_WRITE;
    if ( blkCnt > 1 )
        sdioData.transMode |= SDIO_M_TRANS_ACMD12 | SDIO_M_TRANS_MULTI_BLK;

    sdioCmd.cmdArgument = blkAddr;
    if ( blkCnt > 1 )
        sdioCmd.cmdIndex = 25;
    else
        sdioCmd.cmdIndex = 24;
    sdioCmd.respType    = SDIO_M_RSP_R1 | SDIO_M_CMD_DATA;
    SDIO_M_SendCommand(sdioHandle, &sdioCmd, &sdioData);
    SDIO_M_WaitSendCommand(sdioHandle, &sdioCmd, &sdioData, true);

    return EWBMOK;
}

/**
  * @brief  Initializes the SDHCI
  * @param  handle : pointer to a tSDHCI_HANDLE structure that contains the configuration information for sdhci module
  * @param  data   : read data buffer
  * @param  blkAddr: block address
  * @param  blkCnt : block count
  * @retval if success, return EWBMOK value
  */
int SDHCI_ReadBlock(tSDHCI_HANDLE* handle, uint8_t* data, int blkAddr, int blkCnt)
{
    tSDIO_M_HANDLE* sdioHandle;
    tSDIO_M_CMD     sdioCmd;
    tSDIO_M_DATA    sdioData;

    if ( handle == NULL )
        return EWBMINVARG;

    sdioHandle = &handle->sdioHandle;

    sdioCmd.cmdArgument = handle->read_bl_len;
    sdioCmd.cmdIndex    = 16;
    sdioCmd.respType    = SDIO_M_RSP_R1;
    SDIO_M_SendCommand(sdioHandle, &sdioCmd, NULL);
    SDIO_M_WaitSendCommand(sdioHandle, &sdioCmd, NULL, false);

    sdioData.blocks     = blkCnt;
    sdioData.blockSize  = 512;
    sdioData.data       = data;
    sdioData.transMode  = SDIO_M_TRANS_READ;
    if ( blkCnt > 1 )
        sdioData.transMode |= SDIO_M_TRANS_ACMD12 | SDIO_M_TRANS_MULTI_BLK;

    sdioCmd.cmdArgument = blkAddr;
    if ( blkCnt > 1 )
        sdioCmd.cmdIndex = 18;
    else
        sdioCmd.cmdIndex = 17;
    sdioCmd.respType    = SDIO_M_RSP_R1 | SDIO_M_CMD_DATA;
    SDIO_M_SendCommand(sdioHandle, &sdioCmd, &sdioData);
    SDIO_M_WaitSendCommand(sdioHandle, &sdioCmd, &sdioData, true);

    return EWBMOK;
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

