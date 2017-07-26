################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include lib/dlib/unicode/subdir.mk
-include lib/dlib/tokenizer/subdir.mk
-include lib/dlib/timer/subdir.mk
-include lib/dlib/threads/subdir.mk
-include lib/dlib/test/gui/subdir.mk
-include lib/dlib/test/blas_bindings/subdir.mk
-include lib/dlib/test/subdir.mk
-include lib/dlib/sockstreambuf/subdir.mk
-include lib/dlib/sockets/subdir.mk
-include lib/dlib/server/subdir.mk
-include lib/dlib/misc_api/subdir.mk
-include lib/dlib/md5/subdir.mk
-include lib/dlib/matlab/subdir.mk
-include lib/dlib/logger/subdir.mk
-include lib/dlib/linker/subdir.mk
-include lib/dlib/image_saver/subdir.mk
-include lib/dlib/image_loader/subdir.mk
-include lib/dlib/http_client/subdir.mk
-include lib/dlib/gui_widgets/subdir.mk
-include lib/dlib/gui_core/subdir.mk
-include lib/dlib/external/zlib/subdir.mk
-include lib/dlib/external/libpng/arm/subdir.mk
-include lib/dlib/external/libpng/subdir.mk
-include lib/dlib/external/libjpeg/subdir.mk
-include lib/dlib/external/cblas/subdir.mk
-include lib/dlib/entropy_encoder/subdir.mk
-include lib/dlib/entropy_decoder/subdir.mk
-include lib/dlib/dnn/subdir.mk
-include lib/dlib/dir_nav/subdir.mk
-include lib/dlib/data_io/subdir.mk
-include lib/dlib/cmake_utils/test_for_cpp11/subdir.mk
-include lib/dlib/bsp/subdir.mk
-include lib/dlib/bit_stream/subdir.mk
-include lib/dlib/bigint/subdir.mk
-include lib/dlib/base64/subdir.mk
-include lib/dlib/all/subdir.mk
-include lib/dlib/subdir.mk
-include build/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: qtalearnf0

# Tool invocations
qtalearnf0: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o "qtalearnf0" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(C_UPPER_DEPS)$(CXX_DEPS)$(OBJS)$(CPP_DEPS)$(C_DEPS) qtalearnf0
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
