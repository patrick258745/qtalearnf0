################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lib/dlib/matlab/example_mex_callback.cpp \
../lib/dlib/matlab/example_mex_class.cpp \
../lib/dlib/matlab/example_mex_function.cpp \
../lib/dlib/matlab/example_mex_struct.cpp \
../lib/dlib/matlab/mex_wrapper.cpp \
../lib/dlib/matlab/subprocess_stream.cpp 

OBJS += \
./lib/dlib/matlab/example_mex_callback.o \
./lib/dlib/matlab/example_mex_class.o \
./lib/dlib/matlab/example_mex_function.o \
./lib/dlib/matlab/example_mex_struct.o \
./lib/dlib/matlab/mex_wrapper.o \
./lib/dlib/matlab/subprocess_stream.o 

CPP_DEPS += \
./lib/dlib/matlab/example_mex_callback.d \
./lib/dlib/matlab/example_mex_class.d \
./lib/dlib/matlab/example_mex_function.d \
./lib/dlib/matlab/example_mex_struct.d \
./lib/dlib/matlab/mex_wrapper.d \
./lib/dlib/matlab/subprocess_stream.d 


# Each subdirectory must supply rules for building sources it contributes
lib/dlib/matlab/%.o: ../lib/dlib/matlab/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/lib" -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


