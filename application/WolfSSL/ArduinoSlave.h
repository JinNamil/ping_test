#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ewbm_spi.h"

#define SLAVE_DATA_BUF_MAX (512+16)

typedef enum {
    STATE_NONE = 0,
	STATE_CMD_START,
	STATE_READ_READY_END,
    STATE_READ_READY,
    STATE_WRITE_READY_END,
    STATE_WRITE_READY,
    STATE_READ_DATA,
    STATE_WRITE_DATA,
    STATE_READ_END,
    STATE_READ_END_FINISH,
    STATE_WRITE_END,
    STATE_WRITE_END_FINISH,
    STATE_FINISHED,
    STATE_MAX
}eSlaveState;

typedef struct {
    uint32_t addr;  // 2byte : length, 3byte : addr
    uint16_t datalen;
    uint8_t  cmdcnt;
    uint8_t  cmdparsed;
    uint8_t  is_read;
    uint8_t  is_sslcmd;
    uint16_t ssl_serial_len;
    uint16_t ssl_read_len;
}stSlaveCmd;

typedef void (*delay_cb)(uint32_t );
void ASlave_Process(stSlaveCmd* pSCmd, uint8_t* pSDataBuf, delay_cb pDelayCB);
void ASlave_Handle(tSPI_HANDLE* handle, stSlaveCmd* pSCmd, uint8_t* pSDataBuf);

