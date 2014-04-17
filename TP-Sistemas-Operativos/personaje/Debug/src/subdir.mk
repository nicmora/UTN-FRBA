################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/funcionesPersonaje.c \
../src/leerConfigPersonaje.c \
../src/personaje.c 

OBJS += \
./src/funcionesPersonaje.o \
./src/leerConfigPersonaje.o \
./src/personaje.o 

C_DEPS += \
./src/funcionesPersonaje.d \
./src/leerConfigPersonaje.d \
./src/personaje.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=c99 -I"/home/utnso/tp-2013-2c-volvimos-en-forma-de-sockets/commons-lib" -I"/home/utnso/tp-2013-2c-volvimos-en-forma-de-sockets/funciones-lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


