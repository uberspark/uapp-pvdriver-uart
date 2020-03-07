#!/usr/bin/fish

# Generally speaking, this variable controls the kernel build process and tells
# the build process which architecture to cross-compile to.
set -x KERNEL kernel7

# Delete the output directory, if it exists.
if test -e ../my_kernel/
    rm -rf ../my_kernel/
end

# Create output directories.
# TODO: copy over work from a previous effort that makes the module version string a variable.
mkdir -p ../my_kernel/lib/modules/4.19.106-v7+
mkdir -p ../my_kernel/overlays

# Build kernel artifacts.
# Adjust the -j parameter, based on your build system. For example, 24 = 16 (cores) * 1.5 for my system. Build time is about three minutes.
make -j 24 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs

# Copy kernel image and overlays to output dirs.
cp arch/arm/boot/zImage ../my_kernel/$KERNEL.img
cp arch/arm/boot/dts/*.dtb ../my_kernel/
cp arch/arm/boot/dts/overlays/*.dtb* ../my_kernel/overlays/
cp arch/arm/boot/dts/overlays/README ../my_kernel/overlays/

# Copy modules to output dir.
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=(pwd)/../my_kernel modules_install

pushd ../

# Create the archive.
tar cvzf my_kernel.tar.gz my_kernel/*

# TODO: Make IP address configurable.
# TODO: Actually modify the Pi's boot kernel using SSH commands. This is potentially a second script, after tar_builder is ran.
# scp pi@172.26.230.171:./my_kernel.tar.gz /home/pi/my_kernel.tar.gz

popd
