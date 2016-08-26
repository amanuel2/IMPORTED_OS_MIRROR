SYSTEM_HEADER_PROJECTS="libc kernel"
PROJECTS="libc hello kernel"

export MAKE=${MAKE:-make}
export HOST=${HOST:-$(./default-host.sh)}

export AR=${HOST}-ar
export AS=${HOST}-as
export CC=${HOST}-gcc

export PREFIX=/usr
export EXEC_PREFIX=$PREFIX
export BOOTDIR=/boot
export LIBDIR=$EXEC_PREFIX/lib
export INCLUDEDIR=$PREFIX/include

export CFLAGS='-O2 -g'
export CPPFLAGS=''

# Configure the cross-compiler to use the desired system root.
export CC="$CC --sysroot=$PWD/sysroot"

# Work around that the -elf gcc targets doesn't have a system include directory
# because configure received --without-headers rather than --with-sysroot.
if echo "$HOST" | grep -Eq -- '-elf($|-)'; then
  export CC="$CC -isystem=$INCLUDEDIR"
fi

# Work around that the -elf gcc targets doesn't link in crti.o and crtn.o.
if echo "$HOST" | grep -Eq -- '-elf($|-)'; then
  export CPPFLAGS="$CPPFLAGS -D__HAS_NO_CRT_INIT"
fi
