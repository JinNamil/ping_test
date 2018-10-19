
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ArduinoSlave.h"

// RLH_FIXME #include "ewbmMS1000_conf.h"
//#include "w5500.h"
//#include "dns.h"
#include "dhcp.h"
#include "Wrapper_ssl.h"

#define IF_SENDRECV_READY  0xF1
#define IF_SENDRECV_END    0xF4
#define IF_CMD_START       0xFE

#define SLAVE_CMD_CNT_MAX  5

//static uint8_t gWizDataBuf[SLAVE_DATA_BUF_MAX];
static uint16_t gWizDataIdx = 0;
static eSlaveState gSState = STATE_NONE;

static uint8_t* pRootCABuf = NULL;
static uint8_t is_makeRootCA = false;
static uint8_t is_first=true;
static uint8_t is_write2m = false;
static uint8_t is_read2m = false;

#ifdef __USE_DEBUG__
#include "ewbm_uart.h"
#define ASLVE_DBG(fmt,args...)        HAL_UART_Printf(fmt,##args)
#else
#define ASLVE_DBG(fmt,args...)
#endif

static void ASlaveParsingCmd(stSlaveCmd* pSCmd, uint8_t cmd)
{
    pSCmd->cmdcnt++;

    if(pSCmd->cmdcnt > SLAVE_CMD_CNT_MAX)
    {
        ASLVE_DBG("ASlaveParsingCmd cmdcnt over max \r\n");
        return;
    }

    if(pSCmd->cmdcnt <= 2) { // 2byte : length
        pSCmd->datalen |= ((uint16_t)cmd << (16-pSCmd->cmdcnt*8));
    } else { // 3byte : addr
        pSCmd->addr |= ((uint32_t)cmd << (16-(pSCmd->cmdcnt-3)*8));
    }

    if(pSCmd->cmdcnt == SLAVE_CMD_CNT_MAX) {
        if(pSCmd->addr & _W5500_SPI_WRITE_)
            pSCmd->is_read = false; // write
        else
            pSCmd->is_read = true; // read

#if 1//def SLAVE_SSL_TEST
        if(pSCmd->addr & HOST_SSL_CMD)
            pSCmd->is_sslcmd = true;
        else
            pSCmd->is_sslcmd = false;
#endif //SLAVE_SSL_TEST

        pSCmd->cmdparsed = true;        

    }
}

static uint8_t checkCnt = 0;
static uint8_t bForceInit = false;
static uint8_t checkForceInit[4] = {0xaf,0xfa,0xe9,0x9e};
static uint8_t tmp[128];
static uint8_t tmpidx = 0;

void ASlave_Handle(tSPI_HANDLE* handle, stSlaveCmd* pSCmd, uint8_t* pSDataBuf)
{
    int state = 0;
    uint8_t data = 0;

    data = SPI_Read(handle);

    if(tmpidx < 128)
        tmp[tmpidx++] = data;

    state = gSState;

    if(state != STATE_NONE) {
        if(checkForceInit[checkCnt] == data)
            checkCnt++;
        else
            checkCnt = 0;

        if(checkCnt == 4) {
            bForceInit = true;
            checkCnt = 0;
            SPI_Write(handle,0);
            return;
        }
    } else {
        checkCnt = 0;
    }

    switch(state) {
        case STATE_NONE :
            if(data == IF_CMD_START)
                gSState = STATE_CMD_START;
            SPI_Write(handle,0);
            break;
        case STATE_CMD_START :
            ASlaveParsingCmd(pSCmd, data);
            if(pSCmd->cmdparsed) {
                if(pSCmd->is_read)
                    gSState = STATE_READ_DATA;
                else
                    gSState = STATE_WRITE_DATA;
                pSCmd->cmdcnt = 0;
                pSCmd->cmdparsed = false;
            }
            SPI_Write(handle,0);
            break;
        case STATE_READ_READY_END :
            gWizDataIdx = 0;
            gSState = STATE_READ_READY;
            SPI_Write(handle, IF_SENDRECV_READY);
            break;
        case STATE_READ_READY :
            if(data == IF_SENDRECV_READY && is_write2m == false)
                is_write2m = true;
			
            if(is_write2m) {
                SPI_Write(handle, pSDataBuf[gWizDataIdx++]);
                if(gWizDataIdx == pSCmd->datalen) {
                    pSCmd->addr = 0;
                    pSCmd->datalen = 0;
                    gSState = STATE_READ_END;
                    is_write2m = false;
                }
            } else {
                SPI_Write(handle,0);
            }
            break;
        case STATE_WRITE_READY_END :
            SPI_Write(handle, IF_SENDRECV_READY);
            gWizDataIdx = 0;
            gSState = STATE_WRITE_READY;                
            break;
        case STATE_WRITE_READY :
            if(data == IF_SENDRECV_READY  && is_read2m == false) { // write 중 IF_SENDRECV_READY 와 동일한 데이터가 들어오는거 방지..
                is_read2m = true;
                SPI_Write(handle,0);
                return;
            }

            if(is_read2m) {
                if(is_makeRootCA) {
                    if(pRootCABuf)
                        pRootCABuf[gWizDataIdx++] = data;
                    else
                        gWizDataIdx++; // ROOT CA BUF alloc fail...
                } else {
                    pSDataBuf[gWizDataIdx++] = data;
                }
                if(gWizDataIdx == pSCmd->datalen) {
                    gSState = STATE_WRITE_END;
                    is_read2m = false;
                }
            }
            SPI_Write(handle,0);
            break;
        case STATE_READ_END_FINISH :
        case STATE_WRITE_END_FINISH :
            if(is_first) {
                SPI_Write(handle, IF_SENDRECV_END);
                is_first=0;
            } else {
                SPI_Write(handle, 0);
            }
            if(data == IF_SENDRECV_END) {
                is_first = true;
                gSState = STATE_NONE;
            }
            break;

        default :
            SPI_Write(handle,0);
            break;
    }

}

void ASlave_Process(stSlaveCmd* pSCmd, uint8_t* pSDataBuf, delay_cb pDelayCB)
{
    int state;

    if(bForceInit) {
        uint8_t memsize[2][8] = { {2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};

        ASLVE_DBG("ASlave_Process Force INIT!!! \r\n");

        memset(pSDataBuf, 0, SLAVE_DATA_BUF_MAX);
        memset(pSCmd, 0, sizeof(stSlaveCmd));
        pSCmd->cmdcnt = 0;
        is_first = true;
        gSState = STATE_NONE;
        state = STATE_NONE;
        gWizDataIdx = 0;

        SSL_Close();
        if(getDHCPState() != 0)
            DHCP_stop();

        /* wizchip initialize*/
        if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
        {
            ASLVE_DBG("22 WIZCHIP Initialized fail.\r\n");
            return;
        }

        bForceInit = false;
    }

    state = gSState;
    switch(state) {
        case STATE_READ_DATA :
#if 1 //def SLAVE_SSL_TEST
            if(pSCmd->is_sslcmd) {
                uint16_t ssl_r_cmd = (pSCmd->addr >> 8);
                switch(ssl_r_cmd){
                    case HOST_SSL_OPEN_CMD:
                        pSDataBuf[0] = SSL_Open();
                        break;

                    case HOST_SSL_IS_CONNECCT_CMD:
                        pSDataBuf[0] = SSL_IsConnect();
                        break;

                    case HOST_SSL_IS_LOAD_VERIFY:
                        pSDataBuf[0] = SSL_IsLoadVerifyBuf();
                        break;

                    case HOST_SSL_GET_ISSUER_CMD:
                        SSL_GetIssuerName(pSDataBuf, pSCmd->datalen);
                        break;
                    case HOST_SSL_GET_SUBJECT_CMD:
                        SSL_GetSubjectName(pSDataBuf, pSCmd->datalen);
                        break;
                    case HOST_SSL_GET_NEXT_ALT_CMD:
                        SSL_GetNextAltname(pSDataBuf, pSCmd->datalen);
                        break;
                    case HOST_SSL_GET_SERIAL_CMD:
                        pSCmd->ssl_serial_len = 0;
                        {
                            uint16_t serial_len = 0;
                            SSL_GetSerialNum(pSDataBuf, pSCmd->datalen, &serial_len);
                            pSCmd->ssl_serial_len = serial_len;
                        }
                        break;
                    case HOST_SSL_GET_SERIAL_SZ_CMD:
//                            pSDataBuf[0] = pSCmd->ssl_serial_len;
                        pSDataBuf[0] = (pSCmd->ssl_serial_len & 0xff00) >> 8;
                        pSDataBuf[1] = (pSCmd->ssl_serial_len & 0x00ff) >> 0;
                        //pSCmd->ssl_serial_len = 0;
                        break;

                    case HOST_SSL_GET_VERSION_CMD:
                        SSL_GetVersion(pSDataBuf, pSCmd->datalen);
                        break;
                    case HOST_SSL_GET_CIPHER_NAME_CMD:
                        SSL_GetCipeherName(pSDataBuf, pSCmd->datalen);
                        break;
                    case HOST_SSL_READ_CMD:
                        pSCmd->ssl_read_len = 0;
                        {
                            uint16_t read_len = 0;
                            //pSCmd->ssl_read_len = 0;
                            SSL_Read(pSDataBuf, pSCmd->datalen, &read_len);
                            pSCmd->ssl_read_len = read_len;
                        }                                                        
                        break;
                    case HOST_SSL_READ_RET_LEN_CMD :
                        pSDataBuf[0] = (pSCmd->ssl_read_len & 0xff00) >> 8;
                        pSDataBuf[1] = (pSCmd->ssl_read_len & 0x00ff) >> 0;
                        //pSCmd->ssl_read_len = 0;
                        break;

                    case HOST_SSL_DHCPALLOC_CMD :
                        pSDataBuf[0] = SSL_DhcpAlloc();
                        break;
                    case HOST_SSL_DHCPGETINFO_CMD :
                        SSL_DhcpGetInfo(pSDataBuf, (uint8_t)pSCmd->datalen);
                        break;
                    case HOST_SSL_GETHOSTBYNAME_CMD :
                        SSL_GetHostByName(pSDataBuf, (uint8_t)pSCmd->datalen);
                        break;

                    default:
                        break;
                }
            }else {
                if(pSCmd->datalen > 1)
                   WIZCHIP_READ_BUF(pSCmd->addr, pSDataBuf, pSCmd->datalen);
                else
                   pSDataBuf[0] = WIZCHIP_READ(pSCmd->addr);
            
            }
#else // SLAVE_SSL_TEST
            if(pSCmd->datalen > 1)
               WIZCHIP_READ_BUF(pSCmd->addr, pSDataBuf, pSCmd->datalen);
            else
               pSDataBuf[0] = WIZCHIP_READ(pSCmd->addr);
#endif // SLAVE_SSL_TEST

            //gWizDataIdx = 0;
                        gSState = STATE_READ_READY_END;
            break;

        case STATE_WRITE_DATA :
            //gWizDataIdx = 0;
                        if(pSCmd->is_sslcmd && (pSCmd->addr >> 8) == HOST_SSL_SET_ROOTCA){
                            //pRootCABuf = XMALLOC(pSCmd->datalen, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                            if(pRootCABuf) {
                                free(pRootCABuf);
                                pRootCABuf = NULL;
                            }
                            pRootCABuf = malloc(pSCmd->datalen);
                            //pRootCABuf = NULL; // test for alloc fail

                            if(pRootCABuf)
                                ASLVE_DBG("*pRootCABuf* alloc(%d) Success\r\n", pSCmd->datalen);
                            else
                                ASLVE_DBG("*pRootCABuf* alloc(%d) FAIL!!\r\n", pSCmd->datalen);
                            is_makeRootCA = true;
                        } else {
                            is_makeRootCA = false;
                        }
                        gSState = STATE_WRITE_READY_END;
                        break;

        case STATE_READ_END :
                        gSState = STATE_READ_END_FINISH;
                        break;

        case STATE_WRITE_END :
#if 1//def SLAVE_SSL_TEST
            if(pSCmd->is_sslcmd) {
                uint16_t ssl_w_cmd = (pSCmd->addr >> 8);
                switch(ssl_w_cmd){
                    case HOST_SSL_CONNECCT_CMD:
                        SSL_Connect(pSDataBuf, pSCmd->datalen);
                        break;
                    case HOST_SSL_CLOSE_CMD:
                        SSL_Close();
                        break;

                    case HOST_SSL_WRITE_CMD:
                        SSL_Write(pSDataBuf, pSCmd->datalen);                                
                        break;

                    case HOST_SSL_SET_PEERVERIFY_CMD:
                        SSL_SetPeerVerify(pSDataBuf);
                        break;

                    case HOST_SSL_SET_ROOTCA:
                        SSL_SetRootCA(pRootCABuf, pSCmd->datalen);
//                            SSL_SetRootCA(pRootCABuf, pSCmd->datalen-100); // test for fail

                        is_makeRootCA = false;
                        //XFREE(pRootCABuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                        if(pRootCABuf)
                            free(pRootCABuf);
                        pRootCABuf = NULL;
                        break;
#if 1 //[[ JYUK_20161010_BEGIN -- set date/time
                    case HOST_SSL_SET_DATE_CMD:
                        SSL_SetDate(pSDataBuf, pSCmd->datalen);
                        break;
                    case HOST_SSL_SET_TIME_CMD:
                        SSL_SetTime(pSDataBuf, pSCmd->datalen);
                        break;
#endif //]] JYUK_20161010_END -- set date/time
                    //[[ dhcp add
                    case HOST_SSL_DHCPINIT_CMD :
                        SSL_DhcpInit(pSDataBuf, pSCmd->datalen);
                        break;

                    case HOST_SSL_DHCPDEINIT_CMD :
                        SSL_DhcpDeInit(pSDataBuf, pSCmd->datalen);
                        break;

                    case HOST_SSL_DNSRUN_CMD :
                        SSL_DnsRun(pSDataBuf, pSCmd->datalen);
                        break;

                    case HOST_SSL_SET_DNSSERVER :
                        SSL_SetDNSServer(pSDataBuf, pSCmd->datalen);
                        break;

                    //]]
                    default:
                        break;                                
                }
            } else {
                if(pSCmd->datalen > 1)
                    WIZCHIP_WRITE_BUF(pSCmd->addr, pSDataBuf, pSCmd->datalen);
                else
                    WIZCHIP_WRITE(pSCmd->addr, pSDataBuf[0]);
            }
#else // SLAVE_SSL_TEST
            if(pSCmd->datalen > 1)
                WIZCHIP_WRITE_BUF(pSCmd->addr, pSDataBuf, pSCmd->datalen);
            else
                WIZCHIP_WRITE(pSCmd->addr, pSDataBuf[0]);
#endif // SLAVE_SSL_TEST
                        pSCmd->addr = 0;
                        pSCmd->datalen = 0;
                        gSState = STATE_WRITE_END_FINISH;
            break;

        default :
            break;
   }

}


