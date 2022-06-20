################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/module/do_ph-2.cpp \
../Core/Src/module/do_ph_cmd.cpp 

OBJS += \
./Core/Src/module/do_ph-2.o \
./Core/Src/module/do_ph_cmd.o 

CPP_DEPS += \
./Core/Src/module/do_ph-2.d \
./Core/Src/module/do_ph_cmd.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/module/%.o: ../Core/Src/module/%.cpp Core/Src/module/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-module

clean-Core-2f-Src-2f-module:
	-$(RM) ./Core/Src/module/do_ph-2.d ./Core/Src/module/do_ph-2.o ./Core/Src/module/do_ph_cmd.d ./Core/Src/module/do_ph_cmd.o

.PHONY: clean-Core-2f-Src-2f-module

