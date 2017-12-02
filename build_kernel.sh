#!/bin/bash

# DTBS Codes
DTBH_PLATFORM_CODE=0x50a6
DTBH_SUBTYPE_CODE=0x217584da

# Target ARCH
export ARCH=arm64

# Make output folder
if [ ! -d $(pwd)/output ];
    then
        mkdir $(pwd)/output;
    fi

# Configure build
make -C $(pwd) O=output ARCH=arm64 "lineage-a3xelte_defconfig"

# Launch build
make -j8 -C $(pwd) O=output ARCH=arm64

# Make dt.img
$(pwd)/dtbTool  -o "$(pwd)/output/arch/arm64/boot/dt.img" -s 2048 -d "$(pwd)/output/arch/arm64/boot/dts/" --platform $DTBH_PLATFORM_CODE --subtype $DTBH_SUBTYPE_CODE

exit
