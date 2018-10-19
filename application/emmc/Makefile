include ./user_configuration.mk

OBJDIR	:= obj
DEPDIR	:= dep
ifeq ($(DebugBuild), y)
BUILD	:= build_debug
else
BUILD	:= build_release
endif

CPU		:= cortex-m0
CORE	:= armv6-m
ENDIAN	:= little-endian

PREFIX=arm-none-eabi-
CC=$(PREFIX)gcc
LD=$(PREFIX)gcc
AR=$(PREFIX)ar
AS=$(PREFIX)as
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump


ifeq ($(Silence),y)
V:=@
else
V:=
endif



#
# Header File Define
#

SYSTEM_INC :=	-I$(ROOT_PATH)/cpu/inc
SYSTEM_INC +=	-I$(ROOT_PATH)/component/config/inc

DRIVER_INC :=	-I$(ROOT_PATH)/component/drivers/ip/inc
ifeq ($(EnablePKA), y)
DRIVER_INC +=	-I$(ROOT_PATH)/component/drivers/clp300/inc
endif

ifeq ($(EnableTRNG), y)
DRIVER_INC +=	-I$(ROOT_PATH)/component/drivers/trng/inc
endif

ifeq ($(EnableSPACC), y)
DRIVER_INC +=	-I$(ROOT_PATH)/component/drivers/spacc/inc
endif

ifeq ($(EnableARIA), y)
DRIVER_INC +=	-I$(ROOT_PATH)/component/drivers/aria/inc
endif


SERVICE_INC :=
ifeq ($(EnableCRYPTO), y)
SERVICE_INC += -I$(ROOT_PATH)/service/crypto/inc
endif
SERVICE_INC := $(strip $(SERVICE_INC))

ifeq ($(FreeRTOS), y)
FREERTOS_INC :=	-I$(ROOT_PATH)/rtos/FreeRTOS/Source/include \
				-I$(ROOT_PATH)/rtos/FreeRTOS/Source/portable/GCC/ARM_CM0
endif

BSP_INC		:= -I$(ROOT_PATH)/component/bsp/$(BSP_NAME)/inc

APP_INC := $(strip $(APP_INC))
APP_INC := $(patsubst %,-I%,$(APP_INC))

INCLUDES :=	$(SYSTEM_INC)
INCLUDES +=	$(DRIVER_INC)
INCLUDES +=	$(SERVICE_INC)
ifeq ($(FreeRTOS), y)
INCLUDES +=	$(FREERTOS_INC)
endif
INCLUDES +=	$(BSP_INC)
INCLUDES +=	$(APP_INC)
INCLUDES :=	$(strip $(INCLUDES))


#
# Source Code
#
vpath %.c	$(ROOT_PATH)/cpu/src
vpath %.c	$(ROOT_PATH)/component/drivers/ip/src
vpath %.c	$(ROOT_PATH)/component/bsp/$(BSP_NAME)/src
vpath %.c	$(SRC_PATH)

ifeq ($(FreeRTOS), y)
vpath %.c	$(ROOT_PATH)/rtos/FreeRTOS/Source
vpath %.c	$(ROOT_PATH)/rtos/FreeRTOS/Source/portable/GCC/ARM_CM0
vpath %.c	$(ROOT_PATH)/rtos/FreeRTOS/Source/portable/MemMang
endif


# Added by HWCHO, For PKA
ifeq ($(EnablePKA), y)
vpath %.c	$(ROOT_PATH)/component/drivers/clp300/src
endif

ifeq ($(EnableTRNG), y)
vpath %.c	$(ROOT_PATH)/component/drivers/trng/src
endif

ifeq ($(EnableSPACC), y)
vpath %.c	$(ROOT_PATH)/component/drivers/spacc/src
endif

ifeq ($(EnableARIA), y)
vpath %.c	$(ROOT_PATH)/component/drivers/aria/src
endif

ifeq ($(EnableCRYPTO), y)
vpath %.c	$(ROOT_PATH)/service/crypto/src
endif

SYSTEM_SRC :=	ewbm_clock.c \
				ewbm_isr.c \
				ewbm_padmux.c \
				startup_ewbm.c \
				system_ewbm.c

DRIVER_SRC := ewbm_pmu.c \
			  ewbm_delay.c

ifeq ($(EnableDMA), y)
DRIVER_SRC += ewbm_dma.c
endif

ifeq ($(EnableGPIO), y)
DRIVER_SRC += ewbm_gpio.c
endif

ifeq ($(EnableGPT), y)
DRIVER_SRC += ewbm_gpt.c
endif

ifeq ($(EnableI2C), y)
DRIVER_SRC += ewbm_i2c.c
endif

ifeq ($(EnableRTC), y)
DRIVER_SRC += ewbm_rtc.c
endif

ifeq ($(EnableSPI), y)
DRIVER_SRC += ewbm_spi.c
DRIVER_SRC += ewbm_spi_arm.c
DRIVER_SRC += ewbm_spi_ewbm.c
endif

ifeq ($(EnableBSPI), y)
DRIVER_SRC += ewbm_bspi.c
endif

ifeq ($(EnableUART), y)
DRIVER_SRC += ewbm_uart.c
endif

ifeq ($(EnableWDT), y)
DRIVER_SRC += ewbm_wdt.c
endif

ifeq ($(EnableSDIO_M), y)
DRIVER_SRC += ewbm_sdio_master.c
endif

ifeq ($(EnableSDIO_S), y)
DRIVER_SRC += ewbm_sdio_slave.c
endif

# Added by HWCHO, For PKA
ifeq ($(EnablePKA), y)
DRIVER_SRC +=	clp300_fw.c	\
				clp300_ecc.c	\
				clp300_modular.c
endif

ifeq ($(EnableTRNG), y)
DRIVER_SRC +=	trng.c
endif

ifeq ($(EnableSPACC), y)
DRIVER_SRC +=	spacc.c
endif

ifeq ($(EnableARIA), y)
DRIVER_SRC +=	aria.c
endif

SERVICE_SRC :=

ifeq ($(FreeRTOS), y)
FREERTOS_SRC :=	croutine.c \
				event_groups.c \
				list.c \
				queue.c \
				tasks.c \
				timers.c \
				port.c \
				heap_3.c
endif

ifeq ($(EnableCRYPTO), y)
CRYPTO_SRC :=	ewbm_crypto_aes.c \
				ewbm_crypto_rsa.c \
				ewbm_crypto_ecc.c \
				ewbm_crypto_hash.c \
				ewbm_crypto_hmac.c \
				ewbm_crypto_rand.c \
				ewbm_crypto.c
endif


SOURCES :=	$(SYSTEM_SRC)
SOURCES +=	$(DRIVER_SRC)
SOURCES +=	$(SERVICE_SRC)
ifeq ($(FreeRTOS), y)
SOURCES +=	$(FREERTOS_SRC)
endif
SOURCES +=	$(BSP_SRC)
SOURCES +=	$(APP_SRC)
SOURCES +=  $(CRYPTO_SRC)
SOURCES :=	$(strip $(SOURCES))

#
# Librarys
#
LIBS := $(LIBC) \
		$(LIBGCC) \
		$(LIBM) \
		$(LIBNOSYS) \
		$(CRT0) \
		$(CRTI)
LIBS += $(APP_LIB)
ifeq ($(EnableCRYPTO), y)
ifeq ($(FreeRTOS), y)
#LIBS += $(ROOT_PATH)/service/crypto/lib/libcrypto_freertos.a
else
#LIBS += $(ROOT_PATH)/service/crypto/lib/libcrypto.a
endif
endif
LIBS :=	$(strip $(LIBS))

#
# Deinfes
#
ifeq ($(CHIP_NAME), MS500)
DEFINES :=	-DEWBMMS500
else ifeq ($(CHIP_NAME), MS300)
DEFINES :=	-DEWBMMS300
endif

ifeq ($(DebugBuild), y)
DEFINES +=	-D__DEBUG__
endif

ifeq ($(QSPIBoot), y)
DEFINES +=	-D__QSPIBOOT__
else ifeq ($(JtagBuild), y)
DEFINES +=	-D__JTAG__
endif

ifeq ($(FreeRTOS), y)
DEFINES +=	-D__FREE_RTOS__
endif

ifeq ($(EnableDMA), y)
DEFINES +=	-D__ENABLE_DMA__
endif

ifeq ($(EnableSDIO_M), y)
DEFINES +=    -D__ENABLE_SDIO_M__
endif

ifeq ($(EnableSDIO_S), y)
DEFINES +=    -D__ENABLE_SDIO_S__
endif

ifeq ($(EnableUART), y)
ifeq ($(EnableUART_P), y)
DEFINES +=	-D__ENABLE_UART_P__
endif
endif

DEFINES += $(USER_DEFINES)
DEFINES := $(strip $(DEFINES))





# MCU flag
MCUFLAG :=	-mcpu=$(CPU) -march=$(CORE) -m$(ENDIAN) -mthumb -msoft-float


#
# Compile Flags
#

LD_FLAGS :=	-T$(APP_LD)
ifeq ($(DebugBuild), y)
LD_FLAGS += -g -O0 \
			$(MCUFLAG) \
			-Wl,--gc-sections,-Map=$(BUILD)/$(APP_NAME).map
else
LD_FLAGS += -O2 \
			$(MCUFLAG) \
			-Wl,--gc-sections,-Map=$(BUILD)/$(APP_NAME).map
endif
LD_FLAGS := $(strip $(LD_FLAGS))

ifeq ($(DebugBuild), y)
AS_FLAGS := -g \
			$(MCUFLAG) $(INCLUDES)
else
AS_FLAGS := $(MCUFLAG) $(INCLUDES)
endif

AS_FLAGS := $(strip $(AS_FLAGS))

ifeq ($(DebugBuild), y)
ifeq ($(OptimizeBuild), y)
C_FLAGS :=	-g -ggdb -Os -Wall -Wfatal-errors -ffunction-sections -fdata-sections \
			$(MCUFLAG) $(INCLUDES)
else
C_FLAGS :=	-g -ggdb -O0 -Wall -Wfatal-errors \
			$(MCUFLAG) $(INCLUDES)
endif
else
C_FLAGS :=	-O2 \
			$(MCUFLAG) $(INCLUDES)
endif

C_FLAGS := $(strip $(C_FLAGS))


OBJ_SRC := $(strip $(patsubst %.c,$(OBJDIR)/%.o,$(SOURCES)))
OBJS := $(strip $(OBJ_SRC))

DEPS_SRC += $(strip $(patsubst %.c,$(DEPDIR)/%.dep,$(SOURCES)))
DEPS := $(strip $(DEPS_SRC))





.SUFFIXES: .o .s
$(OBJDIR)/%.o : %.s
	@if [ ! -d $(OBJDIR) ]; then mkdir -p $(OBJDIR) ; fi
	@echo "Assembly Compile : $<"
	$(V)$(AS) $(AS_FLAGS) -c $< -o $@

.SUFFIXES: .o .c
$(OBJDIR)/%.o : %.c
	@if [ ! -d $(OBJDIR) ]; then mkdir -p $(OBJDIR) ; fi
	@echo "C Compile : $<"
	$(V)$(CC) $(C_FLAGS) $(DEFINES) -c $< -o $@



.SUFFIXES: %.dep .c
$(DEPDIR)/%.dep : %.c
	@echo "Dependency Check : $<"
	@if [ ! -d $(DEPDIR) ]; then mkdir -p $(DEPDIR) ; fi
	$(V)$(CC) $(C_FLAGS) -M $< > $@


.SUFFIXES: %.dep .s
$(DEPDIR)/%.dep : %.s
	@echo "Dependency Check : $<"
	@if [ ! -d $(DEPDIR) ]; then mkdir -p $(DEPDIR) ; fi
	$(V)$(CC) $(C_FLAGS) -M $< > $@

.PHONY: all clean bin asm
all:$(APP_NAME) bin asm
$(APP_NAME):$(OBJS)
	@if [ ! -d $(BUILD) ]; then mkdir -p $(BUILD) ; fi
	@echo "Linking $(BUILD)/$(APP_NAME).elf"
	$(LD) $(LD_FLAGS) -o $(BUILD)/$(APP_NAME).elf $^ $(LIBS)

dep: $(DEPS)

bin:
	$(OBJCOPY) --gap-fill=0xff -O binary  $(BUILD)/$(APP_NAME).elf $(BUILD)/$(APP_NAME).bin

asm:
	$(OBJDUMP) --disassemble-all $(BUILD)/$(APP_NAME).elf > $(BUILD)/$(APP_NAME).S

clean:
	-rm -rf $(OBJDIR)
	-rm -rf $(DEPDIR)
	-rm -rf $(BUILD)


test:
	@echo "DEFINES : $(DEFINES)"
	@echo "LIBS: $(LIBS)"
	@echo "OBJS: $(OBJS)"
	@echo "DEPS: $(DEPS)"
	@echo "CFLAGS: $(C_FLAGS)"
