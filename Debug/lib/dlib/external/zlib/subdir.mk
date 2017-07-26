################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/dlib/external/zlib/adler32.c \
../lib/dlib/external/zlib/compress.c \
../lib/dlib/external/zlib/crc32.c \
../lib/dlib/external/zlib/deflate.c \
../lib/dlib/external/zlib/gzclose.c \
../lib/dlib/external/zlib/gzlib.c \
../lib/dlib/external/zlib/gzread.c \
../lib/dlib/external/zlib/gzwrite.c \
../lib/dlib/external/zlib/infback.c \
../lib/dlib/external/zlib/inffast.c \
../lib/dlib/external/zlib/inflate.c \
../lib/dlib/external/zlib/inftrees.c \
../lib/dlib/external/zlib/trees.c \
../lib/dlib/external/zlib/uncompr.c \
../lib/dlib/external/zlib/zutil.c 

OBJS += \
./lib/dlib/external/zlib/adler32.o \
./lib/dlib/external/zlib/compress.o \
./lib/dlib/external/zlib/crc32.o \
./lib/dlib/external/zlib/deflate.o \
./lib/dlib/external/zlib/gzclose.o \
./lib/dlib/external/zlib/gzlib.o \
./lib/dlib/external/zlib/gzread.o \
./lib/dlib/external/zlib/gzwrite.o \
./lib/dlib/external/zlib/infback.o \
./lib/dlib/external/zlib/inffast.o \
./lib/dlib/external/zlib/inflate.o \
./lib/dlib/external/zlib/inftrees.o \
./lib/dlib/external/zlib/trees.o \
./lib/dlib/external/zlib/uncompr.o \
./lib/dlib/external/zlib/zutil.o 

C_DEPS += \
./lib/dlib/external/zlib/adler32.d \
./lib/dlib/external/zlib/compress.d \
./lib/dlib/external/zlib/crc32.d \
./lib/dlib/external/zlib/deflate.d \
./lib/dlib/external/zlib/gzclose.d \
./lib/dlib/external/zlib/gzlib.d \
./lib/dlib/external/zlib/gzread.d \
./lib/dlib/external/zlib/gzwrite.d \
./lib/dlib/external/zlib/infback.d \
./lib/dlib/external/zlib/inffast.d \
./lib/dlib/external/zlib/inflate.d \
./lib/dlib/external/zlib/inftrees.d \
./lib/dlib/external/zlib/trees.d \
./lib/dlib/external/zlib/uncompr.d \
./lib/dlib/external/zlib/zutil.d 


# Each subdirectory must supply rules for building sources it contributes
lib/dlib/external/zlib/%.o: ../lib/dlib/external/zlib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D__cplusplus=201103L -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


