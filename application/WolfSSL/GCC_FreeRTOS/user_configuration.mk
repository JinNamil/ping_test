# User Configuration

#
# Options
#
CHIP_NAME        ?= MS500
DebugBuild       ?= y
JtagBuild        ?= n
# If you change QSPIBoot, you have to check linker script(.ld) file. 
QSPIBoot         ?= n
FreeRTOS         ?= y
Silence          ?= y
OptimizeBuild    ?= y

#
# Driver Enable
#
EnableDMA        ?= n
EnableGPIO       ?= y
EnableGPT        ?= n
EnableI2C        ?= n
EnableRTC        ?= y
EnableSPI        ?= y
EnableBSPI       ?= y
EnableUART       ?= y
EnableUART_P     ?= y
EnableWDT        ?= n
EnableSDIO_M     ?= n
EnableSDIO_S     ?= n
EnablePKA        ?= y
EnableTRNG       ?= y
EnableSPACC      ?= y
EnableARIA       ?= n
EnableCRYPTO     ?= y
use_ssl          ?= y
use_w5500        ?= y

#
# User Application
#
APP_NAME         :=    wiznet_arduino

ROOT_PATH        :=    ../../..

BSP_NAME         :=

BSP_SRC          :=

SRC_PATH         :=    ../

APP_INC          :=    ./ \
                       ./config

APP_SRC          :=    arduino_test.c \
			 		   ArduinoSlave.c


APP_LIB          :=

APP_LD           :=    wiznet_arduino.ld

USER_DEFINES     :=

