################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/DummyModules/DummySIMModule.c 

OBJS += \
./Core/Src/DummyModules/DummySIMModule.o 

C_DEPS += \
./Core/Src/DummyModules/DummySIMModule.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/DummyModules/%.o Core/Src/DummyModules/%.su Core/Src/DummyModules/%.cyclo: ../Core/Src/DummyModules/%.c Core/Src/DummyModules/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Src/Handlers/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-DummyModules

clean-Core-2f-Src-2f-DummyModules:
	-$(RM) ./Core/Src/DummyModules/DummySIMModule.cyclo ./Core/Src/DummyModules/DummySIMModule.d ./Core/Src/DummyModules/DummySIMModule.o ./Core/Src/DummyModules/DummySIMModule.su

.PHONY: clean-Core-2f-Src-2f-DummyModules

