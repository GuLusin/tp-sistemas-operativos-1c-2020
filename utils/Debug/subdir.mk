################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../cJSON.c \
../config.c \
../console.c \
../pcb.c \
../socket.c 

OBJS += \
./cJSON.o \
./config.o \
./console.o \
./pcb.o \
./socket.o 

C_DEPS += \
./cJSON.d \
./config.d \
./console.d \
./pcb.d \
./socket.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


