################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lib/dlib/cmake_utils/test_for_cpp11/cpp11_test.cpp 

OBJS += \
./lib/dlib/cmake_utils/test_for_cpp11/cpp11_test.o 

CPP_DEPS += \
./lib/dlib/cmake_utils/test_for_cpp11/cpp11_test.d 


# Each subdirectory must supply rules for building sources it contributes
lib/dlib/cmake_utils/test_for_cpp11/%.o: ../lib/dlib/cmake_utils/test_for_cpp11/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/lib" -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


