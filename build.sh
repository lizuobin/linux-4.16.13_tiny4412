#!/bin/bash


make ARCH=arm CROSS_COMPILE=arm-4412-linux-gnueabi-  -j8 LOADADDR=0x40008000 uImage
make ARCH=arm CROSS_COMPILE=arm-4412-linux-gnueabi-  -j8 dtbs
make ARCH=arm CROSS_COMPILE=arm-4412-linux-gnueabi-  -j8 modules
make ARCH=arm CROSS_COMPILE=arm-4412-linux-gnueabi-  -j8 modules_install INSTALL_MOD_PATH=../modules
