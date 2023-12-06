################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/core_m4.c \
../Core/Src/exti.c \
../Core/Src/general_timers.c \
../Core/Src/gpio.c \
../Core/Src/ir_sensors.c \
../Core/Src/main.c \
../Core/Src/reaction.c \
../Core/Src/serial.c \
../Core/Src/slapper.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/stm_rcc.c \
../Core/Src/syscalls.c \
../Core/Src/sysconfig.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/timers.c 

OBJS += \
./Core/Src/core_m4.o \
./Core/Src/exti.o \
./Core/Src/general_timers.o \
./Core/Src/gpio.o \
./Core/Src/ir_sensors.o \
./Core/Src/main.o \
./Core/Src/reaction.o \
./Core/Src/serial.o \
./Core/Src/slapper.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/stm_rcc.o \
./Core/Src/syscalls.o \
./Core/Src/sysconfig.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/timers.o 

C_DEPS += \
./Core/Src/core_m4.d \
./Core/Src/exti.d \
./Core/Src/general_timers.d \
./Core/Src/gpio.d \
./Core/Src/ir_sensors.d \
./Core/Src/main.d \
./Core/Src/reaction.d \
./Core/Src/serial.d \
./Core/Src/slapper.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/stm_rcc.d \
./Core/Src/syscalls.d \
./Core/Src/sysconfig.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/timers.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/core_m4.cyclo ./Core/Src/core_m4.d ./Core/Src/core_m4.o ./Core/Src/core_m4.su ./Core/Src/exti.cyclo ./Core/Src/exti.d ./Core/Src/exti.o ./Core/Src/exti.su ./Core/Src/general_timers.cyclo ./Core/Src/general_timers.d ./Core/Src/general_timers.o ./Core/Src/general_timers.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/ir_sensors.cyclo ./Core/Src/ir_sensors.d ./Core/Src/ir_sensors.o ./Core/Src/ir_sensors.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/reaction.cyclo ./Core/Src/reaction.d ./Core/Src/reaction.o ./Core/Src/reaction.su ./Core/Src/serial.cyclo ./Core/Src/serial.d ./Core/Src/serial.o ./Core/Src/serial.su ./Core/Src/slapper.cyclo ./Core/Src/slapper.d ./Core/Src/slapper.o ./Core/Src/slapper.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/stm_rcc.cyclo ./Core/Src/stm_rcc.d ./Core/Src/stm_rcc.o ./Core/Src/stm_rcc.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysconfig.cyclo ./Core/Src/sysconfig.d ./Core/Src/sysconfig.o ./Core/Src/sysconfig.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/timers.cyclo ./Core/Src/timers.d ./Core/Src/timers.o ./Core/Src/timers.su

.PHONY: clean-Core-2f-Src

