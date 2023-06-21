#!/bin/bash -x

leave() {
	echo -e "$1" && exit $2
}

usage() {
    echo "
Usage: update_tarball.sh VOXIN_UPDATE OLD_TARBALL DESTINATION_DIR

Update an older voxin tarball using a new update.
Store the resulting tarball into the destination directory.

Example
# Update to 3.2 the 'English US' archive:
update_tarball.sh voxin-update-3.2.tgz voxin-enu-3.1.tgz build_dir/

# Update to 3.2 the 'Tom Compact' voice 
update_tarball.sh voxin-update-ve-3.2.tgz voxin-american-english-tom-compact-3.1.tgz build_dir/
"
    exit 1
}

[ $# = 3 ] || usage
[ -f "$1" ] || leave "$1 is not a file" 1 
[ -f "$2" ] || leave "$2 is not a file" 1
[ -d "$3" ] || leave "$3 is not a directory" 1

UPDATE_NEW=$(realpath $1)
VOICE_OLD=$(realpath $2)
BUILD=$(mktemp -p $3 -d)

if [ "$3" = "." ]; then                                                                                                                           
    BUILD=$(mktemp -p $3 -d)                                                                                                                       
else                                                                                                                                             
    BUILD=$3                                                                                                                                       
fi                                                                                                                                               



is_ve() {
    local a=$(echo "$1" | sed -E "s/.*voxin-update(-ve-).*tgz/\1/")
    if [ "$a" = "-ve-" ]; then
	NEW_VER=$(echo "$1" | sed -E "s/.*voxin-update-ve-(.*).tgz/\1/")
    fi
    [ -n "$NEW_VER" ]
}

is_vv() {
    local a=$(echo "$1" | sed -E "s/.*(voxin-update-)[0-9]+.*tgz/\1/")
    if [ "$a" = "voxin-update-" ]; then
	NEW_VER=$(echo "$1" | sed -E "s/.*voxin-update-(.*).tgz/\1/")
    fi
    [ -n "$NEW_VER" ]
}

lang_ve() {
    LANG_TARBALL=$(echo "$1" | sed -E "s/.*voxin-(.*)-[0-9].*.tgz/\1/")
    [ "$LANG_TARBALL" = "$1" ] && return 1
    OLD_VER=$(echo "$1" | sed -E "s/.*voxin-.*-([0-9].*).tgz/\1/")

    cd ${BUILD}
    tar -xf ${UPDATE_NEW} || return 1   
    tar -xf ${VOICE_OLD} || return 1   

    mv voxin-${NEW_VER}/voxin-update-ve-${NEW_VER} voxin-${NEW_VER}/voxin-${LANG_TARBALL}-${NEW_VER} || exit 1
    mv voxin-${OLD_VER}/voxin-${LANG_TARBALL}-${OLD_VER}/packages/all/voxin-ve-*.txz voxin-${NEW_VER}/voxin-${LANG_TARBALL}-${NEW_VER}/packages/all/ || exit 1

    return 0
}

lang_vv() {
    LANG_TARBALL=$(echo "$1" | sed -E "s/.*voxin-(.*)-[0-9].*.tgz/\1/")
    [ "$LANG_TARBALL" = "$1" ] && return 1
    OLD_VER=$(echo "$1" | sed -E "s/.*voxin-.*-([0-9].*).tgz/\1/")

    cd ${BUILD}
    tar -xf ${UPDATE_NEW} || exit 1   
    tar -xf ${VOICE_OLD} || exit 1   

    mv voxin-${NEW_VER}/voxin-update-${NEW_VER} voxin-${NEW_VER}/voxin-${LANG_TARBALL}-${NEW_VER} || exit 1
    mv voxin-${OLD_VER}/voxin-${LANG_TARBALL}-${OLD_VER}/packages/all/voxin-viavoice-*.txz voxin-${NEW_VER}/voxin-${LANG_TARBALL}-${NEW_VER}/packages/all/ || exit 1
    
    return 0
}

unset OLD_VER NEW_VER LANG_TARBALL 

is_ve "$1"
if [ "$?" = 0 ]; then
    lang_ve "$VOICE_OLD" || usage
else
    is_vv "$1" || usage
    lang_vv "$VOICE_OLD" || usage
fi

fakeroot bash -c "tar -cf voxin-${LANG_TARBALL}-${NEW_VER}.tgz voxin-${NEW_VER}"
rm -rf voxin-${NEW_VER}
rm -rf voxin-${OLD_VER}
echo $PWD/voxin-${LANG_TARBALL}-${NEW_VER}.tgz
    
