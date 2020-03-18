# Kernel Build Notes

There is a hidden file, named '.config', located in this folder. This is the
config used to build the kernel for the Pi, with the PL011 as a loadable
module. This configuration also has DMA disabled for UARTs, which we require.

If you should need to modify this config:

```bash
KERNEL=kernel7
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
```

Make sure the cross-compiler toolchain is on your path.

Once you've re-configured your kernel, save the configuration and re-run the
fish script, which will render an archive to copy to the Pi over scp.

You should copy the fish script to the top-level of your kernel tree, as it uses relative paths that assume this as a starting location.
