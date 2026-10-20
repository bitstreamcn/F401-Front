################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../App/Src/application.cpp \
../App/Src/common.cpp \
../App/Src/cppmain.cpp \
../App/Src/gpiox.cpp \
../App/Src/iicx.cpp \
../App/Src/led.cpp \
../App/Src/log.cpp \
../App/Src/uartx.cpp 

OBJS += \
./App/Src/application.o \
./App/Src/common.o \
./App/Src/cppmain.o \
./App/Src/gpiox.o \
./App/Src/iicx.o \
./App/Src/led.o \
./App/Src/log.o \
./App/Src/uartx.o 

CPP_DEPS += \
./App/Src/application.d \
./App/Src/common.d \
./App/Src/cppmain.d \
./App/Src/gpiox.d \
./App/Src/iicx.d \
./App/Src/led.d \
./App/Src/log.d \
./App/Src/uartx.d 


# Each subdirectory must supply rules for building sources it contributes
App/Src/%.o App/Src/%.su App/Src/%.cyclo: ../App/Src/%.cpp App/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DTX_INCLUDE_USER_DEFINE_FILE -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../AZURE_RTOS/App -I../TOF/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/Components/vl53l5cx/modules -I../Drivers/BSP/Components/vl53l5cx/porting -I../Drivers/BSP/Components/vl53l5cx -I../Middlewares/ST/threadx/common/inc/ -I../Middlewares/ST/threadx/ports/cortex_m4/gnu/inc/ -I"M:/STM32CubeIDE/workspace/F401-Front/App/Inc" -I"M:/STM32CubeIDE/workspace/F401-Front/App/Common" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-App-2f-Src

clean-App-2f-Src:
	-$(RM) ./App/Src/application.cyclo ./App/Src/application.d ./App/Src/application.o ./App/Src/application.su ./App/Src/common.cyclo ./App/Src/common.d ./App/Src/common.o ./App/Src/common.su ./App/Src/cppmain.cyclo ./App/Src/cppmain.d ./App/Src/cppmain.o ./App/Src/cppmain.su ./App/Src/gpiox.cyclo ./App/Src/gpiox.d ./App/Src/gpiox.o ./App/Src/gpiox.su ./App/Src/iicx.cyclo ./App/Src/iicx.d ./App/Src/iicx.o ./App/Src/iicx.su ./App/Src/led.cyclo ./App/Src/led.d ./App/Src/led.o ./App/Src/led.su ./App/Src/log.cyclo ./App/Src/log.d ./App/Src/log.o ./App/Src/log.su ./App/Src/uartx.cyclo ./App/Src/uartx.d ./App/Src/uartx.o ./App/Src/uartx.su

.PHONY: clean-App-2f-Src

