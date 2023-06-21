#!/bin/bash

process_file() {
    local file=$1
    case "${file##*\/}" in
	voxin-update*) ;;
	voxin-???-*z) [ -n "$VV_TARBALL" ] && ./update_tarball.sh $VV_TARBALL "$file" $DIR;;
	voxin-*z) [ -n "$VE_TARBALL" ] && ./update_tarball.sh $VE_TARBALL "$file" $DIR;;
	*);;
    esac
}

leave() {
	echo -e "$1" && exit $2
}

usage() {
    echo "
Usage: update_tarballs.sh VOXIN_UPDATE DESTINATION_DIR

Update each older archive from stdin using a new update.
Store the resulting tarball into the destination directory.

Example
# Update to 3.2 each Viavoice based archive in list:
cat list | update_tarballs.sh voxin-update-3.2.tgz build_dir/

# Update to 3.2 each Vocalizer Embedded based archive in list:
cat list | update_tarballs.sh voxin-update-ve-3.2.tgz build_dir/
"
    exit 1
}

[ $# = 2 ] || usage
[ -f "$1" ] || leave "$1 is not a file" 1 
[ -d "$2" ] || leave "$3 is not a directory" 1

DIR=$2

unset VE_TARBALL VV_TARBALL
case "${1##*\/}" in
    voxin-update-ve-*z) VE_TARBALL=$1;;
    voxin-update-*z) VV_TARBALL=$1;;
    *) usage && leave "voxin-update missing" 1;;					 
esac

while read file; do process_file "$file"; done
