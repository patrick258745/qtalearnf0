################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lib/dlib/bigint/bigint_kernel_1.cpp \
../lib/dlib/bigint/bigint_kernel_2.cpp 

OBJS += \
./lib/dlib/bigint/bigint_kernel_1.o \
./lib/dlib/bigint/bigint_kernel_2.o 

CPP_DEPS += \
./lib/dlib/bigint/bigint_kernel_1.d \
./lib/dlib/bigint/bigint_kernel_2.d 


# Each subdirectory must supply rules for building sources it contributes
lib/dlib/bigint/%.o: ../lib/dlib/bigint/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/lib" -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


