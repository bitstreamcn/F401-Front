################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/Device/PMW3901/interface/driver_pmw3901mb_interface.c 

C_DEPS += \
./App/Device/PMW3901/interface/driver_pmw3901mb_interface.d 

OBJS += \
./App/Device/PMW3901/interface/driver_pmw3901mb_interface.o 


# Each subdirectory must supply rules for building sources it contributes
App/Device/PMW3901/interface/%.o App/Device/PMW3901/interface/%.su App/Device/PMW3901/interface/%.cyclo: ../App/Device/PMW3901/interface/%.c App/Device/PMW3901/interface/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DTX_INCLUDE_USER_DEFINE_FILE -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../AZURE_RTOS/App -I../TOF/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/Components/vl53l5cx/modules -I../Drivers/BSP/Components/vl53l5cx/porting -I../Drivers/BSP/Components/vl53l5cx -I../Middlewares/ST/threadx/common/inc/ -I../Middlewares/ST/threadx/ports/cortex_m4/gnu/inc/ -I"M:/STM32CubeIDE/workspace/F401-Front/App/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-App-2f-Device-2f-PMW3901-2f-interface

clean-App-2f-Device-2f-PMW3901-2f-interface:
	-$(RM) ./App/Device/PMW3901/interface/driver_pmw3901mb_interface.cyclo ./App/Device/PMW3901/interface/driver_pmw3901mb_interface.d ./App/Device/PMW3901/interface/driver_pmw3901mb_interface.o ./App/Device/PMW3901/interface/driver_pmw3901mb_interface.su

.PHONY: clean-App-2f-Device-2f-PMW3901-2f-interface

