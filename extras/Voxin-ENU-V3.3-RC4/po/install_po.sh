#!/bin/bash
# This file is under LGPL license
# 2007-2019, Gilles Casse <gcasse@oralux.org>
#
# install_po.sh : install the newly translated messages
#

lang="$1"
default=${LANG:0:2}
[ -z "$lang" ] && read -p "Please, enter your language code (default = $default): " lang
[ -z "$lang" ] && lang=$default
[ "$lang" = en ] && exit 0


# The directory locale/$lang/LC_MESSAGES is created if it does not exist

LOCALE="$(dirname $(pwd))/locale/$lang/LC_MESSAGES"

install -d $LOCALE

if [ -e "$lang.new.po" ]; then 
    mv $lang.def.po $lang.def.po.bak
    mv $lang.new.po $lang.def.po
fi

msgfmt $lang.def.po -o voxin.mo
mv voxin.mo $LOCALE

echo "...the file voxin.mo is installed in $LOCALE"   
