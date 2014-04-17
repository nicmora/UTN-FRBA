################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/funcionesNivel.c \
../src/leerConfigNivel.c \
../src/nivel.c 

OBJS += \
./src/funcionesNivel.o \
./src/leerConfigNivel.o \
./src/nivel.o 

C_DEPS += \
./src/funcionesNivel.d \
./src/leerConfigNivel.d \
./src/nivel.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=c99 -I"/home/utnso/tp-2013-2c-volvimos-en-forma-de-sockets/commons-lib" -I"/home/utnso/tp-2013-2c-volvimos-en-forma-de-sockets/funciones-lib" -I"/home/utnso/tp-2013-2c-volvimos-en-forma-de-sockets/gui-lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


