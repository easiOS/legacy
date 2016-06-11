PATH := $(DEVKITARM)/bin:$(PATH)

# --- Project details -------------------------------------------------
 
PROJ    := easiwin
TARGET  := $(PROJ)
 
OBJS    := main.o eelphant.o video.o
 
# --- Build defines ---------------------------------------------------
 
PREFIX  := /home/easimer/devkitPro/devkitARM/bin/arm-none-eabi-
CC      := $(PREFIX)gcc
LD      := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy
 
ARCH    := -mthumb-interwork -mthumb
SPECS   := -specs=gba.specs
 
CFLAGS  := $(ARCH) -O2 -Wall -fno-strict-aliasing
LDFLAGS := $(ARCH) $(SPECS)
 
 
.PHONY : build clean
 
# --- Build -----------------------------------------------------------
# Build process starts here!
build: $(TARGET).gba
 
# Strip and fix header (step 3,4)
$(TARGET).gba : $(TARGET).elf
	$(OBJCOPY) -v -O binary $< $@
	-@gbafix $@
 
# Link (step 2)
$(TARGET).elf : $(OBJS)
	$(LD) $^ $(LDFLAGS) -o $@
 
# Compile (step 1)
$(OBJS) : %.o : %.c
	$(CC) -c $< $(CFLAGS) -o $@
 
# --- Clean -----------------------------------------------------------
 
clean : 
	@rm -fv *.gba
	@rm -fv *.elf
	@rm -fv *.o
 
#EOF
