/**
  ******************************************************************************
  * @file    ewbm_emmc.c
  * @author  eWBM
  * @version v0.1
  * @date    29-07-2016
  * @brief   EMMC module driver
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

#include "ewbm_emmc.h"
#include "ewbm_delay.h"
#include "ewbm_gpio.h"

#define EMMC_DBG_ENABLE

#ifdef EMMC_DBG_ENABLE
    #include "ewbm_uart.h"
    #define EMMC_DBG(fmt,args...)        UART_Printf(fmt,##args)
#else
    #define EMMC_DBG(fmt,args...)
#endif

/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_EMMC
  * @{
  */

/** @addtogroup EMMC_External_Func
  * @{
  */

/** @addtogroup EMMC_External_Func_G1
  *  @brief    Initialization and Deinitialization functions
  * @{
  */

/**
  * @brief  Initializes the eMMC
  * @param  handle: pointer to a tEMMC_HANDLE structure that contains the configuration information for emmc module
  * @retval if success, return EWBMOK value
  */
int EMMC_Init(tEMMC_HANDLE* handle)
{
    tSDIO_M_HANDLE* sdioHandle;
    tSDIO_M_CMD     sdioCmd;
    volatile uint32_t        resp;
    uint32_t        cmdArg;

    if ( handle == NULL )
        return EWBMINVARG;

    sdioHandle = &handle->sdioHandle;

    sdioHandle->maxClock    = 96000000;
    sdioHandle->minClock    = 192000;
    sdioHandle->clock       = 192000;
    sdioHandle->busWidth    = handle->busWidth;
    sdioHandle->vddBank     = GPIO_BANK_1;
    sdioHandle->vddPin      = GPIO_PIN_10;

    SDIO_M_Init(sdioHandle);


    EMMC_DBG("CMD 0\r\n");
    sdioCmd.cmdArgument = 0;
    sdioCmd.cmdIndex    = 0;
    sdioCmd.respType    = SDIO_M_RSP_NONE;
    SDIO_M_SendCommand(sdioHandle, &sdioCmd, NULL);
    __m_delay(1);

    while ( 1 )
    {
        EMMC_DBG("CMD 1\r\n");
        sdioCmd.cmdArgument = 0x40FF8000;
        sdioCmd.cmdIndex    = 1;
        sdioCmd.respType    = SDIO_M_RSP_R1;
        SDIO_M_SendCommand(sdioHandle, &sdioCmd, NULL);
        SDIO_M_WaitSendCommand(sdioHandle, &sdioCmd, NULL, false);
        resp = SDIO_M_GetResponse(sdioHandle, SDIO_M_RESP1);
        if ( resp & 0x80000000 )
        {
            EMMC_DBG("-> card ready!\r\n");
            break;
        }
        else
        {
            EMMC_DBG("-> card busy!\r\n");
            EMMC_DBG("-> %08x\r\n", resp);
        }
        
        __m_delay(1);
    }
    
    __m_delay(1);

    EMMC_DBG("CMD 2\r\n");
    sdioCmd.cmdArgument = 0;
    sdioCmd.cmdIndex    = 2;
    sdioCmd.respType    = SDIO_M_RSP_R2;
    SDIO_M_SendCommand(sdioHandle, &sdioCmd, NULL);
    SDIO_M_WaitSendCommand(sdioHandle, &sdioCmd, NULL, false);
    __m_delay(1);

    EMMC_DBG("CMD 3\r\n");
    sdioCmd.cmdArgument = (0x0 | 0x1 << 16);
    sdioCmd.cmdIndex    = 3;
    sdioCmd.respType    = SDIO_M_RSP_R1;
    SDIO_M_SendCommand(sdioHandle, &sdioCmd, NULL);
    SDIO_M_WaitSendCommand(sdioHandle, &sdioCmd, NULL, false);
    __m_delay(1);

    EMMC_DBG("CMD 7\r\n");
    sdioCmd.cmdArgument = (0x0 | 0x1 << 16);
    sdioCmd.cmdIndex    = 7;
    sdioCmd.respType    = SDIO_M_RSP_R1;
    SDIO_M_SendCommand(sdioHandle, &sdioCmd, NULL);
    SDIO_M_WaitSendCommand(sdioHandle, &sdioCmd, NULL, false);
    __m_delay(1);

    EMMC_DBG("CMD 6\r\n");
    cmdArg = 0;
    if ( handle->busWidth == 4 )
        cmdArg = 1;
    else
    if ( handle->busWidth == 8 )
        cmdArg = 2;
    sdioCmd.cmdArgument = (0x3 << 24) | (0xB7 << 16) | (cmdArg << 8);
    sdioCmd.cmdIndex    = 6;
    sdioCmd.respType    = SDIO_M_RSP_R1b;
    SDIO_M_SendCommand(sdioHandle, &sdioCmd, NULL);
    SDIO_M_WaitSendCommand(sdioHandle, &sdioCmd, NULL, false);
    __m_delay(1);

    EMMC_DBG("CMD 6\r\n");
    cmdArg = 0;
    if ( handle->clock >= 52000000 )
        cmdArg = 1;
    sdioCmd.cmdArgument = (0x3 << 24) | (0xB9 << 16) | (cmdArg << 8);
    sdioCmd.cmdIndex    = 6;
    sdioCmd.respType    = SDIO_M_RSP_R1b;
    SDIO_M_SendCommand(sdioHandle, &sdioCmd, NULL);
    SDIO_M_WaitSendCommand(sdioHandle, &sdioCmd, NULL, false);
    __m_delay(1);

    SDIO_M_SetClock(sdioHandle, handle->clock);
    if ( handle->clock >= 96000000 )
        SDIO_M_SetUHS(sdioHandle);
    __m_delay(1);

    return EWBMOK;
}

/**
  * @brief  Initializes the eMMC
  * @param  handle: pointer to a tEMMC_HANDLE structure that contains the configuration information for emmc module
  * @retval if success, return EWBMOK value
  */
int EMMC_DeInit(tEMMC_HANDLE* handle)
{
    if ( handle == NULL )
        return EWBMINVARG;

    return EWBMOK;

}

/**
  * @}
  */

/** @addtogroup EMMC_External_Func_G2
  * @{
  */

/**
  * @brief  Write the eMMC
  * @param  handle: pointer to a tEMMC_HANDLE structure that contains the configuration information for emmc module
  * @param  data:
  * @param  blkAddr:
  * @param  blkCnt:
  * @retval if success, return EWBMOK value
  */
int EMMC_WriteBlock(tEMMC_HANDLE* handle, uint8_t* data, int blkAddr, int blkCnt)
{
    tSDIO_M_REG*    baseAddr    = SDIO_M;
    tSDIO_M_HANDLE* sdioHandle;
    tSDIO_M_CMD     sdioCmd;
    tSDIO_M_DATA    sdioData;

    if ( handle == NULL )
        return EWBMINVARG;

    sdioHandle = &handle->sdioHandle;
    baseAddr->clk_ctrl2 = SDIO_M_FBCLK_DLY_VAL(0x3F) | SDIO_M_DOUT_CLK_SEL;
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
  * @brief  Read the eMMC
  * @param  handle: pointer to a tEMMC_HANDLE structure that contains the configuration information for emmc module
  * @param  data:
  * @param  blkAddr:
  * @param  blkCnt:
  * @retval if success, return EWBMOK value
  */
int EMMC_ReadBlock(tEMMC_HANDLE* handle, uint8_t* data, int blkAddr, int blkCnt)
{
    tSDIO_M_REG*    baseAddr    = SDIO_M;
    tSDIO_M_HANDLE* sdioHandle;
    tSDIO_M_CMD     sdioCmd;
    tSDIO_M_DATA    sdioData;

    if ( handle == NULL )
        return EWBMINVARG;

    sdioHandle = &handle->sdioHandle;
    baseAddr->clk_ctrl2 = SDIO_M_DIN_DLY_VAL(0x3F);  //SDIO_M_FBCLK_DLY_VAL(0x3F) | SDIO_M_DOUT_CLK_SEL;

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

/**
  * @}
  */
