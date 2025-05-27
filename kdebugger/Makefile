KSDK := ../ps4-ksdk/

LFILE := ./kdebugger.x

CC       := gcc
AS       := gcc
OBJCOPY  := objcopy
ODIR     := build
SDIR     := source
IDIRS    := -I$(KSDK)/include -I. -Iinclude
LDIRS    := -L$(KSDK) -L. -Llib
CFLAGS   := $(IDIRS) -O3 -std=gnu11 -fno-builtin -fno-exceptions -fno-asynchronous-unwind-tables -nostartfiles -nostdlib -Wall -masm=intel -march=btver2 -mtune=btver2 -m64 -mabi=sysv -mcmodel=small -fPIE
SFLAGS   := -nostartfiles -nostdlib -masm=intel -march=btver2 -mtune=btver2 -m64 -mabi=sysv -mcmodel=small
LFLAGS   := $(LDIRS) -Xlinker -T $(LFILE) -Wl,--build-id=none -pie
CFILES   := $(wildcard $(SDIR)/*.c)
SFILES   := $(wildcard $(SDIR)/*.s)
OBJS     := $(patsubst $(SDIR)/%.c, $(ODIR)/%.o, $(CFILES)) $(patsubst $(SDIR)/%.s, $(ODIR)/%.o, $(SFILES))

LIBS := -lKSDK

TARGET := kdebugger.elf

# Default target
all: $(TARGET)

$(TARGET): $(ODIR) $(OBJS)
	@echo "[+] Linking $(TARGET)..."
	@$(CC) crt0.s $(ODIR)/*.o -o $(TARGET) $(CFLAGS) $(LFLAGS) $(LIBS)
	@echo "[+] Converting to DYN format..."
	@elfedit --output-type=DYN $(TARGET)
	@echo "[+] Stripping symbols..."
	@strip $(TARGET)
	@echo "[✓] Build complete: $(TARGET)"

$(ODIR)/%.o: $(SDIR)/%.c
	@echo "[+] Compiling C: $<"
	@$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/%.s
	@echo "[+] Assembling: $<"
	@$(AS) -c -o $@ $< $(SFLAGS)

$(ODIR):
	@echo "[+] Creating output directory: $@"
	@mkdir -p $@

.PHONY: clean

clean:
	@echo "[+] Cleaning up..."
	@rm -f $(TARGET) $(ODIR)/*.o
	@echo "[✓] Clean complete."
