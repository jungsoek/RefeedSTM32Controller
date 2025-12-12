################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Handlers/AlarmWakeUp.c \
../Core/Src/Handlers/CloseInput.c \
../Core/Src/Handlers/CloseValve.c \
../Core/Src/Handlers/EXTIWakeUp.c \
../Core/Src/Handlers/FORMAT.c \
../Core/Src/Handlers/Handshake.c \
../Core/Src/Handlers/OpenInput.c \
../Core/Src/Handlers/OpenValve.c \
../Core/Src/Handlers/PostPer.c \
../Core/Src/Handlers/PostUCO.c \
../Core/Src/Handlers/Recheck.c \
../Core/Src/Handlers/Setting.c \
../Core/Src/Handlers/Sleep.c \
../Core/Src/Handlers/TestMeasure.c \
../Core/Src/Handlers/TestPercent.c \
../Core/Src/Handlers/TestSDClose.c \
../Core/Src/Handlers/TestSDOpen.c \
../Core/Src/Handlers/TurnOff.c \
../Core/Src/Handlers/UnlockDoor.c \
../Core/Src/Handlers/Validation.c 

OBJS += \
./Core/Src/Handlers/AlarmWakeUp.o \
./Core/Src/Handlers/CloseInput.o \
./Core/Src/Handlers/CloseValve.o \
./Core/Src/Handlers/EXTIWakeUp.o \
./Core/Src/Handlers/FORMAT.o \
./Core/Src/Handlers/Handshake.o \
./Core/Src/Handlers/OpenInput.o \
./Core/Src/Handlers/OpenValve.o \
./Core/Src/Handlers/PostPer.o \
./Core/Src/Handlers/PostUCO.o \
./Core/Src/Handlers/Recheck.o \
./Core/Src/Handlers/Setting.o \
./Core/Src/Handlers/Sleep.o \
./Core/Src/Handlers/TestMeasure.o \
./Core/Src/Handlers/TestPercent.o \
./Core/Src/Handlers/TestSDClose.o \
./Core/Src/Handlers/TestSDOpen.o \
./Core/Src/Handlers/TurnOff.o \
./Core/Src/Handlers/UnlockDoor.o \
./Core/Src/Handlers/Validation.o 

C_DEPS += \
./Core/Src/Handlers/AlarmWakeUp.d \
./Core/Src/Handlers/CloseInput.d \
./Core/Src/Handlers/CloseValve.d \
./Core/Src/Handlers/EXTIWakeUp.d \
./Core/Src/Handlers/FORMAT.d \
./Core/Src/Handlers/Handshake.d \
./Core/Src/Handlers/OpenInput.d \
./Core/Src/Handlers/OpenValve.d \
./Core/Src/Handlers/PostPer.d \
./Core/Src/Handlers/PostUCO.d \
./Core/Src/Handlers/Recheck.d \
./Core/Src/Handlers/Setting.d \
./Core/Src/Handlers/Sleep.d \
./Core/Src/Handlers/TestMeasure.d \
./Core/Src/Handlers/TestPercent.d \
./Core/Src/Handlers/TestSDClose.d \
./Core/Src/Handlers/TestSDOpen.d \
./Core/Src/Handlers/TurnOff.d \
./Core/Src/Handlers/UnlockDoor.d \
./Core/Src/Handlers/Validation.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Handlers/%.o Core/Src/Handlers/%.su Core/Src/Handlers/%.cyclo: ../Core/Src/Handlers/%.c Core/Src/Handlers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Src/Handlers/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Handlers

clean-Core-2f-Src-2f-Handlers:
	-$(RM) ./Core/Src/Handlers/AlarmWakeUp.cyclo ./Core/Src/Handlers/AlarmWakeUp.d ./Core/Src/Handlers/AlarmWakeUp.o ./Core/Src/Handlers/AlarmWakeUp.su ./Core/Src/Handlers/CloseInput.cyclo ./Core/Src/Handlers/CloseInput.d ./Core/Src/Handlers/CloseInput.o ./Core/Src/Handlers/CloseInput.su ./Core/Src/Handlers/CloseValve.cyclo ./Core/Src/Handlers/CloseValve.d ./Core/Src/Handlers/CloseValve.o ./Core/Src/Handlers/CloseValve.su ./Core/Src/Handlers/EXTIWakeUp.cyclo ./Core/Src/Handlers/EXTIWakeUp.d ./Core/Src/Handlers/EXTIWakeUp.o ./Core/Src/Handlers/EXTIWakeUp.su ./Core/Src/Handlers/FORMAT.cyclo ./Core/Src/Handlers/FORMAT.d ./Core/Src/Handlers/FORMAT.o ./Core/Src/Handlers/FORMAT.su ./Core/Src/Handlers/Handshake.cyclo ./Core/Src/Handlers/Handshake.d ./Core/Src/Handlers/Handshake.o ./Core/Src/Handlers/Handshake.su ./Core/Src/Handlers/OpenInput.cyclo ./Core/Src/Handlers/OpenInput.d ./Core/Src/Handlers/OpenInput.o ./Core/Src/Handlers/OpenInput.su ./Core/Src/Handlers/OpenValve.cyclo ./Core/Src/Handlers/OpenValve.d ./Core/Src/Handlers/OpenValve.o ./Core/Src/Handlers/OpenValve.su ./Core/Src/Handlers/PostPer.cyclo ./Core/Src/Handlers/PostPer.d ./Core/Src/Handlers/PostPer.o ./Core/Src/Handlers/PostPer.su ./Core/Src/Handlers/PostUCO.cyclo ./Core/Src/Handlers/PostUCO.d ./Core/Src/Handlers/PostUCO.o ./Core/Src/Handlers/PostUCO.su ./Core/Src/Handlers/Recheck.cyclo ./Core/Src/Handlers/Recheck.d ./Core/Src/Handlers/Recheck.o ./Core/Src/Handlers/Recheck.su ./Core/Src/Handlers/Setting.cyclo ./Core/Src/Handlers/Setting.d ./Core/Src/Handlers/Setting.o ./Core/Src/Handlers/Setting.su ./Core/Src/Handlers/Sleep.cyclo ./Core/Src/Handlers/Sleep.d ./Core/Src/Handlers/Sleep.o ./Core/Src/Handlers/Sleep.su ./Core/Src/Handlers/TestMeasure.cyclo ./Core/Src/Handlers/TestMeasure.d ./Core/Src/Handlers/TestMeasure.o ./Core/Src/Handlers/TestMeasure.su ./Core/Src/Handlers/TestPercent.cyclo ./Core/Src/Handlers/TestPercent.d ./Core/Src/Handlers/TestPercent.o ./Core/Src/Handlers/TestPercent.su ./Core/Src/Handlers/TestSDClose.cyclo ./Core/Src/Handlers/TestSDClose.d ./Core/Src/Handlers/TestSDClose.o ./Core/Src/Handlers/TestSDClose.su ./Core/Src/Handlers/TestSDOpen.cyclo ./Core/Src/Handlers/TestSDOpen.d ./Core/Src/Handlers/TestSDOpen.o ./Core/Src/Handlers/TestSDOpen.su ./Core/Src/Handlers/TurnOff.cyclo ./Core/Src/Handlers/TurnOff.d ./Core/Src/Handlers/TurnOff.o ./Core/Src/Handlers/TurnOff.su ./Core/Src/Handlers/UnlockDoor.cyclo ./Core/Src/Handlers/UnlockDoor.d ./Core/Src/Handlers/UnlockDoor.o ./Core/Src/Handlers/UnlockDoor.su ./Core/Src/Handlers/Validation.cyclo ./Core/Src/Handlers/Validation.d ./Core/Src/Handlers/Validation.o ./Core/Src/Handlers/Validation.su

.PHONY: clean-Core-2f-Src-2f-Handlers

