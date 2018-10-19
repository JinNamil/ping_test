/**
  ******************************************************************************
  * @file    ewbm_dma.c
  * @author  eWBM
  * @version v0.1
  * @date    16-06-2016
  * @brief   DMA module driver
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

#include "ewbm_dma.h"
#include "ewbm_isr.h"
#include "ewbm_clock.h"

#ifdef DBG_ENABLE
#include "ewbm_uart.h"
#define DMA_DBG(fmt,args...)   UART_Printf(fmt,##args)
#else
#define DMA_DBG(fmt,args...)
#endif

#define DMA_TEST_SIZE 500
/** @addtogroup EWBM_Peripheral
 * @{
 */

/** @addtogroup EWBM_DMA
 * @{
 */

/** @defgroup DMA_Internal_Types DMA Intenal Types
 * @{
 */

/**
 * @brief  DMA clock reset bit define
 */
#define DMA_CLK_RESET      (1<<0)

/**
 * @brief  tDMA_INTCB structure that use to register interrupt
 */
typedef struct _tDMA_INTRCB {
    DMACB  cb;
    void*   param;
} tDMA_INTCB;

/**
 * @brief  Internal global arrangement variables that have a structure of tDMA_INTCB
 */
static tDMA_INTCB gDmaIntCb[DMA_CHANNEL_MAX];

/**
 * @}
 */

/** @addtogroup DMA_Static_Func
 * @{
 */

/** @addtogroup DMA_Static_Func_G1
 * @{
 */

/**
 * @brief  Clear the Interrupt status
 * @param  handle: pointer to a tDMA_HANDLE structure that contains the configuration information for DMA module
 * @retval  none
 */
static void dma_int_clear(tDMA_HANDLE* handle)
{
    tDMAC_REG* baseAddr = DMA;

    if ( handle == NULL )
        return;

    if ( baseAddr->intstatus&(1<<handle->channel) )
    {
        if ( baseAddr->interrorstatus&(1<<handle->channel) )
        {
            DMA_DBG("DMAC interrupt error status ERROR!\r\n");
        }

        // clear terminal count interrupt
        DMA_DBG("DMAC Interrupt clear(channel:%d)\r\n", handle->channel);
        baseAddr->inttcclear |= 1<<handle->channel;
        baseAddr->interrclr |= 1<<handle->channel;
    }
}

/**
 * @brief  DMA interrupt callback, call the registered callback function according to inerrupt type
 * @param  priv: pointer to parameter of interrupt
 * @retval none
 */
static void dma_int_cb(void *priv)
{
    tDMA_HANDLE* handle = (tDMA_HANDLE*)priv;
    tDMAC_REG* baseAddr = DMA;

    if ( handle == NULL )
        return;

    if ( baseAddr->intstatus&(1<<handle->channel) && gDmaIntCb[handle->channel].cb )
        gDmaIntCb[handle->channel].cb(gDmaIntCb[handle->channel].param);

    dma_int_clear(handle);
}

/**
 * @brief  Verify source peripheral and destination peripheral
 * @param  handle: pointer to a tDMA_HANDLE structure that contains the configuration information for DMA module
 * @retval  if success, return EWBMOK value
 */
static int dma_check_peri(tDMA_HANDLE* handle)
{
    if ( handle == NULL )
        return EWBMINVARG;

    switch ( handle->transType )
    {
        // m2m check
        case DMA_TRANSFER_TYPE_M2M:
            {
                if ( handle->srcPeri != 0 || handle->dstPeri != 0 )
                    return EWBMINVARG;
            }
            break;
        // m2p check
        case DMA_TRANSFER_TYPE_M2P:
            {
                if ( handle->srcPeri != 0 )
                {
                    return EWBMINVARG;
                }
                else
                {
                    if ( handle->dstPeri != DMA_SPI1_TX && handle->dstPeri != DMA_SPI2_TX && handle->dstPeri != DMA_SPI3_TX &&
                         handle->dstPeri != DMA_UART1_TX && handle->dstPeri != DMA_UART2_TX && handle->dstPeri != DMA_UART3_TX &&
                         handle->dstPeri != DMA_I2C1 && handle->dstPeri != DMA_I2C2 && handle->dstPeri != DMA_I2C3 && handle->dstPeri != DMA_I2C4 )
                    {                    
                        return EWBMINVARG;
                    }
                }
            }
            break;
        // p2m
        case DMA_TRANSFER_TYPE_P2M:
            {
                if ( handle->dstPeri != 0 )
                {
                    return EWBMINVARG;
                }
                else
                {
                    if ( handle->srcPeri != DMA_SPI1_RX && handle->srcPeri != DMA_SPI2_RX && handle->srcPeri != DMA_SPI3_RX &&
                         handle->srcPeri != DMA_UART1_RX && handle->srcPeri != DMA_UART2_RX && handle->srcPeri != DMA_UART3_RX &&
                         handle->srcPeri != DMA_I2C1 && handle->srcPeri != DMA_I2C2 && handle->srcPeri != DMA_I2C3 && handle->srcPeri != DMA_I2C4 )
                    {
                        return EWBMINVARG;
                    }
                }
            }
            break;
        // p2p
        case DMA_TRANSFER_TYPE_P2P:
            {
                if ( handle->dstPeri == 0 && handle->dstPeri == 0 )
                {
                    return EWBMINVARG;
                }
                else
                {
                    if ( handle->srcPeri != DMA_SPI1_RX && handle->srcPeri != DMA_SPI2_RX && handle->srcPeri != DMA_SPI3_RX &&
                         handle->srcPeri != DMA_UART1_RX && handle->srcPeri != DMA_UART2_RX && handle->srcPeri != DMA_UART3_RX &&
                         handle->srcPeri != DMA_I2C1 && handle->srcPeri != DMA_I2C2 && handle->srcPeri != DMA_I2C3 && handle->srcPeri != DMA_I2C4 )
                    {
                        return EWBMINVARG;
                    }

                    if ( handle->dstPeri != DMA_SPI1_TX && handle->dstPeri != DMA_SPI2_TX && handle->dstPeri != DMA_SPI3_TX &&
                         handle->dstPeri != DMA_UART1_TX && handle->dstPeri != DMA_UART2_TX && handle->dstPeri != DMA_UART3_TX &&
                         handle->dstPeri != DMA_I2C1 && handle->dstPeri != DMA_I2C2 && handle->dstPeri != DMA_I2C3 && handle->dstPeri != DMA_I2C4 )
                    {                        
                        return EWBMINVARG;
                    }
                }
            }
            break;

        case DMA_TRANSFER_TYPE_P2M_CP:
            break;

        default:
            return EWBMINVARG;
    }
    return EWBMOK;
}

int DMA_Init(tDMA_HANDLE* handle)
{
    tDMAC_REG* baseAddr = DMA;

    if ( handle == NULL )
        return EWBMINVARG;

    if ( dma_check_peri(handle) != EWBMOK )
        return EWBMINVARG;

    // Reset DMA clk
    CLKRST->peri_rst = DMA_CLK_RESET;
    while ( !(CLKRST->peri_rst & DMA_CLK_RESET) );

    // Clear any pending interrupts on the channel
    baseAddr->inttcclear |= 1<<handle->channel;
    baseAddr->interrclr |= 1<<handle->channel;

    // SMDMAC enable
    baseAddr->configuration |= DMAC_CONFIGURATION_ENABLE;

    return EWBMOK;
}

/**
 * @brief  DeInitializes the DMA
 * @param  handle: pointer to a tDMA_HANDLE structure that contains the configuration information for DMA module
 * @retval  if success, return EWBMOK value
 */
int DMA_DeInit(tDMA_HANDLE* handle)
{
    tDMAC_REG* baseAddr = DMA;

    if ( handle == NULL )
        return EWBMINVARG;

    if ( dma_check_peri(handle) != EWBMOK )
        return EWBMINVARG;

    // SMDMAC disable
    baseAddr->configuration &= ~DMAC_CONFIGURATION_ENABLE;

    if ( gDmaIntCb[handle->channel].cb != NULL )
    {
        gDmaIntCb[handle->channel].cb = NULL;
        gDmaIntCb[handle->channel].param = NULL;

        external_isr_clear((IRQn_Type)DMA_IRQn);
    }

    baseAddr = NULL;

    return EWBMOK;
}

/**
 * @}
 */

/** @addtogroup DMA_External_Func_G2
 *  @brief    Operation functions
 * @{
 */

/**
 * @brief  Enable interrupt of the DMA
 * @param  handle: pointer to a tDMA_HANDLE structure that contains the configuration information for DMA module
 * @param  IntCb: pointer to callback function when interrupt occurs
 * @param  IntCbParam: pointer to parameter of callback function
 * @retval  if success, return EWBMOK value
 */
int DMA_EnableInterrupt(tDMA_HANDLE* handle, DMACB intCb, void* intCbParam)
{
    tDMAC_REG* baseAddr = DMA;

    if ( handle == NULL )
        return EWBMINVARG;

    if ( handle->channel == DMA_CHANNEL_0 )
    {
        DMAC_C0_CONTROL(baseAddr) |= DMAC_CHX_CTRL_TCI_ENABLE;
    }
    else
    {
        DMAC_C1_CONTROL(baseAddr) |= DMAC_CHX_CTRL_TCI_ENABLE;
    }

    if ( intCb )
    {
        gDmaIntCb[handle->channel].cb = intCb;
        gDmaIntCb[handle->channel].param = intCbParam;

        external_isr_set((IRQn_Type)DMA_IRQn, dma_int_cb, (void*)handle);
    }

    return EWBMOK;
}

/**
 * @brief  Disable interrupt of the DMA
 * @param  handle: pointer to a tDMA_HANDLE structure that contains the configuration information for DMA module
 * @retval  if success, return EWBMOK value
 */
int DMA_DisableInterrupt(tDMA_HANDLE* handle)
{
    tDMAC_REG* baseAddr = DMA;

    if ( handle == NULL )
        return EWBMINVARG;

    if ( handle->channel == DMA_CHANNEL_0 )
    {
        DMAC_C0_CONTROL(baseAddr) &= ~(DMAC_CHX_CTRL_TCI_ENABLE);
    }
    else
    {
        DMAC_C1_CONTROL(baseAddr) &= ~(DMAC_CHX_CTRL_TCI_ENABLE);
    }

    gDmaIntCb[handle->channel].cb = NULL;
    gDmaIntCb[handle->channel].param = NULL;

    external_isr_clear((IRQn_Type)DMA_IRQn);
    return EWBMOK;
}

/**
 * @brief  Clear interrupt of the DMA
 * @param  handle: pointer to a tDMA_HANDLE structure that contains the configuration information for DMA module
 * @retval if success, return EWBMOK value
 */
int DMA_ClearInterrupt(tDMA_HANDLE* handle)
{
    tDMAC_REG* baseAddr = DMA;

    if ( handle == NULL )
        return EWBMINVARG;

    if ( dma_check_peri(handle) != EWBMOK )
        return EWBMINVARG;

    // Clear any pending interrupts on the channel
    baseAddr->inttcclear |= 1 << handle->channel;
    baseAddr->interrclr |= 1 << handle->channel;

    // SMDMAC enable
    baseAddr->configuration |= DMAC_CONFIGURATION_ENABLE;

    return EWBMOK;
}

/**
 * @brief  Start the DMA
 * @param  handle: pointer to a tDMA_HANDLE structure that contains the configuration information for DMA module
 * @retval  if success, return EWBMOK value
 */
int DMA_Start(tDMA_HANDLE* handle)
{
    tDMAC_REG* baseAddr = DMA;

    if ( handle == NULL )
        return EWBMINVARG;

    if ( dma_check_peri(handle) != EWBMOK || handle->transLength > MAX_TRANSFER_SIZE )
        return EWBMINVARG;

    // Clear any pending interrupts on the channel
    baseAddr->inttcclear |= 1<<handle->channel;
    baseAddr->interrclr |= 1<<handle->channel;

    if ( handle->channel == DMA_CHANNEL_0 )
    {
        DMAC_C0_SRCADDR(baseAddr) = (uint32_t)handle->srcAddr;
        DMAC_C0_DSTADDR(baseAddr) = (uint32_t)handle->dstAddr;
        DMAC_C0_LLI(baseAddr) = 0;
        DMAC_C0_CONTROL(baseAddr) = ((handle->transLength&DMAC_CHX_CTRL_TRANSFER_SIZE_MASK)|\
                                    ((handle->srcBurst<<12)&DMAC_CHX_CTRL_SB_SIZE_MASK)|((handle->dstBurst<<15)&DMAC_CHX_CTRL_DB_SIZE_MASK)|\
                                    ((handle->srcWidth<<18)&DMAC_CHX_CTRL_SRC_WIDTH_MASK)|((handle->dstWidth<<21)&DMAC_CHX_CTRL_DEST_WIDTH_MASK)|\
                                    (handle->srcIncrese?DMAC_CHX_CTRL_SRC_INCREMENT:0)|(handle->dstIncrese?DMAC_CHX_CTRL_DEST_INCREMENT:0)|DMAC_CHX_CTRL_TCI_ENABLE);
        DMAC_C0_CONFIGURATION(baseAddr) = (DMAC_CHX_CONFIG_ENABLE|(handle->srcPeri>0x0F?(((handle->srcPeri-0x10)<<1)&DMAC_CHX_CONFIG_SRC_PERI_MASK):((handle->srcPeri<<1)&DMAC_CHX_CONFIG_SRC_PERI_MASK))|\
                                          (handle->dstPeri>0x0F?(((handle->dstPeri-0x10)<<6)&DMAC_CHX_CONFIG_DEST_PERI_MASK):((handle->dstPeri<<6)&DMAC_CHX_CONFIG_DEST_PERI_MASK))|\
                                          ((handle->transType<<11)&DMAC_CHX_CONFIG_FLOW_CTRL_MASK)|DMAC_CHX_CONFIG_INT_ERROR|DMAC_CHX_CONFIG_INT_TERMINAL_CNT);
    }
    else
    {
        DMAC_C1_SRCADDR(baseAddr) = (uint32_t)handle->srcAddr;
        DMAC_C1_DSTADDR(baseAddr) = (uint32_t)handle->dstAddr;
        DMAC_C1_LLI(baseAddr) = 0;
        DMAC_C1_CONTROL(baseAddr) = ((handle->transLength&DMAC_CHX_CTRL_TRANSFER_SIZE_MASK)|\
                                    ((handle->srcBurst<<12)&DMAC_CHX_CTRL_SB_SIZE_MASK)|((handle->dstBurst<<15)&DMAC_CHX_CTRL_DB_SIZE_MASK)|\
                                    ((handle->srcWidth<<18)&DMAC_CHX_CTRL_SRC_WIDTH_MASK)|((handle->dstWidth<<21)&DMAC_CHX_CTRL_DEST_WIDTH_MASK)|\
                                    (handle->srcIncrese?DMAC_CHX_CTRL_SRC_INCREMENT:0)|(handle->dstIncrese?DMAC_CHX_CTRL_DEST_INCREMENT:0)|DMAC_CHX_CTRL_TCI_ENABLE);
        DMAC_C1_CONFIGURATION(baseAddr) = (DMAC_CHX_CONFIG_ENABLE|(handle->srcPeri>0x0F?(((handle->srcPeri-0x10)<<1)&DMAC_CHX_CONFIG_SRC_PERI_MASK):((handle->srcPeri<<1)&DMAC_CHX_CONFIG_SRC_PERI_MASK))|\
                                          (handle->dstPeri>0x0F?(((handle->dstPeri-0x10)<<6)&DMAC_CHX_CONFIG_DEST_PERI_MASK):((handle->dstPeri<<6)&DMAC_CHX_CONFIG_DEST_PERI_MASK))|\
                                          ((handle->transType<<11)&DMAC_CHX_CONFIG_FLOW_CTRL_MASK)|DMAC_CHX_CONFIG_INT_ERROR|DMAC_CHX_CONFIG_INT_TERMINAL_CNT);
    }

    return EWBMOK;
}


/**
 * @}
 */

/** @addtogroup UART_External_Func_G3
 *  @brief    Information functions
 * @{
 */

/**
 * @brief  Get part number of DMA
 * @param  handle: pointer to a tDMAC_HANDLE structure that contains the configuration information for DMA module
 * @retval  return part number
 */
uint16_t DMA_GetPartNum(tDMA_HANDLE* handle)
{
    tDMAC_REG* baseAddr = DMA;

    if ( handle == NULL )
        return ((uint16_t)EWBMINVARG);

    return ((DMAC_PERIPH_ID0(baseAddr)&DMAC_PERIPH_ID0_PARTNUM_MASK)|(DMAC_PERIPH_ID1(baseAddr)&DMAC_PERIPH_ID1_PARTNUM_MASK)<<8);
}

/**
 * @brief  Get identification of the designer
 * @param  handle: pointer to a tDMA_HANDLE structure that contains the configuration information for DMA module
 * @retval  return desinger
 */
uint8_t DMA_GetDesigner(tDMA_HANDLE* handle)
{
    tDMAC_REG* baseAddr = DMA;

    if ( handle == NULL )
        return ((uint8_t)EWBMINVARG);

    return ((DMAC_PERIPH_ID1(baseAddr)&DMAC_PERIPH_ID1_DESIGNER_MASK)>>4|(DMAC_PERIPH_ID2(baseAddr)&DMAC_PERIPH_ID2_DESIGNER_MASK)<<4);
}

/**
  * @brief  Get revision number of the DMA
  * @param  handle: pointer to a tDMA_HANDLE structure that contains the configuration information for DMA module
  * @retval  return revision
  */
uint8_t DMA_GetRevision(tDMA_HANDLE* handle)
{
    tDMAC_REG* baseAddr = DMA;

    if ( handle == NULL )
        return ((uint8_t)EWBMINVARG);

    return ((DMAC_PERIPH_ID2(baseAddr)&DMAC_PERIPH_ID2_REVISION_MASK)>>4);
}

/**
  * @brief  Get configuration option of the DMA
  * @param  handle: pointer to a tDMA_HANDLE structure that contains the configuration information for DMA module
  * @retval  return configuration
  */
uint8_t DMA_GetConfiguration(tDMA_HANDLE* handle)
{
    tDMAC_REG* baseAddr = DMA;

    if ( handle == NULL )
        return ((uint8_t)EWBMINVARG);

    return (DMAC_PERIPH_ID3(baseAddr));
}

/**
  * @brief  Get identification of DMA
  * @param  handle: pointer to a tDMA_HANDLE structure that contains the configuration information for DMA module
  * @retval  return identification
  */
uint32_t DMA_GetId(tDMA_HANDLE* handle)
{
    tDMAC_REG* baseAddr = DMA;

    if ( handle == NULL )
        return ((uint32_t)EWBMINVARG);

    return ((DMAC_PCELL_ID0(baseAddr)&DMAC_PCELL_ID0_MASK) |
           ((DMAC_PCELL_ID1(baseAddr)&DMAC_PCELL_ID1_MASK)<<8)|
           ((DMAC_PCELL_ID2(baseAddr)&DMAC_PCELL_ID2_MASK)<<16)|
           ((DMAC_PCELL_ID3(baseAddr)&DMAC_PCELL_ID3_MASK)<<24));
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
