################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Startup/startup_stm32f091rctx.c 

OBJS += \
./Startup/startup_stm32f091rctx.o 

C_DEPS += \
./Startup/startup_stm32f091rctx.d 


# Each subdirectory must supply rules for building sources it contributes
Startup/%.o Startup/%.su Startup/%.cyclo: ../Startup/%.c Startup/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DNUCLEO_F091RC -DSTM32F091RCTx -DSTM32F091xC -c -I../Inc -I../CMSIS -O0 -ffunction-sections -fdata-sections -Wall -Werror -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Startup

clean-Startup:
	-$(RM) ./Startup/startup_stm32f091rctx.cyclo ./Startup/startup_stm32f091rctx.d ./Startup/startup_stm32f091rctx.o ./Startup/startup_stm32f091rctx.su

.PHONY: clean-Startup

