/**
  ******************************************************************************
  * @file    ewbm_sdio_master.c
  * @author  eWBM
  * @version v0.1
  * @date    29-06-2016
  * @brief   SDIO Master module driver
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

/* Header File */
#include "ewbm_sdio_master.h"
#include "ewbm_isr.h"
#include "ewbm_clock.h"
#include "ewbm_delay.h"
#include "ewbm_gpio.h"

#ifdef SDIO_M_DBG_ENABLE
    #include "ewbm_uart.h"
    #define SDIO_M_DBG(fmt,args...)        UART_Printf(fmt,##args)
#else
    #define SDIO_M_DBG(fmt,args...)
#endif

/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_SDIO_MASTER
  * @{
  */

/** @defgroup SDIO_MASTER_Internal_Types SDIO_MASTER Intenal Types
  * @{
  */

/**
  * @brief  SDIO Master clock reset bit defines
  */
#define SDIO_M_CLK_RESET                   (1<<27)



volatile uint32_t* getBitBandAddress(uint32_t address, int bit )
{
    uint32_t addr = address ;
    uint32_t word_band_base = addr & 0xf0000000 ;
    uint32_t bit_band_base = word_band_base | 0x02000000 ;
    uint32_t offset = addr - word_band_base ;

    return  (volatile uint32_t*)(bit_band_base + (offset * 32) + (bit * 4)) ;
}

/**
  * @}
  */

/** @addtogroup SDIO_MASTER_Static_Func
  * @{
  */

/** @addtogroup SDIO_MASTER_Static_Func_G1
  * @{
  */

static int sdio_transfer_data()
{
    tSDIO_M_REG* baseAddr  = SDIO_M;
    uint32_t     intStatus = 0;

    while ( 1 )
    {
        intStatus = baseAddr->int_sts;
        if ( intStatus & SDIO_M_INT_ERR )
            return -1;

        if ( intStatus & SDIO_M_INT_DMA_END )
        {
            baseAddr->int_sts = SDIO_M_INT_DMA_END;
            return 0;
        }
    }

    return -1;
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup SPI_External_Func
  * @{
  */

/** @addtogroup SPI_External_Func_G1
  *  @brief    Initialization and Configuration functions
  * @{
  */

/**
  * @brief  Initialized the Master of SDIO module
  * @param  priv: pointer to tSDIO_M_HANDLE structure
  * @retval if success, return EWBMOK.
  */

int SDIO_M_Init(tSDIO_M_HANDLE* handle)
{
    tSDIO_M_REG* baseAddr = SDIO_M;

    if ( handle == NULL )
        return EWBMINVARG;
        
    CLKRST->peri_rst = SDIO_M_CLK_RESET;
    while ( !(CLKRST->peri_rst & SDIO_M_CLK_RESET) );

    SDIO_M_Reset(handle, SDIO_M_RESET_ALL);

    SDIO_M_SetClock(handle, handle->clock);
    SDIO_M_SetBusWidth(handle, handle->busWidth);
    
        
//    baseAddr->clk_ctrl = (0x1 << 0);
//    baseAddr->host_ctrl = (0x1 << 2) | (0x1 << 1);//rising edge/4bit mode
    
    baseAddr->int_en     = SDIO_M_INT_ALL;
    baseAddr->err_en     = SDIO_M_ERR_ALL;
    
    baseAddr->host_ctrl3 = SDIO_M_CMD_LINE_MODE | SDIO_M_SDMA_ALIGN;
//    baseAddr->host_ctrl2 = 0x3a;
    baseAddr->tout_ctrl  = 0x0D;
    

    return EWBMOK;
}

/**
  * @brief  de-initialized the Master of SDIO module
  * @param  priv: pointer to tSDIO_M_HANDLE structure
  * @retval if success, return EWBMOK.
  */

int SDIO_M_DeInit(tSDIO_M_HANDLE* handle)
{
    tSDIO_M_REG* baseAddr = SDIO_M;

    if ( handle == NULL )
        return EWBMINVARG;

    baseAddr->host_ctrl3 = 0;
    baseAddr->clk_ctrl   = 0;

    return EWBMOK;
}

/**
  * @brief  reset the Master of SDIO
  * @param  handle: pointer to tSDIO_M_HANDLE structure
  * @param  mask  : software reset value
  * @retval none
  */

void SDIO_M_Reset(tSDIO_M_HANDLE* handle, eSDIO_M_RESET mask)
{
    tSDIO_M_REG* baseAddr = SDIO_M;
    int          timeout  = 100;

    baseAddr->sw_rst = mask;
    while ( baseAddr->sw_rst & mask )
    {
        if ( timeout == 0 )
            return;

        timeout--;
        __m_delay(1);
    }
}

/**
  * @brief  set the UHS of SDIO Master module
  * @param  handle: pointer to tSDIO_M_HANDLE structure
  * @retval none
  */

void SDIO_M_SetUHS(tSDIO_M_HANDLE* handle)
{
    tSDIO_M_REG* baseAddr = SDIO_M;
    tGPIO_HANDLE gpioHandle;

    baseAddr->host_ctrl2 = SDIO_M_CTRL_UHS | SDIO_M_CTRL_VDD180 | SDIO_M_CTRL_DRV_TYPE(3);//0x30;

    gpioHandle.bank = handle->vddBank;
    GPIO_Init(&gpioHandle);
    GPIO_SetDir(&gpioHandle, handle->vddPin, 0, 1);
    GPIO_WritePin(&gpioHandle, handle->vddPin, 1);
    
    SYSCON->pdv_sel18 |= 0x6;
}
#if 0
void SDIO_M_SetClockDelay(tSDIO_M_HANDLE* handle, uint8_t fbDly, uint8_t dinDly, uint8_t doutSel)
{
    tSDIO_M_REG* baseAddr = SDIO_M;
    
    uint32_t clkDelay = 0;
    
    clkDelay = SDIO_M_FBCLK_DLY_VAL(fbDly) | SDIO_M_DIN_DLY_VAL(dinDly);
    if ( doutSel > 0 )
        clkDelay |= SDIO_M_DOUT_CLK_SEL;
    
    baseAddr->clk_ctrl2 = clkDelay;
}
#endif
/**
  * @brief  set the Clock of SDIO Master module
  * @param  handle: pointer to tSDIO_M_HANDLE structure
  * @param  clock : clock value
  * @retval if success, return EWBMOK
  */

int SDIO_M_SetClock(tSDIO_M_HANDLE* handle, uint32_t clock)
{
    tSDIO_M_REG* baseAddr = SDIO_M;
    uint32_t     clk      = 0;
    uint32_t     div      = 0;

    if (( handle == NULL ) || ( clock == 0 ))
        return EWBMINVARG;
    
    baseAddr->clk_ctrl   = 0;
    if ( handle->maxClock <= clock )
    {
        div = 1;
    }
    else
    {
        for ( div=2; div<2046; div+=2)
        {
            if ( (handle->maxClock/div) <= clock )
                break;
        }
    }

    div >>= 1;

    clk  = SDIO_M_CLK_DIV((div&0xFF));
    clk |= SDIO_M_CLK_DIV_HI(((div&0x300)>>8));
    clk |= SDIO_M_CLK_INT_EN;

    baseAddr->clk_ctrl2  = 0;//SDIO_M_DIN_DLY_VAL(0xF);
    baseAddr->clk_ctrl   = clk;
    while (( baseAddr->clk_ctrl & SDIO_M_CLK_INT_STBL ) != SDIO_M_CLK_INT_STBL );

    if ( clock > 26000000 )
        baseAddr->host_ctrl |= SDIO_M_CTRL_HISPD;
    else
        baseAddr->host_ctrl &= ~SDIO_M_CTRL_HISPD;

    handle->clock = clock;

    return EWBMOK;
}

/**
  * @brief  set the bus width of SDIO Master module
  * @param  handle: pointer to tSDIO_M_HANDLE structure
  * @param  busWidth : bus width value
  * @retval if success, return EWBMOK
  */

int SDIO_M_SetBusWidth(tSDIO_M_HANDLE* handle, uint32_t busWidth)
{
    tSDIO_M_REG* baseAddr = SDIO_M;

    if ( busWidth == 4 )
    {
        baseAddr->host_ctrl &= ~SDIO_M_CTRL_8BIT;
        baseAddr->host_ctrl |= SDIO_M_CTRL_4BIT;
    }
    else
    if ( busWidth == 8 )
    {
        baseAddr->host_ctrl &= ~SDIO_M_CTRL_4BIT;
        baseAddr->host_ctrl |= SDIO_M_CTRL_8BIT;
    }
    else
    {
        baseAddr->host_ctrl &= ~SDIO_M_CTRL_4BIT;
        baseAddr->host_ctrl &= ~SDIO_M_CTRL_8BIT;
    }

    handle->busWidth = busWidth;

    return EWBMOK;
}

/**
  * @brief  get the response value after send command from master to slave
  * @param  handle: pointer to tSDIO_M_HANDLE structure
  * @param  resp : response number
  * @retval if success, response value
  */

uint32_t SDIO_M_GetResponse(tSDIO_M_HANDLE* handle, eSDIO_M_RESP resp)
{
    tSDIO_M_REG* baseAddr = SDIO_M;

    if ( handle == NULL )
        return EWBMINVARG;

    if ( (resp != SDIO_M_RESP1) && (resp != SDIO_M_RESP2) && (resp != SDIO_M_RESP3) && (resp != SDIO_M_RESP4) )
    {
        return 0;
    }

    return baseAddr->resp[resp];
}

/**
  * @brief  get the response value after send command from master to slave
  * @param  handle: pointer to tSDIO_M_HANDLE structure
  * @param  data : pointer to tSDIO_M_DATA structure
  * @retval if success, response value
  */

int SDIO_M_DataConfig(tSDIO_M_HANDLE* handle, tSDIO_M_DATA* data)
{
    tSDIO_M_REG* baseAddr = SDIO_M;

    if (( handle == NULL ) || ( data == NULL ))
        return EWBMINVARG;

    baseAddr->sdma_addr = (uint32_t)data->data;
    baseAddr->blk_size  = data->blockSize;
    baseAddr->blk_cnt   = data->blocks;
    baseAddr->tran_mode = (data->transMode | SDIO_M_TRAN_DMA_EN | SDIO_M_TRAN_BLKCNT_EN);

    return EWBMOK;
}

/**
  * @brief  send the command and argument data for communication with slave module
  * @param  handle: pointer to tSDIO_M_HANDLE structure
  * @param  data : pointer to tSDIO_M_DATA structure
  * @retval if success, response value
  */

int SDIO_M_SendCommand(tSDIO_M_HANDLE* handle, tSDIO_M_CMD* cmd, tSDIO_M_DATA* data)
{
    tSDIO_M_REG* baseAddr = SDIO_M;

    if (( handle == NULL ) || ( cmd == NULL ))
        return EWBMINVARG;

    baseAddr->int_sts = SDIO_M_INT_ALL;
    baseAddr->err_sts = SDIO_M_ERR_ALL;

    if ( data != NULL )
    {
        baseAddr->sdma_addr = (uint32_t)data->data;
        baseAddr->blk_size  = data->blockSize;
        baseAddr->blk_cnt   = data->blocks;
        baseAddr->tran_mode = (data->transMode | SDIO_M_TRAN_DMA_EN | SDIO_M_TRAN_BLKCNT_EN);
    }

    baseAddr->arg = cmd->cmdArgument;
    baseAddr->cmd = (cmd->respType | (cmd->cmdIndex << 8));

    return EWBMOK;
}

/**
  * @brief  send the command and argument data for communication with slave module
  * @param  handle: pointer to tSDIO_M_HANDLE structure
  * @param  data : pointer to tSDIO_M_DATA structure
  * @param  busy : transfer complete flag
  * @retval if success, response value
  */

int SDIO_M_WaitSendCommand(tSDIO_M_HANDLE* handle, tSDIO_M_CMD* cmd, tSDIO_M_DATA* data, bool busy)
{
    tSDIO_M_REG* baseAddr  = SDIO_M;
    uint32_t     intStatus = 0;
    uint16_t     intMask   = 0;
    int          ret       = 0;

    if (( handle == NULL ) || ( cmd == NULL ))
        return EWBMINVARG;

    intMask = SDIO_M_INT_RESP;
    if ( busy == true )
        intMask |= SDIO_M_INT_DATA_END;

    __io_delay(100);
    while ( 1 )
    {
        intStatus = baseAddr->int_sts;
        if ( intStatus & SDIO_M_INT_ERR )
        {
            ret =  -1;
            break;
        }

        if (( intStatus & intMask ) == intMask )
        {
            baseAddr->int_sts = intMask;
            ret = 0;
            break;
        }
    }

    if (( ret == 0 ) && ( data != NULL ))
    {
        sdio_transfer_data();
    }

    baseAddr->int_sts = SDIO_M_INT_ALL;
    baseAddr->err_sts = SDIO_M_ERR_ALL;

    if ( ret == 0 )
        return EWBMOK;

    return EWBMERR;
}

void SDIO_M_WaitCardInt(tSDIO_M_HANDLE* handle)
{
    tSDIO_M_REG* baseAddr  = SDIO_M;
    uint32_t     intStatus = 0;

    if ( handle == NULL )
        return;

    while ( 1 )
    {
        intStatus = baseAddr->int_sts;
        if (( intStatus & SDIO_M_INT_CARD_INT ) == SDIO_M_INT_CARD_INT )
        {
            baseAddr->int_sts = SDIO_M_INT_CARD_INT;
            break;
        }
    }
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

