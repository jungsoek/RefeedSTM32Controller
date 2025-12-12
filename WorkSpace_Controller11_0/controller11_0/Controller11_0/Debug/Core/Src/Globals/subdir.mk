################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Globals/Globals.c 

OBJS += \
./Core/Src/Globals/Globals.o 

C_DEPS += \
./Core/Src/Globals/Globals.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Globals/%.o Core/Src/Globals/%.su Core/Src/Globals/%.cyclo: ../Core/Src/Globals/%.c Core/Src/Globals/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Src/Handlers/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Globals

clean-Core-2f-Src-2f-Globals:
	-$(RM) ./Core/Src/Globals/Globals.cyclo ./Core/Src/Globals/Globals.d ./Core/Src/Globals/Globals.o ./Core/Src/Globals/Globals.su

.PHONY: clean-Core-2f-Src-2f-Globals

