#!/bin/bash -x
# This file is under LGPL license
# 2007-2019, Gilles Casse <gcasse@oralux.org>
#
# new_po.sh : create or update xx.new.po, (xx = fr,...) the file which 
# includes the translation for the xx language.
#

VOXIN=".."

lang="$1"
default=${LANG:0:2}
[ -z "$lang" ] && read -p "Please, enter your language code (default = $default): " lang
[ -z "$lang" ] && lang=$default
[ "$lang" = en ] && exit 0

if [ -s "$lang.new.po" ]; then
  echo "The $lang.new.po file already exists.";
  read -p "Do you really want to remove it ? (y/N): " q
  case $q in
    y|Y) ;;
    *) echo "ok, we stop here.";exit 1;;
  esac
fi

touch $lang.def.po $lang.new.po voxin.pot
chmod +w $lang.def.po $lang.new.po voxin.pot

echo "Building the new $lang.new.po..."   

#xgettext --add-location -s `find  $VOXIN -maxdepth 2 -name "*sh" -name "*inc"` -o voxin.pot
xgettext --language=Shell --keyword --keyword=_gettext --add-location -s `find  $VOXIN -maxdepth 2 -name '*sh' ! -path '*/po/*' -o -name '*.inc' ` -o voxin.pot

msgmerge $lang.def.po voxin.pot -o $lang.new.po

echo "...the file $lang.new.po is ready"   
