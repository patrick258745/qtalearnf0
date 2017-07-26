################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/dlib/external/libpng/png.c \
../lib/dlib/external/libpng/pngerror.c \
../lib/dlib/external/libpng/pngget.c \
../lib/dlib/external/libpng/pngmem.c \
../lib/dlib/external/libpng/pngpread.c \
../lib/dlib/external/libpng/pngread.c \
../lib/dlib/external/libpng/pngrio.c \
../lib/dlib/external/libpng/pngrtran.c \
../lib/dlib/external/libpng/pngrutil.c \
../lib/dlib/external/libpng/pngset.c \
../lib/dlib/external/libpng/pngtrans.c \
../lib/dlib/external/libpng/pngwio.c \
../lib/dlib/external/libpng/pngwrite.c \
../lib/dlib/external/libpng/pngwtran.c \
../lib/dlib/external/libpng/pngwutil.c 

OBJS += \
./lib/dlib/external/libpng/png.o \
./lib/dlib/external/libpng/pngerror.o \
./lib/dlib/external/libpng/pngget.o \
./lib/dlib/external/libpng/pngmem.o \
./lib/dlib/external/libpng/pngpread.o \
./lib/dlib/external/libpng/pngread.o \
./lib/dlib/external/libpng/pngrio.o \
./lib/dlib/external/libpng/pngrtran.o \
./lib/dlib/external/libpng/pngrutil.o \
./lib/dlib/external/libpng/pngset.o \
./lib/dlib/external/libpng/pngtrans.o \
./lib/dlib/external/libpng/pngwio.o \
./lib/dlib/external/libpng/pngwrite.o \
./lib/dlib/external/libpng/pngwtran.o \
./lib/dlib/external/libpng/pngwutil.o 

C_DEPS += \
./lib/dlib/external/libpng/png.d \
./lib/dlib/external/libpng/pngerror.d \
./lib/dlib/external/libpng/pngget.d \
./lib/dlib/external/libpng/pngmem.d \
./lib/dlib/external/libpng/pngpread.d \
./lib/dlib/external/libpng/pngread.d \
./lib/dlib/external/libpng/pngrio.d \
./lib/dlib/external/libpng/pngrtran.d \
./lib/dlib/external/libpng/pngrutil.d \
./lib/dlib/external/libpng/pngset.d \
./lib/dlib/external/libpng/pngtrans.d \
./lib/dlib/external/libpng/pngwio.d \
./lib/dlib/external/libpng/pngwrite.d \
./lib/dlib/external/libpng/pngwtran.d \
./lib/dlib/external/libpng/pngwutil.d 


# Each subdirectory must supply rules for building sources it contributes
lib/dlib/external/libpng/%.o: ../lib/dlib/external/libpng/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D__cplusplus=201103L -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


