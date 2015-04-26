################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/common.c \
../src/differences.c \
../src/huffman.c \
../src/queue.c \
../src/tree.c 

OBJS += \
./src/common.o \
./src/differences.o \
./src/huffman.o \
./src/queue.o \
./src/tree.o 

C_DEPS += \
./src/common.d \
./src/differences.d \
./src/huffman.d \
./src/queue.d \
./src/tree.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=c90 -O0 -g -Wall -Wextra -c -fmessage-length=0 -ansi -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


