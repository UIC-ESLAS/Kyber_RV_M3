CROSS_PREFIX ?= riscv64-unknown-elf
RISCV_GCC    = $(CROSS_PREFIX)-gcc
LD            = $(CROSS_PREFIX)-gcc
OBJCOPY       = $(CROSS_PREFIX)-objcopy
OBJDUMP 	  = $(CROSS_PREFIX)-objdump
PLATFORM     ?= vexriscv

COMMON_DIR = common

ifndef CRYPTO_PATH
CRYPTO_PATH = crypto_kem/kyber512/fstack
endif
ifndef CRYPTO_ITERATIONS
CRYPTO_ITERATIONS = 100
endif

RISCV_ARCH ?= rv32im
RISCV_ABI ?= ilp32
RISCV_CMODEL ?= medany
RISCV_ARCHFLAGS += -march=$(RISCV_ARCH)
RISCV_ARCHFLAGS += -mabi=$(RISCV_ABI)
RISCV_ARCHFLAGS += -mcmodel=$(RISCV_CMODEL)

# C Flags that must be used for the Murax SoC
VEXRISCV_CFLAGS += $(RISCV_ARCHFLAGS)
VEXRISCV_CFLAGS += -fstrict-volatile-bitfields

VEXRISCV_PLATFORM ?= pqvexriscvsim

PLATFORM_BSP_DIR = bsp/vexrv

VEXRISCV_LINKERSCRIPT = $(PLATFORM_BSP_DIR)/$(VEXRISCV_PLATFORM).ld
# LD Flags that must be used to link executables for the Murax SoC
VEXRISCV_LDFLAGS += $(RISCV_ARCHFLAGS)
VEXRISCV_LDFLAGS += --specs=nosys.specs
VEXRISCV_LDFLAGS += -Wl,-T$(VEXRISCV_LINKERSCRIPT)
VEXRISCV_LDFLAGS += -nostartfiles -ffreestanding -Wl,--gc-sections
VEXRISCV_LDFLAGS += -L$(PLATFORM_BSP_DIR)
VEXRISCV_LDFLAGS += -Wl,--start-group -l$(VEXRISCV_PLATFORM)bsp -lc -Wl,--end-group

PLATFORM_CFLAGS = $(VEXRISCV_CFLAGS) -DVEXRISCV_PLATFORM=$(VEXRISCV_PLATFORM) -DCRYPTO_ITERATIONS=$(CRYPTO_ITERATIONS)
PLATFORM_LDFLAGS = $(VEXRISCV_LDFLAGS)
PLATFORM_LINKDEP = $(PLATFORM_BSP_DIR)/lib$(VEXRISCV_PLATFORM)bsp.a $(VEXRISCV_LINKERSCRIPT)

$(PLATFORM_LINKDEP):
	make -C $(PLATFORM_BSP_DIR) PLATFORM=$(VEXRISCV_PLATFORM)

CFLAGS       += -O3
CFLAGS       += -fno-common -MD $(DEFINES) \
							-DPQRISCV_PLATFORM=$(PLATFORM) \
              $(PLATFORM_CFLAGS)
CFLAGS		+= -I$(PLATFORM_BSP_DIR) -I$(COMMON_DIR) -I$(CRYPTO_PATH)		  			  
LDFLAGS      += \
                $(PLATFORM_LDFLAGS)


PROGRAM_SRCS = $(wildcard $(CRYPTO_PATH)/*.c) $(wildcard $(CRYPTO_PATH)/*.S)
COMMON_SRCS = $(COMMON_DIR)/aes_encrypt.S $(COMMON_DIR)/aes_keyschedule.S  $(COMMON_DIR)/aes.c $(COMMON_DIR)/sha2.c $(COMMON_DIR)/fips202.c $(COMMON_DIR)/keccakf1600.c $(COMMON_DIR)/keccakf1600_asm.S $(COMMON_DIR)/randombytes.c $(COMMON_DIR)/hal-vexriscv.c


.PHONY: all
# all: bin/ntt_test.elf bin/hello.elf
all: test stack speed_vexrv testvectors

test: bin/test.elf
stack: bin/stack.elf
speed_vexrv: bin/speed_vexrv.elf
testvectors: bin/testvectors.elf

# benchmark/*.c

bin/%.elf: \
		benchmark/%.c \
		$(COMMON_SRCS) $(PROGRAM_SRCS) \
		$(PLATFORM_LINKDEP)
	mkdir -p $(dir $@)
	$(RISCV_GCC) -o $@ $(CFLAGS) \
		$(filter %.c,$^) $(filter %.S,$^) \
		$(LDFLAGS)
	$(OBJCOPY) -O ihex $@ $(basename $@).hex
	$(OBJCOPY) -Obinary $@ $(basename $@).bin
	$(OBJDUMP) -D $@ > $(basename $@).s

.PHONY: clean-software
clean-software:
	rm -rf bin

.PHONY: clean
clean: clean-software



