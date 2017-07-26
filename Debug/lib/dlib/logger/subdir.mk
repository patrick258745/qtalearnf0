################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lib/dlib/logger/extra_logger_headers.cpp \
../lib/dlib/logger/logger_config_file.cpp \
../lib/dlib/logger/logger_kernel_1.cpp 

OBJS += \
./lib/dlib/logger/extra_logger_headers.o \
./lib/dlib/logger/logger_config_file.o \
./lib/dlib/logger/logger_kernel_1.o 

CPP_DEPS += \
./lib/dlib/logger/extra_logger_headers.d \
./lib/dlib/logger/logger_config_file.d \
./lib/dlib/logger/logger_kernel_1.d 


# Each subdirectory must supply rules for building sources it contributes
lib/dlib/logger/%.o: ../lib/dlib/logger/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/lib" -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


