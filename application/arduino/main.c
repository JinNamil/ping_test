/**
  ******************************************************************************
  * @file    main.c
  * @author  eWBM
  * @version v1.0
  * @date    29-07-2016
  * @brief   Arduino Interface 
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ewbm_global_config.h"


#ifdef __FREE_RTOS__
#include "FreeRTOS.h"
#include "task.h"
#endif

#include "ewbm_delay.h"

#define USE_UART_DEBUG    1

#if USE_UART_DEBUG
    #include "ewbm_uart.h"
    static tUART_HANDLE debugHandle;
    #define ARDUINO_DBG(fmt,args...)    UART_Printf(fmt,##args)
#else
    #define ARDUINO_DBG(fmt,args...)   
#endif

#include "ewbm_spi.h"
#include "ewbm_i2c.h"

typedef struct _spi_xfer_t {
    tSPI_HANDLE *handle;
    uint8_t *input;
    uint8_t *output;
    uint32_t len;
    uint8_t mode;
}spi_xfer_t;

int arduino_i2c_init(tI2C_HANDLE *handle)
{
	int ret = 0;

	if(handle == NULL)	
		return (int)EWBMMEM;

	handle->channel = I2C_CHANNEL_2;
	handle->freq = I2C_FREQ_100KHZ;
	
	ret = (int)I2C_Init(handle);
	if(ret != EWBMOK)	{
		return ret;
	}

	ret = (int)I2C_MasterModeEnable(handle);
	if(ret != EWBMOK)	{
		return ret;
	}

	return ret;
}

int arduino_i2c_master_send(tI2C_HANDLE *handle, 
							uint8_t slave, 
							uint8_t regaddr, 
							uint8_t message)
{
	int ret;
	tI2C_MSG send_msg;
	uint8_t messagebuf[2] = {0, };

	memset((void *)&send_msg, 0, sizeof send_msg);

	send_msg.slaveAddr = slave;
	send_msg.buf = (uint8_t *)&regaddr;
	send_msg.len = sizeof regaddr;

	if((ret = I2C_MasterSend(handle, &send_msg)) != (int)EWBMOK) {
		return ret;
	}

	send_msg.slaveAddr = slave;
	send_msg.buf = (uint8_t *)&message;
	send_msg.len = sizeof message;

	if((ret = I2C_MasterSend(handle, &send_msg)) != (int)EWBMOK) {
		return ret;
	}


	return ret;

}

int arduino_i2c_master_read(tI2C_HANDLE *handle, 
							uint8_t slave, 
							uint8_t *message)
{
	int ret;
	tI2C_MSG recv_msg;
	uint8_t messagebuf[2] = {0, };

	memset((void *)&recv_msg, 0, sizeof recv_msg);

	recv_msg.slaveAddr = slave;
	recv_msg.buf = (uint8_t *)message;
	recv_msg.len = 1;

	if((ret = I2C_MasterSend(handle, &recv_msg)) != (int)EWBMOK) {
		return ret;
	}

	return ret;

}

static void arduino_spi_xfer(spi_xfer_t *t)
{
	do {
		dummy = 0xFF;
		if( t->input != (void *)0 ) {
			dummy = *(t->input++);
		}

		SPI_Write(t->handle, dummy);
		dummy = SPI_Read(t->handle);

		if( t->output != (void *)0 ) {
			*(t->output++) = dummy;
		}


	} while( ++xfer_loop < t->len );
}

void arduino_spi_send_then_recv(tSPI_HANDLE *handle, 
								uint8_t *w_buf, uint32_t w_size, 
								uint8_t *r_buf, uint32_t r_size)
{

	spi_xfer xfer;

	xfer.handle = handle;
	xfer.input = (uint8_t *)w_buf;
	xfer.len = w_size;
	xfer.output = NULL;

	arduino_spi_xfer(&xfer);

	if(r_buf != NULL)	{
		xfer.handle = handle;
		xfer.input = NULL;
		xfer.output = r_buf;
		xfer.len = r_size;

		ardunino_spi_xfer(&xfer);
	}

}


int arduino_spi_init(tSPI_HANDLE *handle)
{

	if(handle == NULL)	{
		return (int)EWBMMEM;
	}

	handle->bank = SPI_BANK_2;
	handle->clockPhase = 0;
	handle->clockPolarity = 0;
	handle->dataFormat = SPI_DSS_08_BIT_DATA;
	handle->dmaMode = 0;
	handle->frameFormat = SPI_MOTOROLA_SPI_FRAME;
	handle->dmaMode = 0;
	handle->intrMode =  0;
	handle->mode = SPI_MASTER_MODE;
	handle->prescale = 0x2;
	handle->reg = (void *)(0x40005000);

	SPI_ModeSelect(handle);
	SPI_Enable(handle);

	return (int)EWBMOK;
}




/*
 * main: initialize and start the system
 */
int main(void)
{
	int ret = EWBMOK;
	tI2C_HANDLE i2c_handle;
	tSPI_HANDLE spi_handle;
	tUART_HANDLE uart_handle;
#if USE_UART_DEBUG
    // Set UART2 handle
    debugHandle.bank = UART_BANK_2;
    debugHandle.baudRate = UART_BAUDRATE_115200;
    debugHandle.wordLength = UART_WORDLENGTH_8BIT;

    // Initialize UART2 handle
    UART_Init(&debugHandle);

    // Enable fifo UART2
    UART_EnableFifo(&debugHandle, UART_FIFO_LEVEL_7_OVER_8, UART_FIFO_LEVEL_1_OVER_8);

    // Enable UART2 to TX mode
    UART_EnableUart(&debugHandle, UART_MODE_TX);

    // Initialize debug with the UART2 handle
    UART_PrintInit(&debugHandle);
#endif

	// TO DO : I2C Initialze
	memset((void *)&i2c_handle, 0, sizeof i2c_handle);
	ret = arduino_i2c_init(&i2c_handle);
	if(ret != EWBMOK)	{
		ARDUINO_DBG("I2C initialize fail ( ret : %d ) \r", ret);
		//TO DO : Error processing
	}
	

	// TO DO : SPI Initialize
	memset((void *)&spi_handle, 0, sizeof spi_handle);
	ret = arduino_spi_init(&spi_handle);
	if(ret != EWBMOK)	{
		ARDUINO_DBG("SPI initialize fail ( ret : %d ) \r", ret);
		//TO DO : Error processing
	}

	// TO DO : SPI Message send using ardunino_spi_send_then_recv
	

	// Set UART1 handle
    uart_handle.bank = EWBM_DEBUG_UART_PORT;
    uart_handle.baudRate = UART_BAUDRATE_115200;
    uart_handle.wordLength = UART_WORDLENGTH_8BIT;

    // Initialize UART1 handle
    UART_Init(&uart_handle);

    // Enable fifo UART1
    UART_EnableFifo(&uart_handle, UART_FIFO_LEVEL_7_OVER_8, UART_FIFO_LEVEL_1_OVER_8);

	// Disable fifo UART1

    // Enable UART1 to TX mode
    UART_EnableUart(&uart_handle, UART_MODE_TXRX);
	
	// TO DO : UART message send and recv 

	// TO DO : UART Initialize


#ifdef __FREE_RTOS__
    //xTaskCreate(APP_GYRO_Test, "GYRO", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);

    for(;;) {
        vTaskDelay(1000);
    }
#else

    for(;;) {
        __m_delay(1000);
    }
#endif

    return 0;
}
