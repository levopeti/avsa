################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Lab2.0AVSA2020.cpp \
../src/ShowManyImages.cpp \
../src/blobs.cpp 

OBJS += \
./src/Lab2.0AVSA2020.o \
./src/ShowManyImages.o \
./src/blobs.o 

CPP_DEPS += \
./src/Lab2.0AVSA2020.d \
./src/ShowManyImages.d \
./src/blobs.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


