################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lib/dlib/dnn/cpu_dlib.cpp \
../lib/dlib/dnn/cublas_dlibapi.cpp \
../lib/dlib/dnn/cudnn_dlibapi.cpp \
../lib/dlib/dnn/curand_dlibapi.cpp \
../lib/dlib/dnn/gpu_data.cpp \
../lib/dlib/dnn/tensor_tools.cpp 

OBJS += \
./lib/dlib/dnn/cpu_dlib.o \
./lib/dlib/dnn/cublas_dlibapi.o \
./lib/dlib/dnn/cudnn_dlibapi.o \
./lib/dlib/dnn/curand_dlibapi.o \
./lib/dlib/dnn/gpu_data.o \
./lib/dlib/dnn/tensor_tools.o 

CPP_DEPS += \
./lib/dlib/dnn/cpu_dlib.d \
./lib/dlib/dnn/cublas_dlibapi.d \
./lib/dlib/dnn/cudnn_dlibapi.d \
./lib/dlib/dnn/curand_dlibapi.d \
./lib/dlib/dnn/gpu_data.d \
./lib/dlib/dnn/tensor_tools.d 


# Each subdirectory must supply rules for building sources it contributes
lib/dlib/dnn/%.o: ../lib/dlib/dnn/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/lib" -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


