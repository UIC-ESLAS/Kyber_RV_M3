# Yet another Improvement of Plantard Arithmetic for Faster Kyber on Low-end 32-bit IoT Devices
This repository provides code for our faster Kyber implementation on three low-end 32-bit IoT devices: the ARM Cortex-M3, SiFive E310 board, and PQRISCV.
Authors: 
 - [Junhao Huang](https://github.com/JunhaoHuang) `<huangjunhao@uic.edu.cn>` 
 - Haosong Zhao `<zhaohaosonguic@gmail.com>`
 - [Jipeng Zhang](https://github.com/Ji-Peng) `<jp-zhang@outlook.com>`
 - Wangchen Dai `<w.dai@my.cityu.edu.hk>`
 - Lu Zhou `<lu.zhou@nuaa.edu.cn>`
 - Ray C. C. Cheung `<r.cheung@cityu.edu.hk>`
 - Çetin Kaya Koç `<cetinkoc@ucsb.edu>`
 - Donglong Chen `<donglongchen@uic.edu.cn>` (Corresponding Author)

## Downloading pqm3 and libopencm3
git clone --recursive https://github.com/UIC-ESLAS/Kyber_RV_M3.git

## ARM Cortex-M3

The setup for testing and evaluating our code on the ARM Cortex-M3 is based on the framework provided in the [pqm3](https://github.com/mupq/pqm3) project. We based our implementation mostly on this repository. However, we could not use the `benchmarks.py`, `test.py`, `testvectors.py` scripts presented in this repository in our environment. Therefore, we wrote three new scripts `new_benchmarks.py`, `new_poly_benchmarks.py`, `new_stack_benchmarks.py` for benchmarking our implementations.

### Prerequisites
   - `arm-none-eabi-gcc`: version 10.2.1
   - `Bossa`: version 1.9.1
   - `libopencm3`: commit `5617ed466444790b787b6df8d7f21d1611905fd1` from [libopencm3](https://github.com/libopencm3/libopencm3/tree/5617ed466444790b787b6df8d7f21d1611905fd1)
   - `python3` with the packages `pyserial` and `numpy` (only required for the evaluation scripts); `{pyserial-}miniterm` is used to read the output from Arduino
   - Hardware: `Arduino Due` development board with sam3x8e
### Usage

Detailed instructions on interacting with the hardware and on installing required software can be found in [pqm3](https://github.com/mupq/pqm3)'s readme.

The scripts `new_benchmarks.py`, `new_poly_benchmarks.py`, and `new_stack_benchmarks.py` cover the benchmarks in our paper.
In case separate, manual testing is required, the binaries for a scheme can be build using
```shell
make PLATFORM=sam3x8e bin/crypto_kem_{scheme}_{variant}_{firmware}.bin
```
where `scheme` can be one of the `{kyber512, kyber768, kyber1024}`, `variant` belongs to `{m3, m3fspeed, m3fstack}`, `firmware` is one of `{test, testvectors, speed, stack}`.

### Example
For building, flashing and evaluating the `testvectors` firmware for our stack-version of `kyber768`, the following command can be used.
```shell
make PLATFORM=sam3x8e ./bin/crypto_kem_kyber768_m3fstack_testvectors.bin
# (You might need to run `make clean` first, if you previously built for a different platform.)

# Flash the binary using bossac.
bossac -a --erase --write --verify --boot=1 --port=/dev/ttyACM0 ./bin/crypto_kem_kyber768_m3fstack_testvectors.bin

# Open the serial monitor.
{pyserial-}miniterm /dev/ttyACM0
```


## RISC-V: SiFive Freedom E310 board

We followed the experimental setup in [SiFive Freedom-E-SDK](https://github.com/sifive/freedom-e-sdk) and [Saber_RV32](https://github.com/Ji-Peng/Saber_RV32).

### Prerequisites
   - `RISC-V GNU toolchain`: version 10.2.0
   - `Segger J-LINK`: flashes the binary to the board
   - `python3` with the packages `pyserial` and `numpy` (only required for the evaluation scripts)
   - Hardware: `SiFive Freedom E310` development board with a 32-bit [RISC-V CPU](https://www.sifive.com/boards/hifive1-rev-b)

### Usage
The scripts `benchmarks.py` and `stack_benchmarks.py` cover the benchmarks in our paper.
In case separate, manual testing is required, the binaries for a scheme can be build using
```shell
# must use make clean first if you want to use the make run command
make clean 

# compile code in CRYPTO_PATH and firmware for CRYPTO_ITERATIONS times.
make CRYPTO_PATH=crypto_kem/{scheme}/{variant} {CRYPTO_ITERATIONS=100} {firmware}

# You can flash the binary to board in the following two ways
1. make run # must use make clean first if you want to use the make run command
2. ./jlink.sh --hex bin/{firmware}.hex --jlink JLinkExe
```
where `scheme` can be one of the `{kyber512, kyber768, kyber1024}`, `variant` belongs to `{fspeed, fstack}`, `firmware` is one of `{test, testvectors, speed, stack}`.

### Example
For building, flashing and evaluating the `testvectors` firmware for our stack-version of `kyber768` the following command can be used:
```shell
make clean && make CRYPTO_PATH=crypto_kem/kyber768/fstack CRYPTO_ITERATIONS=2 testvectors

# Flash the binary jlink.
1. make run # must use make clean first if you want to use the make run command
2. ./jlink.sh --hex bin/testvectors.hex --jlink JLinkExe

# Open the serial monitor.
python3 listen.py
```

### Code Size

```bash
riscv64-unknown-elf-nm bin/speed.elf --print-size --size-sort --radix=d | \
grep -v '\<_\|\<metal\|\<pll_configs' | \
awk '{sum+=$2 ; print $0} END{print "Total size =", sum, "bytes =", sum/1024, "kB"}'
```

## RISC-V: PQRISCV

We followed the experimental setup in [Kyber_RISC_V_Thesis](https://github.com/denigreco/Kyber_RISC_V_Thesis), [PQRISCV](https://github.com/mupq/pqriscv), and [PQRISCV-VEXRISCV](https://github.com/mupq/pqriscv-vexriscv).

### Prerequisites
   - `RISC-V GNU toolchain`: version 10.2.0
   - `jdk>1.8.0`
   - `sbt`
   - `verilator`
   - `python3` with the packages `pyserial` and `numpy` (only required for the evaluation scripts)
   - Hardware: `PQRISCV simulator` [PQRISCV-VEXRISCV](https://github.com/mupq/pqriscv-vexriscv)

### Usage
Manual testing is required to obtain the benchmarks in this paper, the binaries for a scheme can be build using
```shell
# must use make clean first if you want to use the make run command
make -f makefile_vexrv.mk clean 

# compile code in CRYPTO_PATH and firmware for CRYPTO_ITERATIONS times.
make -f makefile_vexrv.mk CRYPTO_PATH=crypto_kem/{scheme}/{variant} {CRYPTO_ITERATIONS=100} {firmware}

# Go to the pqriscv-vexriscv directory and run the following; 
# Flash the binary using sbt.
sbt "runMain mupq.PQVexRiscvSim --init ../Kyber_RV_M3/RISC-V/bin/{firmware}.bin"
```
where `scheme` can be one of the `{kyber512, kyber768, kyber1024}`, `variant` belongs to `{fspeed, fstack}`, `firmware` is one of `{test, stack, speed_vexrv, testvectors}`.

### Example
For building, flashing and evaluating the `testvectors` firmware for our stack-version of `kyber768` the following command can be used:
```shell
make -f makefile_vexrv.mk CRYPTO_PATH=crypto_kem/kyber768/fstack CRYPTO_ITERATIONS=2 testvectors
# Flash the binary using sbt.
sbt "runMain mupq.PQVexRiscvSim --init ../Kyber_RV_M3/RISC-V/bin/testvectors.bin"

# Open the serial monitor.
python3 listen.py
```

## Files structure
The following files are the main files we used in this paper
- `pqm3`: implementation on Cortex-M3
   - `common`: contains code that is shared between different schemes
   - `config.py`: saves platform configuration
   - `crypto_kem`: contains the implementations for kyber512, kyber768, kyber1024
     - `kyber512`:
       - `m3`: the original implementation with the Montgomery arithmetic presented in [pqm3](https://github.com/mupq/pqm3)
       - `m3fspeed`: the high-speed version (speed-version) implementation with the Plantard arithmetic.
       - `m3fstack`: the stack-friendly version (stack-version) implementation with the Plantard arithmetic.
     - `kyber768`:
       - `m3`: the original implementation with Montgomery arithmetic presented in [pqm3](https://github.com/mupq/pqm3)
       - `m3fspeed`:the high-speed version (speed-version) implementation with the Plantard arithmetic.
       - `m3fstack`: the stack-friendly version (stack-version) implementation with the Plantard arithmetic.
     - `kyber1024`:
       - `m3`: the original implementation with Montgomery arithmetic presented in [pqm3](https://github.com/mupq/pqm3)
       - `m3fspeed`: the high-speed version (speed-version) implementation with the Plantard arithmetic.
       - `m3fstack`: the stack-friendly version (stack-version) implementation with the Plantard arithmetic.
   - `Makefile`: Makefile to build the code in [pqm3](https://github.com/mupq/pqm3)
   - `new_benchmarks.py`: This script is used for building, flashing, and evaluating the outputs produced by `mupq/crypto_kem/speed.c`. The desired algorithms as well as the number of iterations can be set in the code. The output is stored in `new_benchmarks.txt`
   - `new_poly_benchmarks.py`: The original pqm3 does not provide code for benchmarking the polynomial arithmetic like NTT, INTT, and base multiplication. We need to modify the `mupq/crypto_kem/speed.c` so that it provides results for these operations. This script is used for building, flashing, and evaluating the outputs produced by `mupq/crypto_kem/speed.c`. The desired algorithms as well as the number of iterations can be set in the code. The output is stored in `new_poly_benchmarks.txt`
   - `new_stack_benchmarks.py`: This script is used for building, flashing, and evaluating the outputs produced by `mupq/crypto_kem/stack.c`. The desired algorithms as well as the number of iterations can be set in the code. The output is stored in `new_stack_benchmarks.txt`
- `RISC-V`: implementation on RISC-V
  - `benchmark`: contains benchmark files
  - `bsp`: contains board support package
  - `config.py`: saves platform configuration
  - `common`: contains code that is shared between different schemes
  - `crypto_kem`: contains the implementations for kyber512, kyber768, kyber1024
    - `kyber512`:
       - `fspeed`: the high-speed version (speed-version) implementation with the Plantard arithmetic.
       - `fstack`: the stack-friendly version (stack-version) implementation with the Plantard arithmetic.
     - `kyber768`:
       - `fspeed`:the high-speed version (speed-version) implementation with the Plantard arithmetic.
       - `fstack`: the stack-friendly version (stack-version) implementation with the Plantard arithmetic.
     - `kyber1024`:
       - `fspeed`: the high-speed version (speed-version) implementation with the Plantard arithmetic.
       - `fstack`: the stack-friendly version (stack-version) implementation with the Plantard arithmetic.
   - `Makefile`: Makefile to build the code for the SiFive board
   - `makefile_vexrv.mk`: Makefile to build the code for PQRISCV
   - `benchmarks.py`: This script is used for building, flashing, and evaluating the outputs produced by `benchmark/speed.c`. The desired algorithms as well as the number of iterations can be set in the code. The output is stored in `benchmarks.txt`
   - `stack_benchmarks.py`: This script is used for building, flashing, and evaluating the outputs produced by `benchmark/stack.c`. The desired algorithms as well as the number of iterations can be set in the code. The output is stored in `stack_benchmarks.txt`
   - `results_vexriscv.txt`: results for PQRISCV
   - `listen.py`: receives output from the SiFive board
   - `jlink.sh`: flashes the binary to the SiFive board