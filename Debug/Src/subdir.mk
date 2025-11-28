################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/bme280.c \
../Src/buffer.c \
../Src/data_acquisition.c \
../Src/fsm.c \
../Src/i2c.c \
../Src/i2cbb.c \
../Src/main.c \
../Src/pwm.c \
../Src/spi.c \
../Src/switch.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/bme280.o \
./Src/buffer.o \
./Src/data_acquisition.o \
./Src/fsm.o \
./Src/i2c.o \
./Src/i2cbb.o \
./Src/main.o \
./Src/pwm.o \
./Src/spi.o \
./Src/switch.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/bme280.d \
./Src/buffer.d \
./Src/data_acquisition.d \
./Src/fsm.d \
./Src/i2c.d \
./Src/i2cbb.d \
./Src/main.d \
./Src/pwm.d \
./Src/spi.d \
./Src/switch.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DNUCLEO_F091RC -DSTM32F091RCTx -DSTM32F091xC -c -I../Inc -I../CMSIS -O0 -ffunction-sections -fdata-sections -Wall -Werror -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/bme280.cyclo ./Src/bme280.d ./Src/bme280.o ./Src/bme280.su ./Src/buffer.cyclo ./Src/buffer.d ./Src/buffer.o ./Src/buffer.su ./Src/data_acquisition.cyclo ./Src/data_acquisition.d ./Src/data_acquisition.o ./Src/data_acquisition.su ./Src/fsm.cyclo ./Src/fsm.d ./Src/fsm.o ./Src/fsm.su ./Src/i2c.cyclo ./Src/i2c.d ./Src/i2c.o ./Src/i2c.su ./Src/i2cbb.cyclo ./Src/i2cbb.d ./Src/i2cbb.o ./Src/i2cbb.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/pwm.cyclo ./Src/pwm.d ./Src/pwm.o ./Src/pwm.su ./Src/spi.cyclo ./Src/spi.d ./Src/spi.o ./Src/spi.su ./Src/switch.cyclo ./Src/switch.d ./Src/switch.o ./Src/switch.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su

.PHONY: clean-Src

