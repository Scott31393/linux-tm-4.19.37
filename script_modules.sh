#!/bin/bash


#!/bin/bash 
#make imx_v7_defconfig 
export ARCH=arm64 
#FDT=0x12000000 
cd $PWD 
PATH_MOD=$2 
#CROSS_COMPILER="/opt/cross_4.8/gcc-linaro-arm-linux-gnueabihf-4.8-2013.10_linux/bin/arm-linux-gnueabihf-" 
#CROSS_COMPILER="/opt/gcc-4.9.1/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-" 
#CROSS_COMPILER="/opt/yocto/fsl-release-bsp/build_seco/tmp/sysroots/x86_64-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-" 
#CROSS_COMPILER="/opt/yocto/fsl-release-bsp/build_seco_5.2/tmp/sysroots/x86_64-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-" 
CROSS_COMPILER="/opt/gcc-linaro-7.4.1-2019.02-i686_aarch64-linux-gnu/bin/aarch64-linux-gnu-"
LOADADDR=$FDT make -j16  CROSS_COMPILE=$CROSS_COMPILER ARCH=arm64 

if [ "$1" == "m" ]; then 
LOADADDR=$FDT make -j16 modules CROSS_COMPILE=$CROSS_COMPILER 
LOADADDR=$FDT make -j16 modules_install CROSS_COMPILE=$CROSS_COMPILER INSTALL_MOD_PATH=$PATH_MOD 
echo "Changing Kernel" 
mkdir $PATH_MOD/boot/ &> /dev/null 
cp arch/arm64/boot/Image $PATH_MOD/boot/ -f 
echo "done!" 
echo "copying dtb..." 
find arch/arm64/boot/dts/broadcom -name "bcm2710-rpi-3-b-plus.dts" -exec cp {} $PATH_MOD/boot/ \; 
echo "done!" 
##sync 
else 
echo "No Compiling modules!"     
##sync 
fi 
