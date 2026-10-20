################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_api.c \
../Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_detection_thresholds.c \
../Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_motion_indicator.c \
../Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_xtalk.c 

C_DEPS += \
./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_api.d \
./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_detection_thresholds.d \
./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_motion_indicator.d \
./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_xtalk.d 

OBJS += \
./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_api.o \
./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_detection_thresholds.o \
./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_motion_indicator.o \
./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_xtalk.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/vl53l5cx/modules/%.o Drivers/BSP/Components/vl53l5cx/modules/%.su Drivers/BSP/Components/vl53l5cx/modules/%.cyclo: ../Drivers/BSP/Components/vl53l5cx/modules/%.c Drivers/BSP/Components/vl53l5cx/modules/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DTX_INCLUDE_USER_DEFINE_FILE -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../AZURE_RTOS/App -I../TOF/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/Components/vl53l5cx/modules -I../Drivers/BSP/Components/vl53l5cx/porting -I../Drivers/BSP/Components/vl53l5cx -I../Middlewares/ST/threadx/common/inc/ -I../Middlewares/ST/threadx/ports/cortex_m4/gnu/inc/ -I"M:/STM32CubeIDE/workspace/F401-Front/App/Inc" -I"M:/STM32CubeIDE/workspace/F401-Front/App/Common" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components-2f-vl53l5cx-2f-modules

clean-Drivers-2f-BSP-2f-Components-2f-vl53l5cx-2f-modules:
	-$(RM) ./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_api.cyclo ./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_api.d ./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_api.o ./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_api.su ./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_detection_thresholds.cyclo ./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_detection_thresholds.d ./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_detection_thresholds.o ./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_detection_thresholds.su ./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_motion_indicator.cyclo ./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_motion_indicator.d ./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_motion_indicator.o ./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_motion_indicator.su ./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_xtalk.cyclo ./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_xtalk.d ./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_xtalk.o ./Drivers/BSP/Components/vl53l5cx/modules/vl53l5cx_plugin_xtalk.su

.PHONY: clean-Drivers-2f-BSP-2f-Components-2f-vl53l5cx-2f-modules

