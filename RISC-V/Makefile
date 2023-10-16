# BSP_DIR sets the path to the target-specific board support package.
BSP_DIR = bsp/sifive
# CRYPTO_PATH sets the path to the program source directory
# CRYPTO_PATH = crypto_kem/kyber768/fstack
COMMON_DIR = common

# Default CRYPTO_PATH; usage: make CRYPTO_PATH=***;
ifndef CRYPTO_PATH
CRYPTO_PATH = crypto_kem/kyber768/fspeed
endif
ifndef CRYPTO_ITERATIONS
CRYPTO_ITERATIONS = 3
endif


# Allow users to select a different cross compiler.
CROSS_COMPILE ?= riscv64-unknown-elf
RISCV_GCC     := $(CROSS_COMPILE)-gcc
RISCV_OBJDUMP := $(CROSS_COMPILE)-objdump
RISCV_OBJCOPY := $(CROSS_COMPILE)-objcopy
RISCV_GDB     := $(CROSS_COMPILE)-gdb
RISCV_AR      := $(CROSS_COMPILE)-ar
RISCV_SIZE    := $(CROSS_COMPILE)-size

ARCH_FLAGS = -march=rv32imac -mabi=ilp32 -mcmodel=medlow
SPEC=nano
MTIME_RATE_HZ_DEF=32768
PROGRAM_SRCS = $(wildcard $(CRYPTO_PATH)/*.c) $(wildcard $(CRYPTO_PATH)/*.S)
COMMON_SRCS = $(COMMON_DIR)/aes.c $(COMMON_DIR)/sha2.c $(COMMON_DIR)/fips202.c $(COMMON_DIR)/keccakf1600.c $(COMMON_DIR)/randombytes.c $(COMMON_DIR)/getcycles.S

RISCV_CFLAGS	+=	$(ARCH_FLAGS) \
					-ffunction-sections -fdata-sections \
					-I$(BSP_DIR)/install/include -I$(COMMON_DIR) -I$(CRYPTO_PATH) \
					--specs=$(SPEC).specs \
					-DMTIME_RATE_HZ_DEF=$(MTIME_RATE_HZ_DEF) \
					-Os -DCRYPTO_ITERATIONS=$(CRYPTO_ITERATIONS)
# for speed version: L=2: stack_size=0x1b00, L=3: stack_size=0x2300, L=4: stack_size=0x2900
# for stack version: L=2: stack_size=0x1b00, L=3: stack_size=0x2900, L=4: stack_size=0x2900
# for stack: L=2: stack_size=0x1a00, L=3: stack_size=0x1a00, L=4: stack_size=0x1a00
# 512, 768, 1024; change __stack_size automatically using the SCHEME_TYPE and BUILD_TYPE (if it is build by stack)
SCHEME_TYPE := $(shell echo $(CRYPTO_PATH) | sed 's/[^0-9]//g')
STACK_SIZE:=0x1b00
ifeq (stack, $(filter stack, $(MAKECMDGOALS)))
	STACK_SIZE:=0x1a00
else
	ifeq ($(shell expr $(SCHEME_TYPE) == 1024), 1)
		STACK_SIZE:=0x2800
	else ifeq ($(shell expr $(SCHEME_TYPE) == 768), 1)
		STACK_SIZE:=0x2600
	else
		STACK_SIZE:=0x1F00
	endif
endif

RISCV_LDFLAGS	+=	-Wl,--gc-sections -Wl,-Map,$(basename $@).map \
					-nostartfiles -nostdlib \
					-L$(sort $(dir $(abspath $(filter %.a,$^)))) \
					-T$(abspath $(filter %.lds,$^)) \
					-Xlinker --defsym=__stack_size=$(STACK_SIZE) \
					-Xlinker --defsym=__heap_max=1

RISCV_LDLIBS	+=	-Wl,--start-group -lc -lgcc -lm -lmetal -lmetal-gloss -Wl,--end-group

.PHONY: all
all: test speed stack testvectors

test: bin/test.elf
stack: bin/stack.elf
speed: bin/speed.elf
testvectors: bin/testvectors.elf

# benchmark/*.c

bin/%.elf: \
		benchmark/%.c \
		$(COMMON_SRCS) $(PROGRAM_SRCS) \
		common/libmetal.a \
		common/libmetal-gloss.a \
		common/metal.default.lds
	mkdir -p $(dir $@)
	$(RISCV_GCC) $(RISCV_CFLAGS) $(RISCV_LDFLAGS) \
		$(filter %.c,$^) $(filter %.S,$^) \
		$(RISCV_LDLIBS) -o $@
	$(RISCV_OBJCOPY) -O ihex $@ $(basename $@).hex
	$(RISCV_OBJDUMP) -D $@ > $(basename $@).s

.PHONY: clean-software
clean-software:
	rm -rf bin

.PHONY: clean
clean: clean-software

.PHONY: run
run: 
	./jlink.sh --hex bin/*.hex --jlink JLinkExe


