################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: plataforma

dependents:
	-cd /home/utnso/tp-2013-2c-volvimos-en-forma-de-sockets/commons-lib/Debug && $(MAKE) all
	-cd /home/utnso/tp-2013-2c-volvimos-en-forma-de-sockets/funciones-lib/Debug && $(MAKE) all

# Tool invocations
plataforma: $(OBJS) $(USER_OBJS) /home/utnso/tp-2013-2c-volvimos-en-forma-de-sockets/commons-lib/Debug/libcommons-lib.so /home/utnso/tp-2013-2c-volvimos-en-forma-de-sockets/funciones-lib/Debug/libfunciones-lib.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/tp-2013-2c-volvimos-en-forma-de-sockets/commons-lib/Debug" -L"/home/utnso/tp-2013-2c-volvimos-en-forma-de-sockets/funciones-lib/Debug" -o "plataforma" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) plataforma
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/utnso/tp-2013-2c-volvimos-en-forma-de-sockets/commons-lib/Debug/libcommons-lib.so:
/home/utnso/tp-2013-2c-volvimos-en-forma-de-sockets/funciones-lib/Debug/libfunciones-lib.so:

-include ../makefile.targets
