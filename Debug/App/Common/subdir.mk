################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/Common/ring_buffer.c 

C_DEPS += \
./App/Common/ring_buffer.d 

OBJS += \
./App/Common/ring_buffer.o 


# Each subdirectory must supply rules for building sources it contributes
App/Common/%.o App/Common/%.su App/Common/%.cyclo: ../App/Common/%.c App/Common/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DTX_INCLUDE_USER_DEFINE_FILE -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../AZURE_RTOS/App -I../TOF/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/Components/vl53l5cx/modules -I../Drivers/BSP/Components/vl53l5cx/porting -I../Drivers/BSP/Components/vl53l5cx -I../Middlewares/ST/threadx/common/inc/ -I../Middlewares/ST/threadx/ports/cortex_m4/gnu/inc/ -I"M:/STM32CubeIDE/workspace/F401-Front/App/Inc" -I"M:/STM32CubeIDE/workspace/F401-Front/App/Common" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-App-2f-Common

clean-App-2f-Common:
	-$(RM) ./App/Common/ring_buffer.cyclo ./App/Common/ring_buffer.d ./App/Common/ring_buffer.o ./App/Common/ring_buffer.su

.PHONY: clean-App-2f-Common

