#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/myos.kernel isodir/boot/myos.kernel
tar --verbose --create --file isodir/boot/myos.initrd --directory=sysroot $(ls sysroot | grep -v boot)
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "myos" {
	multiboot /boot/myos.kernel
	module /boot/myos.initrd
}
EOF
grub-mkrescue -o myos.iso isodir
