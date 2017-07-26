################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/dlib/external/libpng/arm/arm_init.c \
../lib/dlib/external/libpng/arm/filter_neon_intrinsics.c 

S_UPPER_SRCS += \
../lib/dlib/external/libpng/arm/filter_neon.S 

OBJS += \
./lib/dlib/external/libpng/arm/arm_init.o \
./lib/dlib/external/libpng/arm/filter_neon.o \
./lib/dlib/external/libpng/arm/filter_neon_intrinsics.o 

C_DEPS += \
./lib/dlib/external/libpng/arm/arm_init.d \
./lib/dlib/external/libpng/arm/filter_neon_intrinsics.d 


# Each subdirectory must supply rules for building sources it contributes
lib/dlib/external/libpng/arm/%.o: ../lib/dlib/external/libpng/arm/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D__cplusplus=201103L -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

lib/dlib/external/libpng/arm/%.o: ../lib/dlib/external/libpng/arm/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler'
	as  -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


