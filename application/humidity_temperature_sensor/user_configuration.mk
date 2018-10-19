# User Configuration

#
# Options
#
CHIP_NAME        ?= MS500
DebugBuild       ?= y
JtagBuild        ?= y
# If you change QSPIBoot, you have to check linker script(.ld) file. 
QSPIBoot         ?= n
FreeRTOS         ?= n
Silence          ?= n
OptimizeBuild    ?= n

#
# Driver Enable
#
EnableDMA        ?= n
EnableGPIO       ?= n
EnableGPT        ?= n
EnableI2C        ?= y
EnableRTC        ?= n
EnableSPI        ?= n
EnableBSPI       ?= n
EnableUART       ?= y
EnableUART_P     ?= y
EnableWDT        ?= n
EnableSDIO_M     ?= n
EnableSDIO_S     ?= n
EnablePKA        ?= n
EnableTRNG       ?= n
EnableSPACC      ?= n
EnableARIA       ?= n
EnableCRYPTO     ?= n


#
# User Application
#
APP_NAME         :=    humidity_temperature_sensor

ROOT_PATH        :=    ../..

BSP_NAME         :=    ms_io_board

BSP_SRC          :=    ewbm_htsensor.c

SRC_PATH         :=    ./

APP_INC          :=    ./ \
                       ./config

APP_SRC          :=    main.c

APP_LIB          :=

APP_LD           :=    humidity_temperature_sensor.ld

USER_DEFINES     :=

