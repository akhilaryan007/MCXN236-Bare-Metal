CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

TARGET  = firmware
BUILD   = build

CPU     = -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16

SRCS     = src/main.c startup/startup_MCXN236.c
INCLUDES = -Iinclude
LDSCRIPT = linker/MCXN236.ld

CFLAGS   = $(CPU) $(INCLUDES) -Wall -g -O0 -ffunction-sections -fdata-sections
LDFLAGS  = $(CPU) -T$(LDSCRIPT) -Wl,-Map=$(BUILD)/$(TARGET).map
LDFLAGS += -Wl,--gc-sections -nostartfiles --specs=nano.specs --specs=nosys.specs

all: $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).bin

$(BUILD)/$(TARGET).elf: $(SRCS) | $(BUILD)
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $@
	$(SIZE) $@

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD):
	-mkdir $(BUILD)

clean:
	-rmdir /s /q $(BUILD)

.PHONY: all clean
