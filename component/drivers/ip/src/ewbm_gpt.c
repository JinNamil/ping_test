/**
  ******************************************************************************
  * @file    ewbm_gpt.c
  * @author  eWBM
  * @version v0.1
  * @date    16-06-2016
  * @brief   GPT(General-Purpose Dual Timer) module driver
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
#include "ewbm_gpt.h"

#include <string.h>
#include "ewbm_isr.h"
#include "ewbm_clock.h"


#ifdef GPT_DBG_ENABLE
    #include "ewbm_uart.h"
    #define GPT_DBG(fmt,args...)        UART_Printf(fmt,##args)
#else
    #define GPT_DBG(fmt,args...)
#endif


/** @addtogroup EWBM_Peripheral
  * @{
  */

/** @addtogroup EWBM_GP_TIMER
  * @{
  */

/** @defgroup GPT_Internal_Types GPT Intenal Types
  * @{
  */

/**
  * @brief  GPT(TMR1/TMR2) clock reset bit defines
  */
#define GPT_CLK_RESET(x)   (1<<(2+x))
 /**
  * @brief  Get base address about channel of i2c
  * @param  channel: channel number of i2c to use
  * @retval pointer to base address
  */

/**
  * @brief  GPT interrupt type enumeration
  */
typedef enum {
    TIMER1_MAT_INTR = 0,    // Timer1 Match Interrupt
    TIMER2_MAT_INTR,        // Timer2 Match Interrupt
    TIMER1_CAP_INTR,        // Timer1 Capture Interrupt
    TIMER2_CAP_INTR,        // Timer2 Capture Interrupt
    TIMER_MAX_INTR
} eGPT_INTRTYPE;

/**
  * @brief  tGPT_INTRCB structure that use to register interrupt
  */
typedef struct _tGPT_INTRCB{
    GPTCB  cb;
    void*  param;
} tGPT_INTRCB;

/**
  * @brief  Internal global arrangement variables that have a structure of tGPT_INTRCB
  */
static tGPT_INTRCB gGPTIntrArray[TIMER_MAX_INTR];

/**
  * @}
  */

/** @addtogroup GPT_Static_Func
  * @{
  */

/** @addtogroup GPT_Static_Func_G1
  * @{
  */

/**
  * @brief  Get base address about banc of general purpose timer
  * @param  bank: bank number of gpt to use
  * @retval pointer to base address
  */
static tGPT_REG* gpt_get_baseaddr(eGPT_BANK bank)
{
    switch ( bank )
    {
        case GPT_BANK0 :
            return (tGPT_REG*)TIMER1;

        case GPT_BANK1 :
            return (tGPT_REG*)TIMER2;

        default :
            return (tGPT_REG*)TIMER2;
    }

    return NULL;
}

/**
  * @brief  Get the Interrupt status
  * @param  addr: pointer to base address
  * @param  status: pointer to get interrupt status value
  * @retval  if success, return EWBMOK value
  */
static int gpt_get_intrsts(tGPT_REG* addr, uint32_t* status)
{
    *status = addr->tmr_int_sts;
    return EWBMOK;
}

/**
  * @brief  Clear the Interrupt status
  * @param  addr: pointer to base address
  * @param  status: clear interrupt status value
  * @retval  if success, return EWBMOK value
  */
static int gpt_clear_intrsts(tGPT_REG* addr, uint32_t status)
{
    addr->tmr_int_sts = status;
    return EWBMOK;
}

/**
  * @brief  GPT interrupt callback. call the registered callback function according to inerrupt type
  * @param  priv: pointer to parameter of interrupt
  * @retval none
  */
static void gpt_intr_cb(void *priv)
{
    uint32_t sts = 0;
    uint32_t i = 0;
    tGPT_HANDLE* handle = (tGPT_HANDLE*)priv;

    if ( handle == NULL || handle->reg == NULL )
        return;

    // get interrupt status
    gpt_get_intrsts(handle->reg, &sts);

    for ( i=0; i<TIMER_MAX_INTR; i++ )
    {
        if ( sts&(1<<i) )
        {
            if ( gGPTIntrArray[i].cb )
            {
                gGPTIntrArray[i].cb(gGPTIntrArray[i].param);
            }
        }
    }
    // clear interrupt status
    gpt_clear_intrsts(handle->reg, sts);
}

/**
  * @}
  */

/**
  * @}
  */


/** @addtogroup GPT_External_Func
  * @{
  */

/** @addtogroup GPT_External_Func_G1
  *  @brief    Initialization and Configuration functions
  * @{
  */

/**
  * @brief  Initializes the GPT according to the specified parameters
  * @param  handle: pointer to a tTIMER_HANDLE structure that contains the configuration information for gp timer module
  * @retval  if success, return EWBMOK value
  */
int GPT_Init(tGPT_HANDLE* handle)
{
    if ( handle == NULL )
        return EWBMINVARG;

    handle->reg = gpt_get_baseaddr(handle->bank);
    if ( handle->reg == NULL )
        return EWBMINVARG;

    // GPT clk reset
    CLKRST->peri_rst = GPT_CLK_RESET(handle->bank);
    while ( !(CLKRST->peri_rst & GPT_CLK_RESET(handle->bank)) );

    // control init
    handle->reg->tmr_ctrl = 0;

    // disable all interrupt
    handle->reg->tmr_int_en = 0;

    // clear gpt isr
    external_isr_clear((IRQn_Type)(TIMER1_IRQn+handle->bank));

    memset(gGPTIntrArray, 0, sizeof(tGPT_INTRCB)*TIMER_MAX_INTR);

    return EWBMOK;
}

/**
  * @brief  DeInitializes the general purpose timer
  * @param  handle: pointer to a tTIMER_HANDLE structure that contains the configuration information for gp timer module
  * @retval  if success, return EWBMOK value
  */
int GPT_DeInit(tGPT_HANDLE* handle)
{
    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    handle->reg->tmr_en = 0;
    handle->reg->tmr_int_en = 0;
    handle->reg = NULL;

    return EWBMOK;
}
/**
  * @}
  */

/** @addtogroup GPT_External_Func_G2
  *  @brief    Operation functions
  * @{
  */

#if 0
int GPT_InterruptEnable(tGPT_HANDLE* handle)
{
    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    if ( handle->type == GP_TMR1_TYPE )
    {
        handle->reg->tmr_int_en |= TIMER1_MAT_INTR;
        handle->reg->tmr_en |= TIMER1_EN;
    }
    else
    {
        handle->reg->tmr_int_en |= TIMER2_MAT_INTR;
        handle->reg->tmr_en |= TIMER2_EN;
    }

    return EWBMOK;
}
#endif
/**
  * @brief  Set to time base configuration
  * @param  handle: pointer to a tTIMER_HANDLE structure that contains the configuration information for gp timer module
  * @retval  if success, return EWBMOK value
  */
int GPT_TimeBaseSetConfig(tGPT_HANDLE* handle, tGPT_BASE_INIT* base, tGPT_CHANNEL_INIT* ch)
{
    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    handle->reg->tmr_ctrl = ((base->count_reset&TIMER_BASE_CNT_RST)|
                              (base->master_channel&TIMER_BASE_MST_SEL)|
                              (base->link_mode&TIMER_BASE_LINK_MODE)|
                              (base->prescaler&TIMER_BASE_PRESCALER));
    if ( ch != 0 )
    {
        if ( handle->type == GP_TMR1_TYPE )
        {

            handle->reg->tmr1_ctrl = ((ch->mode&TIMER_CTRL_MODE)|
                                   (ch->triger_mode&TIMER_CTRL_TRIG_MODE)|
                                   (ch->triger_polarity&TIMER_CTRL_TRIGER_POL)|
                                   (ch->triger_select&TIMER_CTRL_TRIGER_SEL)|
                                   (ch->match_output&TIMER_CTRL_MATCH_OUT)|
                                   (ch->output_mode&TIMER_CTRL_OUT_MODE));

            handle->reg->tmr1_match = ch->match_value;
        }
        else
        if ( handle->type == GP_TMR2_TYPE )
        {
            handle->reg->tmr2_ctrl = ((ch->mode&TIMER_CTRL_MODE)|
                                   (ch->triger_mode&TIMER_CTRL_TRIG_MODE)|
                                   (ch->triger_polarity&TIMER_CTRL_TRIGER_POL)|
                                   (ch->triger_select&TIMER_CTRL_TRIGER_SEL)|
                                   (ch->match_output&TIMER_CTRL_MATCH_OUT)|
                                   (ch->output_mode&TIMER_CTRL_OUT_MODE));
            handle->reg->tmr2_match = ch->match_value;
        }
        else
        {
            return EWBMINVARG;
        }
    }

    return EWBMOK;
}

/**
  * @brief  Start the GPT time Base
  * @param  handle: pointer to a tTIMER_HANDLE structure that contains the configuration information for general purpose timer module
  * @param  intrCb: pointer to callback function when interrupt occurs
  * @param  intrCbParam: pointer to parameter of callback function
  * @retval  if success, return EWBMOK value
  */
int GPT_TimeBaseStart(tGPT_HANDLE* handle, GPTCB intrCb, void* intrCbParam)
{
    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    if ( intrCb )
    {
        // register callback func to array of gpt interrupt

        gGPTIntrArray[TIMER1_MAT_INTR+handle->bank].cb = intrCb;
        gGPTIntrArray[TIMER1_MAT_INTR+handle->bank].param = intrCbParam;

        // enable update interrupt
        handle->reg->tmr_int_en |= (TIMER1_MAT_INTR_EN<<handle->bank);


        // register isr handle
        external_isr_set((IRQn_Type)(TIMER1_IRQn+handle->bank), gpt_intr_cb, (void*)handle);
    }
    // enable counter
    handle->reg->tmr_en |= (TIMER1_EN<<handle->bank);

    return EWBMOK;
}

/**
  * @brief  Stop the GPT time base
  * @param  handle: pointer to a tTIMER_HANDLE structure that contains the configuration information for general purpose timer module
  * @retval  if success, return EWBMOK value
  */
int GPT_TimeBaseStop(tGPT_HANDLE* handle)
{
    if ( handle == NULL || handle->reg == NULL )
        return EWBMINVARG;

    handle->reg->tmr_int_en &= ~(TIMER1_MAT_INTR+handle->bank); // disable update interrupt
    handle->reg->tmr_en &= ~(TIMER1_EN<<handle->bank); // disable counter

    external_isr_clear((IRQn_Type)(TIMER1_IRQn+handle->bank));

    return EWBMOK;
}

/**
 * @brief  Get part number of General-Purpose Dual Timer
 * @param  handle: pointer to a tGPT_HANDLE structure that contains the configuration information for GPT module
 * @retval  part number
 */
uint16_t GPT_GetPartNum(tGPT_HANDLE* handle)
{
    tGPT_REG* baseaddr = gpt_get_baseaddr(handle->bank);

    if ( handle == NULL )
        return EWBMINVARG;

    return baseaddr->peri_id0 & TIMER_PERI_ID0_PART_NUM;
}

/**
 * @brief  Get configuration number of General-Purpose Dual Timer
 * @param  handle: pointer to a tGPT_HANDLE structure that contains the configuration information for GPT module
 * @retval  part number
 */
uint16_t GPT_GetConfiguration(tGPT_HANDLE* handle)
{
    tGPT_REG* baseaddr = gpt_get_baseaddr(handle->bank);

    if ( handle == NULL )
        return EWBMINVARG;

    return baseaddr->peri_id0 & TIMER_PERI_ID0_CONFIG_NUM;
}

/**
 * @brief  Get major revision of General-Purpose Dual Timer
 * @param  handle: pointer to a tGPT_HANDLE structure that contains the configuration information for GPT module
 * @retval  part number
 */
uint16_t GPT_GetMajRevision(tGPT_HANDLE* handle)
{
    tGPT_REG* baseaddr = gpt_get_baseaddr(handle->bank);

    if ( handle == NULL )
        return EWBMINVARG;

    return baseaddr->peri_id1 & TIMER_PERI_ID1_MAJ_REV;
}

/**
 * @brief  Get minor revision of General-Purpose Dual Timer
 * @param  handle: pointer to a tGPT_HANDLE structure that contains the configuration information for GPT module
 * @retval  part number
 */
uint16_t GPT_GetMinRevision(tGPT_HANDLE* handle)
{
    tGPT_REG* baseaddr = gpt_get_baseaddr(handle->bank);

    if ( handle == NULL )
        return EWBMINVARG;

    return baseaddr->peri_id1 & TIMER_PERI_ID1_MIN_REV;
}

/**
 * @brief  Get Reserved number of General-Purpose Dual Timer
 * @param  handle: pointer to a tGPT_HANDLE structure that contains the configuration information for GPT module
 * @retval  part number
 */
uint16_t GPT_GetReservedNum(tGPT_HANDLE* handle)
{
    tGPT_REG* baseaddr = gpt_get_baseaddr(handle->bank);

    if ( handle == NULL )
        return EWBMINVARG;

    return baseaddr->peri_id1 & TIMER_PERI_ID1_RSVD_NUM;
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
