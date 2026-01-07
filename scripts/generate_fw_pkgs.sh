#!/usr/bin/env bash
#  SPDX-License-Identifier: LGPL-2.1-or-later

if [ -z "${1}" ]; then
  echo "
>>> The version is mandatory
"
  exit 1
fi

VERSION=${1}
[ -z $BRANCH ] && BRANCH="testingd"

if [ ! -f WHENCE ];then
  echo "
>>> WHENCE file not found !!!
>>> Are you at the root of linux-firmware source tree ?
"
  exit 1
fi

if [ -z $HOME ]; then
  echo "
>>> \$HOME not set !!!
>>> Cannot help you
"
  exit 1

fi

ROOT=$HOME/$BRANCH/base

cp WHENCE WHENCE.in

sed -i "s@bnx2x:@bnx2x -@" WHENCE.in
sed -i "s@Driver:Atheros@Driver: Atheros@" WHENCE.in
sed -i "s/^Driver /Driver:/" WHENCE.in

rm -f List List.sort List.uniq
rm -r $ROOT/*-firmware

grep -E '^Driver:' WHENCE.in|while read D N l;do echo "$D $N " >>List;done
cat List |sort>List.sort
cat List.sort|uniq> List.uniq

cat List.uniq|while read N
do
  FILE=${N/Driver: /}
  DESCRIPTION=$FILE
  FILE=${FILE,,}
  DIR="$ROOT/${FILE//\//-}-firmware"

  mkdir -p $DIR

  sed -n "/${N//\//\\/}/,/^-----/p" WHENCE.in >"$DIR/WHENCE"
  sed -i "/${N//\//\\/}/,/^-----/d" WHENCE.in
  DESCRIPTION="$(echo $(head -1 $DIR/WHENCE)|sed s/^Driver:\ //)"
  [ -z "$DESCRIPTION" ] && DESCRIPTION="$FILE firmware for Linux"
  [ "$DESCRIPTION" == "$FILE" ] && DESCRIPTION="$FILE firmware for Linux"

  echo "description=\"$DESCRIPTION\"
url='https://git.kernel.org/?p=linux/kernel/git/firmware/linux-firmware.git;a=summary'

packager=\"tnut <tnut@nutyx.org>\"
contributors=\"Spiky\"

license=\"Custom\"

set=(linux-firmware)

name=${FILE//\//-}-firmware
version=$VERSION

makedepends=(rdfind)" > $DIR/Pkgfile

case ${FILE//\//-} in
  "rt2860sta"|"rt2870sta")
    echo "run=(rt2800usb-firmware)" >> $DIR/Pkgfile ;;

  "rtl8192ee"|"rtl8723bs")
    echo "run=(rtl8xxxu-firmware)" >> $DIR/Pkgfile ;;

esac

echo "source=(WHENCE
        https://gitlab.com/kernel-firmware/linux-firmware/-/archive/\$version/linux-firmware-\$version.tar.bz2)

build() {

cd linux-firmware-\$version

# delete this python check as long we don't know what's missing here
sed -i "/check_whence.py/d" copy-firmware.sh

# ignore broken links as it's irrelevant when using DESTDIR
sed 's@err \"Broken@warn \"Brocken@' copy-firmware.sh

cp ../WHENCE .
make DESTDIR=\$PKG FIRMWAREDIR=/lib/firmware NUM_JOBS=1 install

install -Dt \$PKG/usr/share/licenses/\$name -m644 WHENCE" >> $DIR/Pkgfile

for LICENSE in $(grep LICEN $DIR/WHENCE|sed -e 's/^.*See //;s/ .*//')
do
 [ -f "$LICENSE" ] && \
 echo "install -Dt \$PKG/usr/share/licenses/\$name -m644\
 $LICENSE" >> $DIR/Pkgfile
done

echo "}" >> $DIR/Pkgfile

done
rm -f List List.sort List.uniq WHENCE.in
