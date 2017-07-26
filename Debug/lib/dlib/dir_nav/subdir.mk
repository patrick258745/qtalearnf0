################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lib/dlib/dir_nav/dir_nav_extensions.cpp \
../lib/dlib/dir_nav/dir_nav_kernel_1.cpp \
../lib/dlib/dir_nav/dir_nav_kernel_2.cpp 

OBJS += \
./lib/dlib/dir_nav/dir_nav_extensions.o \
./lib/dlib/dir_nav/dir_nav_kernel_1.o \
./lib/dlib/dir_nav/dir_nav_kernel_2.o 

CPP_DEPS += \
./lib/dlib/dir_nav/dir_nav_extensions.d \
./lib/dlib/dir_nav/dir_nav_kernel_1.d \
./lib/dlib/dir_nav/dir_nav_kernel_2.d 


# Each subdirectory must supply rules for building sources it contributes
lib/dlib/dir_nav/%.o: ../lib/dlib/dir_nav/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/lib" -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


