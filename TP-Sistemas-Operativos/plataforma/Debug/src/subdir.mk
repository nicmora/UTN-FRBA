################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/funcionesPlataforma.c \
../src/leerConfigPlataforma.c \
../src/plataforma.c 

OBJS += \
./src/funcionesPlataforma.o \
./src/leerConfigPlataforma.o \
./src/plataforma.o 

C_DEPS += \
./src/funcionesPlataforma.d \
./src/leerConfigPlataforma.d \
./src/plataforma.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=c99 -I"/home/utnso/tp-2013-2c-volvimos-en-forma-de-sockets/commons-lib" -I"/home/utnso/tp-2013-2c-volvimos-en-forma-de-sockets/funciones-lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


