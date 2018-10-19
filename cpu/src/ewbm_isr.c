/**
  ******************************************************************************
  * @file    ewbm_isr.c
  * @author  eWBM
  * @version v0.1
  * @date    16-06-2016
  * @brief   Source file of interrupt service
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
#include "ewbm_isr.h"
#include "ewbm_uart.h"

typedef struct _tISR_INFO 
{
    ISRCB   isrCb;
    void*   priv;
} tISR_INFO;

static tISR_INFO isrInfoVector[INTR_MAX_IRQn];

int external_isr_clear(IRQn_Type idx)
{
    if ( idx >= INTR_MAX_IRQn )
        return -1;

    isrInfoVector[idx].isrCb = (void*)0;
    isrInfoVector[idx].priv = (void*)0;

    NVIC_DisableIRQ(idx);
    return 0;
}

int external_isr_set(IRQn_Type idx, ISRCB isrCb, void* priv)
{
    if ( idx >= INTR_MAX_IRQn )
        return -1;

    if ( isrCb == (void *)0 )
        return -1;

    isrInfoVector[idx].isrCb = isrCb;
    isrInfoVector[idx].priv  = priv;

    NVIC_ClearPendingIRQ(idx);
    NVIC_EnableIRQ(idx);

    return 0;
}

void external_isr_init(void)
{
    IRQn_Type irq;

    __enable_irq();

    for ( irq=SystemInit_IRQn; irq<INTR_MAX_IRQn; irq++ )
    {
        isrInfoVector[irq].isrCb = (void*)0;
        isrInfoVector[irq].priv = (void*)0;

        NVIC_DisableIRQ(irq);
    }
}

#ifndef __FREE_RTOS__
static uint32_t jitter = 0;

void SysTick_Handler(void)
{
    jitter++;

    if ( jitter >= 0xFFFFFFFF )
        jitter = 0;
}

uint32_t get_system_jitter(void)
{
    return jitter;
}
#endif

void NMI_Handler(void)
{
    __attribute__((unused))uint32_t nmi_sts  = SYSCON->nmi_sts;
}

void HardFault_Handler(void)
{
    UART_Printf("HardFault_Handler\r\n");
    while(1);
}

void MemManage_Handler(void)
{
    while(1);
}

void BusFault_Handler(void)
{
    while(1);
}

void UsageFault_Handler(void)
{
    while(1);
}

#ifndef __FREE_RTOS__
void SVC_Handler(void)
{
    while(1);
}

void DebugMon_Handler(void)
{
    while(1);
}

void PendSV_Handler(void)
{
    while(1);
}
#endif

void External_Handler(void)
{
    uint32_t extNum = __get_IPSR() - 0x10;

    if ( isrInfoVector[extNum].isrCb )
        isrInfoVector[extNum].isrCb(isrInfoVector[extNum].priv);
}

