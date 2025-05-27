ODIR	:= build
SDIR	:= source
IDIR	:= include
CFLAGS	:= -I$(IDIR) -Os -std=c11 -ffunction-sections -fdata-sections -fno-builtin -nostartfiles -nostdlib -Wall -Wextra -masm=intel -march=btver2 -mtune=btver2 -m64 -mabi=sysv -mcmodel=small -fpie -fPIC
CFILES	:= $(wildcard $(SDIR)/*.c)
SFILES	:= $(wildcard $(SDIR)/*.s)
OBJS	:= $(patsubst $(SDIR)/%.c, build/%.o, $(CFILES)) $(patsubst $(SDIR)/%.s, build/%.o, $(SFILES))

TARGET = libPS4.a

$(TARGET): $(ODIR) $(OBJS)
	@echo "[libPS4] Archiving objects into static lib ($@)"
	@ar rcs $@ $(OBJS)
	@echo "[libPS4] Done."

$(ODIR)/%.o: $(SDIR)/%.c
	@echo "[libPS4] Compiling: $<"
	@gcc -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/%.s
	@echo "[libPS4] Assembling: $<"
	@gcc -c -o $@ $< $(CFLAGS)

$(ODIR):
	@echo "[libPS4] Creating Output Folder"
	@mkdir $@

.PHONY: clean

clean:
	@echo "[libPS4] Performing Cleanup"
	@rm -rf $(TARGET) $(ODIR)