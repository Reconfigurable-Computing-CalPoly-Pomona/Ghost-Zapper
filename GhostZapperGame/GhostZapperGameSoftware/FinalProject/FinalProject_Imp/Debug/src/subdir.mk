################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

CPP_SRCS += \
../src/adsr_core.cpp \
../src/chu_init.cpp \
../src/ctimer_core.cpp \
../src/ddfs_core.cpp \
../src/gpio_cores.cpp \
../src/i2c_core.cpp \
../src/main_video_test.cpp \
../src/ps2_core.cpp \
../src/spi_core.cpp \
../src/sseg_core.cpp \
../src/timer_core.cpp \
../src/uart_core.cpp \
../src/vga_core.cpp \
../src/xadc_core.cpp 

OBJS += \
./src/adsr_core.o \
./src/chu_init.o \
./src/ctimer_core.o \
./src/ddfs_core.o \
./src/gpio_cores.o \
./src/i2c_core.o \
./src/main_video_test.o \
./src/ps2_core.o \
./src/spi_core.o \
./src/sseg_core.o \
./src/timer_core.o \
./src/uart_core.o \
./src/vga_core.o \
./src/xadc_core.o 

CPP_DEPS += \
./src/adsr_core.d \
./src/chu_init.d \
./src/ctimer_core.d \
./src/ddfs_core.d \
./src/gpio_cores.d \
./src/i2c_core.d \
./src/main_video_test.d \
./src/ps2_core.d \
./src/spi_core.d \
./src/sseg_core.d \
./src/timer_core.d \
./src/uart_core.d \
./src/vga_core.d \
./src/xadc_core.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MicroBlaze g++ compiler'
	mb-g++ -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -I../../FP_Yeetus_BSP/microblaze_I/include -mno-xl-reorder -mlittle-endian -mcpu=v11.0 -mxl-soft-mul -Wl,--no-relax -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


