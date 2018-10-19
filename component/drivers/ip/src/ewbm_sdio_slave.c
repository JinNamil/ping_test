/**
  ******************************************************************************
  * @file    ewbm_sdio_slave.c
  * @author  eWBM
  * @version v0.1
  * @date    29-06-2016
  * @brief   SDIO Slave module driver
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
#include "ewbm_sdio_slave.h"
#include "ewbm_isr.h"
#include "ewbm_clock.h"
#include "ewbm_gpio.h"
#include "ewbm_delay.h"

#ifdef SDIO_S_DBG_ENABLE
    #include "ewbm_uart.h"
    #define SDIO_S_DBG(fmt,args...)        UART_Printf(fmt,##args)
#else
    #define SDIO_S_DBG(fmt,args...)
#endif

/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_SDIO_SLAVE
  * @{
  */

/** @defgroup SDIO_SLAVE_Internal_Types SDIO_SLAVE Intenal Types
  * @{
  */

/**
  * @brief  SDIO Slave clock reset bit defines
  */
#define SDIO_S_CLK_RESET            (1<<26)

/**
  * @}
  */

/** @addtogroup SDIO_SLAVE_Static_Func
  * @{
  */

/** @addtogroup SDIO_SLAVE_Static_Func_G1
  * @{
  */
//extern tGPIO_HANDLE   gpioMainHandle;
uint32_t tableAddr;

/**
  * @brief  SPI interrupt callback. call the registered callback function according to inerrupt type
  * @param  priv: pointer to parameter of interrupt
  * @retval none
  */
static void sdio_s_intr_cb(void* priv)
{
    tSDIO_S_HANDLE* handle   = (tSDIO_S_HANDLE*)priv;
    tSDIO_S_REG*    baseAddr = SDIO_S;
    uint32_t        glbStatus;
    uint32_t        status;
    uint32_t        admaAddr;

    if ( handle == NULL )
        return;

    baseAddr->glb_int_ena = 0;

    glbStatus = baseAddr->glb_int_sts;
    baseAddr->glb_int_sts = glbStatus;
    if ( glbStatus & SDIO_S_GLB_INT_STS_FN0_INT_TO_ARM )
    {
        status = baseAddr->ahb_fn0_int;
        baseAddr->ahb_fn0_int = status;

        if ( status & SDIO_S_AHB_FN0_INT_AHBSOFT_RST )
        {
            baseAddr->soft_rst_ahb = SDIO_S_SOFT_RST_AHB_AHBSOFT_VALID;
        }

        if ( status & SDIO_S_AHB_FN0_INT_VOLT_SWITCH_CMD )
        {
            tGPIO_HANDLE gpioHandle;

            baseAddr->uhs_support |= SDIO_S_UHS_SUPPORT_MODE;
            baseAddr->uhs_support |= SDIO_S_UHS_SUPPORT_CARD_VOLT_ACCEPTED;

            gpioHandle.bank = handle->vddBank;
            GPIO_Init(&gpioHandle);
            GPIO_SetDir(&gpioHandle, handle->vddPin, 0, 1);
            GPIO_WritePin(&gpioHandle, handle->vddPin, 1);

            //baseAddr->uhs_support |= SDIO_S_UHS_SUPPORT_CARD_VOLT_ACCEPTED;
            baseAddr->uhs_support |= SDIO_S_UHS_SUPPORT_SD_CLK_LINE_SWITCHED;
            baseAddr->uhs_support |= SDIO_S_UHS_SUPPORT_SD_CMD_LINE_SWITCHED;
        }

        if ( status & SDIO_S_AHB_FN0_INT_CMD19_RD_STRT )
        {

        }

        if ( status & SDIO_S_AHB_FN0_INT_CMD19_RD_TRN_OVR )
        {

        }

        if ( status & SDIO_S_AHB_FN0_INT_FN0_WR_STRT )
        {
            admaAddr = baseAddr->adma_sys_addr;
            baseAddr->adma_sys_addr = admaAddr;
        }

        if ( status & SDIO_S_AHB_FN0_INT_FN0_WR_TRN_OVR )
        {

        }

        if ( status & SDIO_S_AHB_FN0_INT_FN0_RD_STRT )
        {
            admaAddr = baseAddr->adma_sys_addr;
            baseAddr->adma_sys_addr = admaAddr;
        }

        if ( status & SDIO_S_AHB_FN0_INT_FN0_RD_OVR )
        {

        }

        if ( status & SDIO_S_AHB_FN0_INT_FN0_RD_ERR )
        {

        }

        if ( status & SDIO_S_AHB_FN0_INT_FN0_ADMA_END )
        {

        }

        if ( status & SDIO_S_AHB_FN0_INT_FN0_ADMA_INT )
        {

        }

        if ( status & SDIO_S_AHB_FN0_INT_FN0_ADMA_ERR )
        {

        }

        baseAddr->ahb_fn0_int = status;
    }

    if ( glbStatus & SDIO_S_GLB_INT_STS_FN1_INT_TO_ARM )
    {
        status = baseAddr->ahb_fn1_int;
        baseAddr->ahb_fn1_int = status;

        if ( status & SDIO_S_AHB_FN1_INT_FN1_WR_OVER )
        {
            //GPIO_WritePin(&gpioMainHandle, GPIO_PIN_0, 1);
            //GPIO_WritePin(&gpioMainHandle, GPIO_PIN_0, 0);
            baseAddr->rddata_rdy = 1;

            if ( handle->writeCb != NULL )
                handle->writeCb(NULL, 0);
        }

        if ( status & SDIO_S_AHB_FN1_INT_FN1_RD_OVER )
        {
            //GPIO_WritePin(&gpioMainHandle, GPIO_PIN_0, 1);
            //GPIO_WritePin(&gpioMainHandle, GPIO_PIN_0, 0);
            baseAddr->rddata_rdy = 1;
            
            if ( handle->readCb != NULL )
                handle->readCb(NULL, 0);
        }

        if ( status & SDIO_S_AHB_FN1_INT_FN1_RD_ERROR )
        {

        }

        if ( status & SDIO_S_AHB_FN1_INT_FUN1_RST )
        {

        }

        if ( status & SDIO_S_AHB_FN1_INT_SD_HOST_FN1_MSG_RDY )
        {
            
        }

        if ( status & SDIO_S_AHB_FN1_INT_FN1_ACK_TO_ARM )
        {
            
        }

        if ( status & SDIO_S_AHB_FN1_INT_FN1_SDIO_RD_START )
        {            
            //GPIO_WritePin(&gpioMainHandle, GPIO_PIN_0, 1);
            //GPIO_WritePin(&gpioMainHandle, GPIO_PIN_0, 0);
            
            baseAddr->rdblkcnt = 0x00;

            admaAddr = baseAddr->adma_sys_addr;
            //baseAddr->adma_sys_addr = admaAddr;
            baseAddr->adma_sys_addr = tableAddr;
        }

        if ( status & SDIO_S_AHB_FN1_INT_FN1_SDIO_WR_START )
        {
            uint32_t data;
            //GPIO_WritePin(&gpioMainHandle, GPIO_PIN_0, 1);
            //GPIO_WritePin(&gpioMainHandle, GPIO_PIN_0, 0);

            data = baseAddr->wrblkcnt;
            baseAddr->wrblkcnt = 0x00;

            data = baseAddr->blksize;
            baseAddr->blksize = data;

            data = baseAddr->argument;
            baseAddr->argument = data;

            data = baseAddr->ahb_sdio_transcnt;
            baseAddr->ahb_sdio_transcnt = data;

            data = baseAddr->csa_pointer;
            baseAddr->csa_pointer = data;

            admaAddr = baseAddr->adma_sys_addr;
            //baseAddr->adma_sys_addr = admaAddr;
            baseAddr->adma_sys_addr = tableAddr;
        }

        if ( status & SDIO_S_AHB_FN1_INT_ADMA_END_INT )
        {

        }

        if ( status & SDIO_S_AHB_FN1_INT_FN1_SUSPEND )
        {

        }

        if ( status & SDIO_S_AHB_FN1_INT_RESUME )
        {

        }

        if ( status & SDIO_S_AHB_FN1_INT_ADMA_INT )
        {

        }

        if ( status & SDIO_S_AHB_FN1_INT_ADMA_ERR )
        {

        }

        if ( status & SDIO_S_AHB_FN1_INT_FUN1_EN )
        {

        }

        admaAddr = baseAddr->adma_sys_addr;
        //baseAddr->adma_sys_addr = admaAddr;
        baseAddr->adma_sys_addr = tableAddr;
    }

    baseAddr->glb_int_ena = SDIO_S_GLB_INT_ENA_ALL;
    baseAddr->rddata_rdy = 1;
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
  * @brief  Initializes the Slave of SPI according to the specified parameters
  * @param  handle: pointer to a tSPI_HANDLE structure that contains the configuration information for SDIO mode
  * @retval  if success, return EWBMOK value
  */

int SDIO_S_Init(tSDIO_S_HANDLE* handle)
{
    tSDIO_S_REG* baseAddr = SDIO_S;    

    if ( handle == NULL )
        return EWBMINVARG;

    // Reset SDIO_S clk
    CLKRST->peri_rst = SDIO_S_CLK_RESET;
    while ( !(CLKRST->peri_rst & SDIO_S_CLK_RESET) );

    while( baseAddr->card_rdy != 0x1 )
        __io_delay(2);

    //=======================================================================
    // 0x3FF232
    // 3 : SHS (Support High Speed), SMPC(support master power control)
    // F : 4BLS(suport 4bit data transfer, low spoedd card),
    //     LSC (low speed device),
    //     S4MI(4bit multi block data transfer),
    //     SBS (support suspend/resume)
    // F : SRW (Suport the Read wait control),
    //     SMB (execute CMD53 in Block mode)
    //     SDC (execute CMD52 while data transfer is in porgress)
    //     SCSI(Support Continuous SPI interrupt)
    // 2 : SD Physical specification 2.00
    // 3 : SDIO specifcation version 2.00
    // 2 : CCCR/FBR version 2.00
    //=====================================================================
    baseAddr->cccr = SDIO_S_CCCR_SHS | SDIO_S_CCCR_SMPC | SDIO_S_CCCR_4BLS | SDIO_S_CCCR_LSC | SDIO_S_CCCR_S4MI |
                     SDIO_S_CCCR_SBS | SDIO_S_CCCR_SRW | SDIO_S_CCCR_SMB | SDIO_S_CCCR_SDC | SDIO_S_CCCR_SCSI |
                     SDIO_S_CCCR_SD_REVISION(SD_VERSION_200) | SDIO_S_CCCR_SDIO_REVISION(SDIO_VERSION_300) | SDIO_S_CCCR_REVISION(CCCR_VERSION_300);

    baseAddr->uhs_support = SDIO_S_UHS_SUPPORT_MODE | SDIO_S_UHS_SUPPORT_DDR_DLY_SELECT;
    baseAddr->clk_delay_timer = 0xFF;

    // IO operational condition register is prgrammed by teh ARM which is used to match with the opreating voltage range of the SD Host
    baseAddr->ocr = 0xFF8000;

    // Function 0,1 interrupt is enable
    baseAddr->glb_int_ena    = SDIO_S_GLB_INT_ENA_ALL;
    baseAddr->ahb_fn0_int_en = SDIO_S_AHB_FN0_INT_EN_ALL;
    baseAddr->ahb_fn1_int_en = SDIO_S_AHB_FN1_INT_EN_ALL;

    //=====================================================================
    // 0x2017
    //    7: This value denotes the SDIO standard interface supported funcion 1
    //    1: CSA support
    //    0: Extendsion of the Standard I/O device code for function 1
    //    2: Function 1 supports high power
    //=====================================================================
    baseAddr->fbr = SDIO_S_FBRIO_DEVICE_CODE_1(7) | SDIO_S_FBRIO_FUN_CSA_SUP | SDIO_S_FBRIO_EXTENDED_IO_DEVICE_CODE_1(0) | SDIO_S_FBRIO_SPS;


    // Support mode SSDR50,SSDR104, SDDR50, SDR50 Type A(SDTA), SDR50 Type C(SDTC), SDR50 Type D(SDTD) SAI(Asynchronous interrupt)
    baseAddr->io_acc_mode = SDIO_S_IO_ACC_MODE_SSDR50 | SDIO_S_IO_ACC_MODE_SSDR104 | SDIO_S_IO_ACC_MODE_SDDR50 |
                            SDIO_S_IO_ACC_MODE_SDTA | SDIO_S_IO_ACC_MODE_SDTC | SDIO_S_IO_ACC_MODE_SDTD | SDIO_S_IO_ACC_MODE_SAI;

    // ARM Processor set this bit to indicate function is ready to operate.
    baseAddr->fun_rdy = SDIO_S_FUNRDY_RDY;

    baseAddr->ahb_transcnt = handle->ahbTranscnt;
    baseAddr->rddata_rdy   = 1;
    baseAddr->ior          = 1;

    external_isr_set((IRQn_Type)SDIO_IRQn, sdio_s_intr_cb, (void*)handle);

    return EWBMOK;
}

/**
  * @brief  De-initializes the Slave of SDIO according to the specified parameters
  * @param  handle: pointer to a tSPI_HANDLE structure that contains the configuration information for SDIO mode
  * @retval  if success, return EWBMOK value
  */

int SDIO_S_DeInit(tSDIO_S_HANDLE* handle)
{
//    tSDIO_S_REG* baseAddr = SDIO_S;

    if ( handle == NULL )
        return EWBMINVARG;


    return EWBMOK;
}

/**
  * @brief  set destination address of DMA
  * @param  handle: pointer to a tSPI_HANDLE structure that contains the configuration information for SDIO mode
  * @param  table : pointer to a destination address
  * @retval  none
  */

void SDIO_S_SetADMATable(tSDIO_S_HANDLE* handle, uint32_t* table)
{
    tSDIO_S_REG* baseAddr = SDIO_S;

    if ( handle == NULL )
        return;

    tableAddr = (uint32_t)table;
    baseAddr->adma_sys_addr = tableAddr;
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

