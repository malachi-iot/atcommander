#!/bin/bash

export MBED_TARGET=$(mbed target | sed 's/\[mbed\] //')
export MBED_TOOLCHAIN=$(mbed toolchain | sed 's/\[mbed\] //')

PROJECT_NAME=${PWD##*/}

STM_MEM_ADDR=0x8000000

st-flash write BUILD/$MBED_TARGET/$MBED_TOOLCHAIN/$PROJECT_NAME.bin $STM_MEM_ADDR
