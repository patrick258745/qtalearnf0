################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lib/dlib/gui_widgets/base_widgets.cpp \
../lib/dlib/gui_widgets/canvas_drawing.cpp \
../lib/dlib/gui_widgets/drawable.cpp \
../lib/dlib/gui_widgets/fonts.cpp \
../lib/dlib/gui_widgets/style.cpp \
../lib/dlib/gui_widgets/widgets.cpp 

OBJS += \
./lib/dlib/gui_widgets/base_widgets.o \
./lib/dlib/gui_widgets/canvas_drawing.o \
./lib/dlib/gui_widgets/drawable.o \
./lib/dlib/gui_widgets/fonts.o \
./lib/dlib/gui_widgets/style.o \
./lib/dlib/gui_widgets/widgets.o 

CPP_DEPS += \
./lib/dlib/gui_widgets/base_widgets.d \
./lib/dlib/gui_widgets/canvas_drawing.d \
./lib/dlib/gui_widgets/drawable.d \
./lib/dlib/gui_widgets/fonts.d \
./lib/dlib/gui_widgets/style.d \
./lib/dlib/gui_widgets/widgets.d 


# Each subdirectory must supply rules for building sources it contributes
lib/dlib/gui_widgets/%.o: ../lib/dlib/gui_widgets/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/lib" -I"/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


