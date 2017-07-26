################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lib/dlib/sockstreambuf/sockstreambuf.cpp \
../lib/dlib/sockstreambuf/sockstreambuf_unbuffered.cpp 

OBJS += \
./lib/dlib/sockstreambuf/sockstreambuf.o \
./lib/dlib/sockstreambuf/sockstreambuf_unbuffered.o 

CPP_DEPS += \
./lib/dlib/sockstreambuf/sockstreambuf.d \
./lib/dlib/sockstreambuf/sockstreambuf_unbuffered.d 


# Each subdirectory must supply rules for building sources it contributes
lib/dlib/sockstreambuf/%.o: ../lib/dlib/sockstreambuf/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/lib" -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


