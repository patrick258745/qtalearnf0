################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lib/dlib/image_saver/save_jpeg.cpp \
../lib/dlib/image_saver/save_png.cpp 

OBJS += \
./lib/dlib/image_saver/save_jpeg.o \
./lib/dlib/image_saver/save_png.o 

CPP_DEPS += \
./lib/dlib/image_saver/save_jpeg.d \
./lib/dlib/image_saver/save_png.d 


# Each subdirectory must supply rules for building sources it contributes
lib/dlib/image_saver/%.o: ../lib/dlib/image_saver/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/lib" -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


