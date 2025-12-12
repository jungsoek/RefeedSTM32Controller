################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Handlers/AlarmWakeUp.c \
../Core/Src/Handlers/FORMAT.c \
../Core/Src/Handlers/OpenInput.c \
../Core/Src/Handlers/PostPer.c \
../Core/Src/Handlers/PostUCO.c \
../Core/Src/Handlers/Recheck.c \
../Core/Src/Handlers/Sleep.c \
../Core/Src/Handlers/UnlockDoor.c \
../Core/Src/Handlers/Validation.c \
../Core/Src/Handlers/closeInput.c \
../Core/Src/Handlers/handshake.c 

OBJS += \
./Core/Src/Handlers/AlarmWakeUp.o \
./Core/Src/Handlers/FORMAT.o \
./Core/Src/Handlers/OpenInput.o \
./Core/Src/Handlers/PostPer.o \
./Core/Src/Handlers/PostUCO.o \
./Core/Src/Handlers/Recheck.o \
./Core/Src/Handlers/Sleep.o \
./Core/Src/Handlers/UnlockDoor.o \
./Core/Src/Handlers/Validation.o \
./Core/Src/Handlers/closeInput.o \
./Core/Src/Handlers/handshake.o 

C_DEPS += \
./Core/Src/Handlers/AlarmWakeUp.d \
./Core/Src/Handlers/FORMAT.d \
./Core/Src/Handlers/OpenInput.d \
./Core/Src/Handlers/PostPer.d \
./Core/Src/Handlers/PostUCO.d \
./Core/Src/Handlers/Recheck.d \
./Core/Src/Handlers/Sleep.d \
./Core/Src/Handlers/UnlockDoor.d \
./Core/Src/Handlers/Validation.d \
./Core/Src/Handlers/closeInput.d \
./Core/Src/Handlers/handshake.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Handlers/%.o Core/Src/Handlers/%.su Core/Src/Handlers/%.cyclo: ../Core/Src/Handlers/%.c Core/Src/Handlers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Handlers

clean-Core-2f-Src-2f-Handlers:
	-$(RM) ./Core/Src/Handlers/AlarmWakeUp.cyclo ./Core/Src/Handlers/AlarmWakeUp.d ./Core/Src/Handlers/AlarmWakeUp.o ./Core/Src/Handlers/AlarmWakeUp.su ./Core/Src/Handlers/FORMAT.cyclo ./Core/Src/Handlers/FORMAT.d ./Core/Src/Handlers/FORMAT.o ./Core/Src/Handlers/FORMAT.su ./Core/Src/Handlers/OpenInput.cyclo ./Core/Src/Handlers/OpenInput.d ./Core/Src/Handlers/OpenInput.o ./Core/Src/Handlers/OpenInput.su ./Core/Src/Handlers/PostPer.cyclo ./Core/Src/Handlers/PostPer.d ./Core/Src/Handlers/PostPer.o ./Core/Src/Handlers/PostPer.su ./Core/Src/Handlers/PostUCO.cyclo ./Core/Src/Handlers/PostUCO.d ./Core/Src/Handlers/PostUCO.o ./Core/Src/Handlers/PostUCO.su ./Core/Src/Handlers/Recheck.cyclo ./Core/Src/Handlers/Recheck.d ./Core/Src/Handlers/Recheck.o ./Core/Src/Handlers/Recheck.su ./Core/Src/Handlers/Sleep.cyclo ./Core/Src/Handlers/Sleep.d ./Core/Src/Handlers/Sleep.o ./Core/Src/Handlers/Sleep.su ./Core/Src/Handlers/UnlockDoor.cyclo ./Core/Src/Handlers/UnlockDoor.d ./Core/Src/Handlers/UnlockDoor.o ./Core/Src/Handlers/UnlockDoor.su ./Core/Src/Handlers/Validation.cyclo ./Core/Src/Handlers/Validation.d ./Core/Src/Handlers/Validation.o ./Core/Src/Handlers/Validation.su ./Core/Src/Handlers/closeInput.cyclo ./Core/Src/Handlers/closeInput.d ./Core/Src/Handlers/closeInput.o ./Core/Src/Handlers/closeInput.su ./Core/Src/Handlers/handshake.cyclo ./Core/Src/Handlers/handshake.d ./Core/Src/Handlers/handshake.o ./Core/Src/Handlers/handshake.su

.PHONY: clean-Core-2f-Src-2f-Handlers

