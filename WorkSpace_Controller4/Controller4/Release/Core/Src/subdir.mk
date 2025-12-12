################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/HX711.c \
../Core/Src/RTC.c \
../Core/Src/SIMCOM.c \
../Core/Src/Ultrasonic.c \
../Core/Src/VL53L0X.c \
../Core/Src/Voltage.c \
../Core/Src/WATERSENSOR.c \
../Core/Src/main.c \
../Core/Src/main_.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c 

OBJS += \
./Core/Src/HX711.o \
./Core/Src/RTC.o \
./Core/Src/SIMCOM.o \
./Core/Src/Ultrasonic.o \
./Core/Src/VL53L0X.o \
./Core/Src/Voltage.o \
./Core/Src/WATERSENSOR.o \
./Core/Src/main.o \
./Core/Src/main_.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o 

C_DEPS += \
./Core/Src/HX711.d \
./Core/Src/RTC.d \
./Core/Src/SIMCOM.d \
./Core/Src/Ultrasonic.d \
./Core/Src/VL53L0X.d \
./Core/Src/Voltage.d \
./Core/Src/WATERSENSOR.d \
./Core/Src/main.d \
./Core/Src/main_.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/HX711.cyclo ./Core/Src/HX711.d ./Core/Src/HX711.o ./Core/Src/HX711.su ./Core/Src/RTC.cyclo ./Core/Src/RTC.d ./Core/Src/RTC.o ./Core/Src/RTC.su ./Core/Src/SIMCOM.cyclo ./Core/Src/SIMCOM.d ./Core/Src/SIMCOM.o ./Core/Src/SIMCOM.su ./Core/Src/Ultrasonic.cyclo ./Core/Src/Ultrasonic.d ./Core/Src/Ultrasonic.o ./Core/Src/Ultrasonic.su ./Core/Src/VL53L0X.cyclo ./Core/Src/VL53L0X.d ./Core/Src/VL53L0X.o ./Core/Src/VL53L0X.su ./Core/Src/Voltage.cyclo ./Core/Src/Voltage.d ./Core/Src/Voltage.o ./Core/Src/Voltage.su ./Core/Src/WATERSENSOR.cyclo ./Core/Src/WATERSENSOR.d ./Core/Src/WATERSENSOR.o ./Core/Src/WATERSENSOR.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/main_.cyclo ./Core/Src/main_.d ./Core/Src/main_.o ./Core/Src/main_.su ./Core/Src/stm32f1xx_hal_msp.cyclo ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_hal_msp.su ./Core/Src/stm32f1xx_it.cyclo ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.cyclo ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su

.PHONY: clean-Core-2f-Src

