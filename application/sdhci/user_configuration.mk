# User Configuration

#
# Options
#
CHIP_NAME        ?= MS500
DebugBuild       ?= y
JtagBuild        ?= n
# If you change QSPIBoot, you have to check linker script(.ld) file. 
QSPIBoot         ?= n
FreeRTOS         ?= n
Silence          ?= y
OptimizeBuild    ?= y

#
# Driver Enable
#
EnableDMA        ?= n
EnableGPIO       ?= y
EnableGPT        ?= n
EnableI2C        ?= n
EnableRTC        ?= n
EnableSPI        ?= n
EnableBSPI       ?= n
EnableUART       ?= y
EnableUART_P     ?= y
EnableWDT        ?= n
EnableSDIO_M     ?= y
EnableSDIO_S     ?= n
EnablePKA        ?= n
EnableTRNG       ?= n
EnableSPACC      ?= n
EnableARIA       ?= n
EnableCRYPTO     ?= n


#
# User Application
#
APP_NAME         :=    sdhci

ROOT_PATH        :=    ../..

BSP_NAME         :=    ms_io_board

BSP_SRC          :=    ewbm_sdhci.c

SRC_PATH         :=    ./

APP_INC          :=    ./ \
                       ./config

APP_SRC          :=    main.c

APP_LIB          :=

APP_LD           :=    sdhci.ld

USER_DEFINES     :=

