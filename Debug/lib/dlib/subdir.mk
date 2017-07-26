################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lib/dlib/all_console.cpp \
../lib/dlib/all_gui.cpp \
../lib/dlib/stack_trace.cpp 

OBJS += \
./lib/dlib/all_console.o \
./lib/dlib/all_gui.o \
./lib/dlib/stack_trace.o 

CPP_DEPS += \
./lib/dlib/all_console.d \
./lib/dlib/all_gui.d \
./lib/dlib/stack_trace.d 


# Each subdirectory must supply rules for building sources it contributes
lib/dlib/%.o: ../lib/dlib/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/lib" -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


