################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TOF/Target/custom_ranging_sensor.c 

C_DEPS += \
./TOF/Target/custom_ranging_sensor.d 

OBJS += \
./TOF/Target/custom_ranging_sensor.o 


# Each subdirectory must supply rules for building sources it contributes
TOF/Target/%.o TOF/Target/%.su TOF/Target/%.cyclo: ../TOF/Target/%.c TOF/Target/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DTX_INCLUDE_USER_DEFINE_FILE -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../AZURE_RTOS/App -I../TOF/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/Components/vl53l5cx/modules -I../Drivers/BSP/Components/vl53l5cx/porting -I../Drivers/BSP/Components/vl53l5cx -I../Middlewares/ST/threadx/common/inc/ -I../Middlewares/ST/threadx/ports/cortex_m4/gnu/inc/ -I"M:/STM32CubeIDE/workspace/F401-Front/App/Inc" -I"M:/STM32CubeIDE/workspace/F401-Front/App/Common" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TOF-2f-Target

clean-TOF-2f-Target:
	-$(RM) ./TOF/Target/custom_ranging_sensor.cyclo ./TOF/Target/custom_ranging_sensor.d ./TOF/Target/custom_ranging_sensor.o ./TOF/Target/custom_ranging_sensor.su

.PHONY: clean-TOF-2f-Target

