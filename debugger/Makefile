# Prefixes for Colors
LIGHT_BLUE := \033[1;34m
YELLOW     := \033[1;33m
RED        := \033[1;31m
GREEN      := \033[1;32m
RESET      := \033[0m

LOG_INFO   := $(LIGHT_BLUE)[Debugger]$(RESET)
LOG_WARN   := $(YELLOW)[Debugger]$(RESET)
LOG_ERROR  := $(RED)[Debugger]$(RESET)
LOG_PASS   := $(GREEN)[Debugger]$(RESET)

LIBPS4	:=	../ps4-payload-sdk

TEXT	:=	0x926200000
DATA	:=	0x926300000

CC		:=	gcc
AS		:=	gcc
OBJCOPY	:=	objcopy
ODIR	:=	build
SDIR	:=	source
IDIRS	:=	-I$(LIBPS4)/include -I. -Iinclude
LDIRS	:=	-L$(LIBPS4) -L. -Llib
CFLAGS	:=	$(IDIRS) -O2 -std=c11 -fno-builtin -nostartfiles -nostdlib -Wall -masm=intel -march=btver2 -mtune=btver2 -m64 -mabi=sysv -mcmodel=large -DTEXT_ADDRESS=$(TEXT) -DDATA_ADDRESS=$(DATA)
SFLAGS	:=	-nostartfiles -nostdlib -march=btver2 -mtune=btver2
LFLAGS	:=	$(LDIRS) -Xlinker -T $(LIBPS4)/linker.x -Wl,--build-id=none -Ttext=$(TEXT) -Tdata=$(DATA)
CFILES	:=	$(wildcard $(SDIR)/*.c)
SFILES	:=	$(wildcard $(SDIR)/*.s)
OBJS	:=	$(patsubst $(SDIR)/%.c, $(ODIR)/%.o, $(CFILES)) $(patsubst $(SDIR)/%.s, $(ODIR)/%.o, $(SFILES))

LIBS	:= -lPS4

TARGET = debugger.bin

# Default target
$(TARGET): $(ODIR) $(OBJS)
	@echo "$(LOG_INFO) Linking $(TARGET)"
	@gcc $(LIBPS4)/crt0.s $(ODIR)/*.o -o temp.t $(CFLAGS) $(LFLAGS) $(LIBS)
	@echo "$(LOG_INFO) Copying to binary format"
	@$(OBJCOPY) -O binary temp.t $(TARGET)
	@echo "$(LOG_PASS) Binary created: $(TARGET)"
	@rm -f temp.t

# Compile C source files
$(ODIR)/%.o: $(SDIR)/%.c
	@echo "$(LOG_INFO) Compiling $<"
	@gcc -c -o $@ $< $(CFLAGS)

# Compile Assembly source files
$(ODIR)/%.o: $(SDIR)/%.s
	@echo "$(LOG_INFO) Assembling $<"
	@gcc -c -o $@ $< $(SFLAGS)

# Create build directory
$(ODIR):
	@echo "$(LOG_INFO) Creating build directory"
	@mkdir -p $@

# Clean up build artifacts
.PHONY: clean
clean:
	@echo "$(LOG_INFO) Cleaning build artifacts"
	@rm -f $(TARGET) $(ODIR)/*.o

