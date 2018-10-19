#ifdef __RTX_RTOS__
/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#elif __FREE_RTOS__
#include "FreeRTOS.h"
#include "task.h"
#else
#include "sleep.h"
#endif

// c standard
#include <time.h>

#include "ewbm_spi.h"
#include "ewbm_gpio.h"
//#include "ewbm_bsp_led.h"
//#include "ewbm_download.h"
#include "ewbm_rtc.h"
#ifdef __USE_DEBUG__
#define DBG_ENABLE
#include "ewbm_uart.h"
static tUART_HANDLE mTestUart_h;
#endif

#include "w5500.h"
#include "dns.h"
#include "dhcp.h"
// RLH_FIXME #include "factory_mode.h"
#include "ArduinoSlave.h"

#define CloseSocket(s)  close(s)

#ifdef DBG_ENABLE
#define HOST_TEST_DBG(fmt,args...)        HAL_UART_Printf(fmt,##args)
#else
#define HOST_TEST_DBG(fmt,args...)
#endif

#ifdef __USE_SSL__
#define SLAVE_SSL_TEST
#endif

static stSlaveCmd  gSCmd;
static uint8_t gWizDataBuf[SLAVE_DATA_BUF_MAX];
static tSPI_HANDLE ssp2Handle;
static tSPI_HANDLE ssp3Handle; /* SPI3 Interface */

static void test_delay(uint32_t ms)
{
#ifdef __FREE_RTOS__
    vTaskDelay(ms);
#elif __RTX_RTOS__
    osDelay(ms);
#else
    m_sleep(ms);
#endif
}

static void  wizchip_select(void)
{
#ifdef EWBMMS1000
    GPIO_SetDir(GPIO_BANK_4, GPIO_PIN_9, 0, SET);
	GPIO_WritePin(GPIO_BANK_4, GPIO_PIN_9, RESET);
#else 
	// RLH_FIXME need to add GPIO access depending on which connection
#endif
}

static void  wizchip_deselect(void)
{
#ifdef EWBMMS1000
    GPIO_SetDir(GPIO_BANK_4, GPIO_PIN_9, 0, SET);
    GPIO_WritePin(GPIO_BANK_4, GPIO_PIN_9, SET);
#else 
	// RLH_FIXME need to add GPIO access depending on which connection
#endif
}

static void  wizchip_write(uint8_t wb)
{
    SPI_Write(&ssp3Handle, wb);
    SPI_Read(&ssp3Handle);
}

static uint8_t wizchip_read(void)
{
    SPI_Write(&ssp3Handle, 0xFF);
    return SPI_Read(&ssp3Handle);
}

void W5500_SPI_Init(void)
{
    int ret;

    /* W5500 GPIO PIN Configuration */
    //TODO
    //HAL_GPIO_SetDir(gpio_port, GPIO_PIN_1, 0, SET);
#ifdef EWBMMS1000
    GPIO_SetDir(GPIO_BANK_4, GPIO_PIN_5, 0, SET);//ready pin setting
    GPIO_WritePin(GPIO_BANK_4, GPIO_PIN_5, SET);//ready pin setting
#else 
	// RLH_FIXME need to add GPIO access depending on which connection
#endif

   	/* SPI3 configuration */
    ssp3Handle.bank = SPI_BANK_3;
    ret = SPI_Init(&ssp3Handle);
    if(ret != EWBMOK){
        HOST_TEST_DBG("HAL_SSP_Init Failed \r\n");
        return;
    }
}

static void SlaveISR_Handler(void* param)
{
	tSPI_HANDLE* handle = (tSPI_HANDLE*)param;

    ASlave_Handle(handle, &gSCmd, gWizDataBuf);
}

static void Slave_Thread(void * pvParam)
{
    int state;
    int resetCnt = 0;
    int checkIdx = 0;
    bool checkfirst = true;
    uint8_t memsize[2][8] = { {2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
//    gSState = STATE_NONE;

	W5500_SPI_Init();
    wizchip_deselect();

    // Wiznet
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
    reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);	


    /* wizchip initialize*/
    if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
    {
        HOST_TEST_DBG("WIZCHIP Initialized fail.\r\n");
        return;
    }

    // spi init
    ssp2Handle.bank = SPI_BANK_2;
    if(SPI_SlaveInit(&ssp2Handle) != EWBMOK){
        HOST_TEST_DBG("HAL_SSP_SlaveInit Failed \r\n");
        return;
    }

    ssp2Handle.intrMode = SPI_INTR_MODE_RT;
    //ssp2Handle.intrMode = SSP_INTR_MODE_RX;    
    SPI_EnableInterrupt(&ssp2Handle, NULL, NULL, SlaveISR_Handler, &ssp2Handle);

    memset(gWizDataBuf, 0, sizeof(gWizDataBuf));
    memset(&gSCmd, 0, sizeof(gSCmd));
    gSCmd.cmdcnt = 0;

    do {
        int i;
        ASlave_Process(&gSCmd, gWizDataBuf, test_delay);
		__io_delay(1);
    }while(1);    
}

/*
 * main: initialize and start the system
 */
int main (void) 
{
#ifdef DBG_ENABLE
    // Set UART handle
    mTestUart_h.bank = UART_BANK_1;
    mTestUart_h.baudRate = UART_BAUDRATE_115200;
    mTestUart_h.wordLength = UART_WORDLENGTH_8BIT;

    // Initialize UART handle
    UART_Init(&mTestUart_h);

    // Enable fifo UART
    UART_EnableFifo(&mTestUart_h, UART_FIFO_LEVEL_7_OVER_8, UART_FIFO_LEVEL_1_OVER_8);

    // Enable UART
    UART_EnableUart(&mTestUart_h, UART_MODE_TX);

    // Initialize debug UART handle
    UART_DebugInit(&mTestUart_h);
#endif   
    // FW Download
    HOST_TEST_DBG("SLAVE START\r\n");
    //factory_verification();
#ifndef DBG_ENABLE
#ifdef EWBMMS1000
	// Only required for MS1000
    download_init();
#endif
#endif
	RTC_GMTimeInit();
    test_delay(100);

#ifdef __RTX_RTOS__
    // Start application test    
    Slave_Thread(NULL);
#elif __FREE_RTOS__
//    xTaskCreate(Slave_Thread, "Slave_Thread", 1024*4+512, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(Slave_Thread, "Slave_Thread", 1024*2, NULL, tskIDLE_PRIORITY, NULL);
#else
    HOST_TEST_DBG("NONE ...\r\n");
#endif

    while(1) {
        test_delay(5000);
    }
}

