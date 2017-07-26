################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lib/dlib/threads/async.cpp \
../lib/dlib/threads/multithreaded_object_extension.cpp \
../lib/dlib/threads/thread_pool_extension.cpp \
../lib/dlib/threads/threaded_object_extension.cpp \
../lib/dlib/threads/threads_kernel_1.cpp \
../lib/dlib/threads/threads_kernel_2.cpp \
../lib/dlib/threads/threads_kernel_shared.cpp 

OBJS += \
./lib/dlib/threads/async.o \
./lib/dlib/threads/multithreaded_object_extension.o \
./lib/dlib/threads/thread_pool_extension.o \
./lib/dlib/threads/threaded_object_extension.o \
./lib/dlib/threads/threads_kernel_1.o \
./lib/dlib/threads/threads_kernel_2.o \
./lib/dlib/threads/threads_kernel_shared.o 

CPP_DEPS += \
./lib/dlib/threads/async.d \
./lib/dlib/threads/multithreaded_object_extension.d \
./lib/dlib/threads/thread_pool_extension.d \
./lib/dlib/threads/threaded_object_extension.d \
./lib/dlib/threads/threads_kernel_1.d \
./lib/dlib/threads/threads_kernel_2.d \
./lib/dlib/threads/threads_kernel_shared.d 


# Each subdirectory must supply rules for building sources it contributes
lib/dlib/threads/%.o: ../lib/dlib/threads/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/lib" -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


