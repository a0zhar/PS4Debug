ODIR    := build
SDIR    := source
IDIRS   := -I. -Iinclude
LDIRS   := -L. -Llib
CFLAGS  := $(IDIRS) -Os -std=gnu11 -ffunction-sections -fdata-sections -fno-builtin -nostartfiles -nostdlib -Wall -masm=intel -march=btver2 -mtune=btver2 -m64 -mabi=sysv -mcmodel=small -fpie
LFLAGS  := $(LDIRS)
CFILES  := $(wildcard $(SDIR)/*.c)
SFILES  := $(wildcard $(SDIR)/*.s)
OBJS    := $(patsubst $(SDIR)/%.c, build/%.o, $(CFILES)) $(patsubst $(SDIR)/%.s, build/%.o, $(SFILES))

TARGET  = libKSDK.a
# Colors
YELLOW := \033[1;33m
GREEN  := \033[1;32m
RESET  := \033[0m

LOG_INFO := $(YELLOW)[KSDK]$(RESET)
LOG_PASS := $(GREEN)[KSDK]$(RESET)
# Build target
$(TARGET): $(ODIR) $(OBJS)
	@echo "$(LOG_INFO) Archiving objects into static library $@"
	@ar rcs $@ $(OBJS)
	@echo "$(LOG_PASS) Build successful: $(TARGET)"

# Compile C files
$(ODIR)/%.o: $(SDIR)/%.c
	@echo "$(LOG_INFO) Compiling C source: $<"
	@gcc -c -o $@ $< $(CFLAGS) $(LFLAGS)
	@echo "$(LOG_PASS) Compilation completed for: $@"

# Assemble S files
$(ODIR)/%.o: $(SDIR)/%.s
	@echo "$(LOG_INFO) Assembling assembly source: $<"
	@gcc -c -o $@ $< $(CFLAGS) $(LFLAGS)
	@echo "$(LOG_PASS) Assembly completed for: $@"

# Create build directory if it doesn't exist
$(ODIR):
	@echo "$(LOG_INFO) Creating build directory: $(ODIR)"
	@mkdir -p $(ODIR)
	@echo "$(LOG_PASS) Build directory created."

# Clean up
.PHONY: clean
clean:
	@echo "[INFO] Cleaning up build files..."
	@rm -rf $(TARGET) $(ODIR)
	@echo "[INFO] Clean completed."

