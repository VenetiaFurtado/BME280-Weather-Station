set -e
make clean
make -j8 all
arm-none-eabi-objcopy -O binary Assignment7_WaveForms.elf Assignment7_WaveForms.bin
st-flash write Assignment7_WaveForms.bin 0x08000000
