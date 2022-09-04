PROJECT = totp-stm32

BUILD_DIR = .

SRCS = $(shell find * -type f -name '*.s' -o -name '*.c')
OBJS = $(addprefix $(BUILD_DIR)/, $(addsuffix .o,  $(basename $(SRCS))))
SUS  = $(addprefix $(BUILD_DIR)/, $(addsuffix .su, $(basename $(SRCS))))
TARGET = $(BUILD_DIR)/$(PROJECT)

CROSS_COMPILE = arm-none-eabi-
DEBUGGER = openocd
OOCDCNF = openocd/bluepill_stm32f103.cfg

CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE = $(CROSS_COMPILE)size
GDB = $(CROSS_COMPILE)gdb

OPTFLAGS += -O2

CFLAGS += -mcpu=cortex-m3 -mthumb -mabi=aapcs
CFLAGS += -Wall -Wextra -Wl,--gc-sections
CFLAGS += -fno-common -ffunction-sections -fdata-sections -static
CFLAGS += -DSTM32F103xB -DHSE_VALUE=8000000 -DLSE_VALUE=32768 -DCOMPILE_DATE=`date +%s`UL
CFLAGS += -Icmsis/inc -Isrc/ -Isrc/peripherals -Isrc/devices

LDSCR = STM32F103CBTX_FLASH.ld

LDFLAGS += -march=armv7-m -mthumb -mabi=aapcs
LDFLAGS += -nostartfiles -nostdlib -lm -lgcc
LDFLAGS += -Lld -T$(LDSCR)

.PHONY: all build output info size flash clean

all: build size

build: CFLAGS += $(OPTFLAGS)
build: $(TARGET).elf

debug_build: CFLAGS += -fstack-usage -ggdb3
debug_build: LDFLAGS += -Xlinker -Map=$(TARGET).map
debug_build: $(TARGET).elf
	$(OBJDUMP) -x -S $(TARGET).elf > $(TARGET).lst
	$(OBJDUMP) -D $(TARGET).elf > $(TARGET).dis
	$(SIZE) $(TARGET).elf > $(TARGET).size

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $+ -o $@

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(@D)
	$(AS) $+ -o $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $+ -o $@ $(LDFLAGS)

size: $(TARGET).elf
	$(SIZE) $(TARGET).elf

flash: build
	$(DEBUGGER) -f $(OOCDCNF) -c "program $(TARGET).elf verify reset exit"

debug: debug_build
	$(DEBUGGER) -f $(OOCDCNF) &
	$(GDB) $(TARGET).elf -ex "target remote localhost:3333" -ex "monitor reset halt"

clean:
	rm -f $(TARGET).elf
	rm -f $(TARGET).bin
	rm -f $(TARGET).hex
	rm -f $(TARGET).size
	rm -f $(TARGET).lst
	rm -f $(TARGET).dis
	rm -f $(TARGET).map
	rm -f $(OBJS)
	rm -f $(SUS)
