################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/mlasampling.cpp \
../src/mlatraining.cpp \
../src/model.cpp \
../src/qtamodel.cpp \
../src/qtatools.cpp \
../src/sampling.cpp \
../src/tools.cpp \
../src/training.cpp 

OBJS += \
./src/mlasampling.o \
./src/mlatraining.o \
./src/model.o \
./src/qtamodel.o \
./src/qtatools.o \
./src/sampling.o \
./src/tools.o \
./src/training.o 

CPP_DEPS += \
./src/mlasampling.d \
./src/mlatraining.d \
./src/model.d \
./src/qtamodel.d \
./src/qtatools.d \
./src/sampling.d \
./src/tools.d \
./src/training.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/lib" -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


