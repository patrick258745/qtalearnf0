################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lib/dlib/server/server_http.cpp \
../lib/dlib/server/server_iostream.cpp \
../lib/dlib/server/server_kernel.cpp 

OBJS += \
./lib/dlib/server/server_http.o \
./lib/dlib/server/server_iostream.o \
./lib/dlib/server/server_kernel.o 

CPP_DEPS += \
./lib/dlib/server/server_http.d \
./lib/dlib/server/server_iostream.d \
./lib/dlib/server/server_kernel.d 


# Each subdirectory must supply rules for building sources it contributes
lib/dlib/server/%.o: ../lib/dlib/server/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/lib" -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


