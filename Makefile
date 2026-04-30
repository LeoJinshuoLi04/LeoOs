# Compiler / Linker Definitions
AS  = i686-elf-as
CPP = i686-elf-g++
LD  = i686-elf-gcc

# Directories
SRC_DIR  := src
STR_DIR  := structures
BUILD_DIR := build

# 1. FIND ALL SOURCE FILES
# This looks recursively for every .cpp and .s file in your folders
CPP_SOURCES := $(shell find $(SRC_DIR) $(STR_DIR) -name '*.cpp')
ASM_SOURCES := $(shell find $(SRC_DIR) -name '*.s')

# 2. GENERATE OBJECT FILE LIST
# This maps 'structures/string/string.cpp' to 'build/structures/string/string.o'
OBJ := $(CPP_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
OBJ += $(ASM_SOURCES:%.s=$(BUILD_DIR)/%.asm.o)

# 3. INCLUDE PATHS
# Finds all directories in src and structures to allow shorthand includes
INC_DIRS  := $(shell find $(SRC_DIR) $(STR_DIR) -type d)
INC_FLAGS := $(foreach dir, $(INC_DIRS), -I$(dir))

# Flags
# Added -I. to allow includes relative to the root if needed
CPPFLAGS = -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti $(INC_FLAGS) -I.
LDFLAGS  = -T src/arch/i386/linker.ld -ffreestanding -O2 -nostdlib -lgcc

# --- BUILD RULES ---

all: $(BUILD_DIR)/myos.bin

# Link the kernel binary
$(BUILD_DIR)/myos.bin: $(OBJ)
	@echo "Linking $@"
	@$(LD) $(LDFLAGS) -o $@ $(OBJ)

# Pattern rule for C++ files
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@echo "Compiling $<"
	@$(CPP) $(CPPFLAGS) -c $< -o $@

# Pattern rule for Assembly files
$(BUILD_DIR)/%.asm.o: %.s
	@mkdir -p $(@D)
	@echo "Assembling $<"
	@$(AS) $< -o $@

run: all
	qemu-system-i386 -kernel build/myos.bin

clean:
	rm -rf $(BUILD_DIR)

debug:
	@echo "CPP Files: $(CPP_SOURCES)"
	@echo "ASM Files: $(ASM_SOURCES)"
	@echo "Object Files: $(OBJ)"

.PHONY: all clean run