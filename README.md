# linux-tm-4.19.37

Custom Kernel for BCM2837 Raspberry Pi 3 B+, for learning driver development and Kernel Linux environment.

# Get Source

First of all Clone repository:

$ git clone https://github.com/Scott31393/linux-tm-4.19.37.git

# Build the Kernel

Now cd inside downloaded repository:

$ cd linux-tm-4.19.37
$ make bcm2837_tm_defconfig
$ make -j4 Image modules dtbs

We can find compiled arm64 Image in:

$ arch/arm64/boot/Image

And dtb in:

$ arch/arm64/boot/dts/broadcom/bcm2837-rpi-3-b-plus.dtb

# Custom Driver Folder

$ cd linux-tm-4.19.37/drivers/tm-driver/

