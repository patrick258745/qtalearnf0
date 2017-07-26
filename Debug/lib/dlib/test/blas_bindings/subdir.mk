################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lib/dlib/test/blas_bindings/blas_bindings_dot.cpp \
../lib/dlib/test/blas_bindings/blas_bindings_gemm.cpp \
../lib/dlib/test/blas_bindings/blas_bindings_gemv.cpp \
../lib/dlib/test/blas_bindings/blas_bindings_ger.cpp \
../lib/dlib/test/blas_bindings/blas_bindings_scal_axpy.cpp \
../lib/dlib/test/blas_bindings/vector.cpp 

OBJS += \
./lib/dlib/test/blas_bindings/blas_bindings_dot.o \
./lib/dlib/test/blas_bindings/blas_bindings_gemm.o \
./lib/dlib/test/blas_bindings/blas_bindings_gemv.o \
./lib/dlib/test/blas_bindings/blas_bindings_ger.o \
./lib/dlib/test/blas_bindings/blas_bindings_scal_axpy.o \
./lib/dlib/test/blas_bindings/vector.o 

CPP_DEPS += \
./lib/dlib/test/blas_bindings/blas_bindings_dot.d \
./lib/dlib/test/blas_bindings/blas_bindings_gemm.d \
./lib/dlib/test/blas_bindings/blas_bindings_gemv.d \
./lib/dlib/test/blas_bindings/blas_bindings_ger.d \
./lib/dlib/test/blas_bindings/blas_bindings_scal_axpy.d \
./lib/dlib/test/blas_bindings/vector.d 


# Each subdirectory must supply rules for building sources it contributes
lib/dlib/test/blas_bindings/%.o: ../lib/dlib/test/blas_bindings/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/lib" -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


