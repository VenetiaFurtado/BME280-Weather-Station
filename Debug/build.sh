set -e
make clean
make -j8 all
arm-none-eabi-objcopy -O binary FinalProject_WeatherStation.elf FinalProject_WeatherStation.bin
st-flash write FinalProject_WeatherStation.bin  0x08000000
