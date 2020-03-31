These files are the original, un-modified versions, found in the linux kernel source tree at:

`linux/drivers/tty/serial`

Source tree:

https://github.com/raspberrypi/linux/tree/rpi-4.19.y


amba-pl011.c:

Line 24 includes the header file for loadable module support.
`#include <linux/module.h>`

Starting at line 2900 of amba-pl011.c, proof of loadable module.
```
/*
 * While this can be a module, if builtin it's most likely the console
 * So let's leave module_exit but move module_init to an earlier place
 */
arch_initcall(pl011_init);
module_exit(pl011_exit);
```
