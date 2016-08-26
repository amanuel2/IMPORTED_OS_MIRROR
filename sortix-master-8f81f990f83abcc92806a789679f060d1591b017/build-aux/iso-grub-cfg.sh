#!/bin/sh -e

this=$(which -- "$0")
thisdir=$(dirname -- "$this")

platform=
directory=
version=

dashdash=
previous_option=
for argument do
  if test -n "$previous_option"; then
    eval $previous_option=\$argument
    previous_option=
    continue
  fi

  case $argument in
  *=?*) parameter=$(expr "X$argument" : '[^=]*=\(.*\)') ;;
  *=)   parameter= ;;
  *)    parameter=yes ;;
  esac

  case $dashdash$argument in
  --) dashdash=yes ;;
  --platform=*) platform=$parameter ;;
  --platform) previous_option=platform ;;
  --version=*) version=$parameter ;;
  --version) previous_option=version ;;
  -*) echo "$0: unrecognized option $argument" >&2
      exit 1 ;;
  *) directory="$argument" ;;
  esac
done

if test -n "$previous_option"; then
  echo "$0: option '$argument' requires an argument" >&2
  exit 1
fi

if test -z "$platform"; then
  echo "$0: platform wasn't set with --platform" >&2
  exit 1
fi

if test -z "$version"; then
  echo "$0: platform wasn't set with --version" >&2
  exit 1
fi

if test -z "$directory"; then
  echo "$0: no directory operand supplied" >&2
  exit 1
fi

machine=$(expr x"$platform" : 'x\([^-]*\).*')

maybe_compressed() {
  if [ -e "$1.xz" ]; then
    echo "$1.xz"
  elif [ -e "$1.gz" ]; then
    echo "$1.gz"
  elif [ -e "$1" ]; then
    echo "$1"
  fi
}

human_size() {
  LC_ALL=C du -bh "$1" | grep -Eo '^[^[:space:]]+'
}

portvar() {
  echo "$1" | sed -e 's/-/_/g' -e 's/+/x/g'
}

isinset() {
  (for port in $2; do
     if [ x"$1" = x"$port" ]; then
       echo true
       exit 0
     fi
   done
   echo false
   exit 0)
}

. "$thisdir/ports.conf"

cd "$directory"

kernel=$(maybe_compressed boot/sortix.bin)
live_initrd=$(maybe_compressed boot/live.initrd)
overlay_initrd=$(maybe_compressed boot/overlay.initrd)
src_initrd=$(maybe_compressed boot/src.initrd)
system_initrd=$(maybe_compressed boot/system.initrd)
ports=$(ls repository | sed 's/\.tix\.tar\.xz//')

mkdir -p boot/grub
exec > boot/grub/grub.cfg

cat << EOF
insmod part_msdos
insmod ext2
EOF
find . | grep -Eq '\.gz$' && echo "insmod gzio"
find . | grep -Eq '\.xz$' && echo "insmod xzio"

echo
cat << EOF
if loadfont unicode ; then
  insmod vbe
  insmod vga
EOF
# TODO: Better method of detecting Sortix GRUB and desirable video_bochs.
if [ x"$(uname)" = x"Sortix" ]; then
  echo "  insmod video_bochs"
fi
cat << EOF
  insmod gfxterm
fi
terminal_output gfxterm

set menu_title="Sortix $version for $machine"
set timeout=10
set default="0"

export menu_title
export timeout
export default
EOF

if [ -n "$ports" ]; then
  echo
  for port in $ports; do
    printf 'port_%s=true\n' "$(portvar "$port")"
  done
  for port in $ports; do
    printf 'tix_%s=false\n' "$(portvar "$port")"
  done
  echo
  for port in $ports; do
    printf 'export port_%s\n' "$(portvar "$port")"
  done
  for port in $ports; do
    printf 'export tix_%s\n' "$(portvar "$port")"
  done
fi

echo
cat << EOF
. /boot/grub/main.cfg
EOF

exec > boot/grub/main.cfg

printf "function load_base {\n"
case $platform in
x86_64-*)
  cat << EOF
  if ! cpuid -l; then
    echo "Error: You cannot run this 64-bit operating system because this" \
         "computer has no 64-bit mode."
    read
    exit
  fi
EOF
  ;;
esac
cat << EOF
  echo -n "Loading /$kernel ($(human_size $kernel)) ... "
  multiboot /$kernel "\$@"
  echo done
EOF
for initrd in $system_initrd $src_initrd $live_initrd $overlay_initrd; do
  cat << EOF
  echo -n "Loading /$initrd ($(human_size $initrd)) ... "
  module /$initrd
  echo done
EOF
done
printf "}\n"

echo
printf "function load_ports {\n"
if [ -z "$ports" ]; then
printf "\ttrue\n"
fi
for port in $ports; do
  tix=repository/$port.tix.tar.xz
  cat << EOF
  if \$tix_$(portvar "$port"); then
    echo -n "Loading /$tix ($(human_size $tix)) ... "
    module --nounzip /$tix --to /$tix
    echo done
  fi
  if \$port_$(portvar "$port"); then
    echo -n "Loading /$tix ($(human_size $tix)) ... "
    module /$tix --tix
    echo done
  fi
EOF
done
printf "}\n"

echo
cat << EOF
function load_sortix {
  load_base "\$@"
  load_ports
}
EOF

menuentry() {
  echo
  printf "menuentry \"Sortix (%s)\" {\n" "$1"
  if [ -n "$2" ]; then
    printf "\tload_sortix %s\n" "$2"
    #printf "\tload_sortix '"
    #printf '%s' "$2" | sed "s,','\\'',g"
    #printf "'\n"
  else
    printf "\tload_sortix\n"
  fi
  printf "}\n"
}

menuentry "live environment" ''
menuentry "new installation" '--init="/sbin/init --target=sysinstall"'
menuentry "upgrade existing installation" '--init="/sbin/init --target=sysupgrade"'

echo
cat << EOF
menuentry "Select ports..." {
	configfile /boot/grub/ports.cfg
}

menuentry "Advanced..." {
	configfile /boot/grub/advanced.cfg
}
EOF

exec > boot/grub/advanced.cfg

cat << EOF
menuentry "Back..." {
  menu=main
  configfile /boot/grub/main.cfg
}

menuentry "Select binary packages..." {
	configfile /boot/grub/tix.cfg
}
EOF

exec > boot/grub/ports.cfg

cat << EOF
menuentry "Back..." {
  menu=main
  configfile /boot/grub/main.cfg
}
EOF

echo
printf 'menuentry "Load all ports" {'
for port in $ports; do
  printf "  port_%s=true\n" "$(portvar "$port")"
done
printf '  configfile /boot/grub/ports.cfg\n'
printf '}\n'

for set in $sets; do
  echo
  set_content=$(eval echo \$set_$set)
  printf 'menuentry "Load only '"$set"' ports" {'
  for port in $ports; do
    printf "  port_%s=%s\n" "$(portvar "$port")" "$(isinset "$port" "$set_content")"
  done
  printf '  configfile /boot/grub/ports.cfg\n'
  printf '}\n'
done

echo
printf 'menuentry "Load no ports" {'
for port in $ports; do
  printf "  port_%s=false\n" "$(portvar "$port")"
done
printf '  configfile /boot/grub/ports.cfg\n'
printf '}\n'

echo
for port in $ports; do
  cat << EOF
if \$port_$(portvar "$port"); then
  menuentry "$port = true" {
    port_$(portvar "$port")=false
    configfile /boot/grub/ports.cfg
  }
else
  menuentry "$port = false" {
    port_$(portvar "$port")=true
    configfile /boot/grub/ports.cfg
  }
fi
EOF
done

exec > boot/grub/tix.cfg

cat << EOF
menuentry "Back..." {
  menu=main
  configfile /boot/grub/advanced.cfg
}
EOF

echo
printf 'menuentry "Load all binary packages" {'
for port in $ports; do
  printf "  tix_%s=true\n" "$(portvar "$port")"
done
printf '  configfile /boot/grub/tix.cfg\n'
printf '}\n'

for set in $sets; do
  echo
  set_content=$(eval echo \$set_$set)
  printf 'menuentry "Load only '"$set"' binary packages" {'
  for port in $ports; do
    printf "  tix_%s=%s\n" "$(portvar "$port")" "$(isinset "$port" "$set_content")"
  done
  printf '  configfile /boot/grub/tix.cfg\n'
  printf '}\n'
done

echo
printf 'menuentry "Load no binary packages" {'
for port in $ports; do
  printf "  tix_%s=false\n" "$(portvar "$port")"
done
printf '  configfile /boot/grub/tix.cfg\n'
printf '}\n'

echo
for port in $ports; do
  cat << EOF
if \$tix_$(portvar "$port"); then
  menuentry "$port = true" {
    tix_$(portvar "$port")=false
    configfile /boot/grub/tix.cfg
  }
else
  menuentry "$port = false" {
    tix_$(portvar "$port")=true
    configfile /boot/grub/tix.cfg
  }
fi
EOF
done
