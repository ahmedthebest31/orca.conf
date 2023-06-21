getVoxinPackage() {
	[ -z "$PACMAN_ARCH" ] && return 1
	find packages/archlinux -name "voxin-[0-9]*$PACMAN_ARCH.pkg.tar.xz"
}

getVoxinSpeechdPackage() {
	[ -z "$PACMAN_ARCH" ] && return 1
	find packages/archlinux -name "voxin-speechd-[0-9]*$PACMAN_ARCH.pkg.tar.xz"
}

getVoxinSpeechdConfPackage() {
	[ -z "$PACMAN_ARCH" ] && return 1
	find packages/archlinux -name "voxin-speechd-conf-[0-9]*$PACMAN_ARCH.pkg.tar.xz"
}

is_sd_voxin_installed() {
    pacman -Ql speech-dispatcher | grep sd_voxin > /dev/null
}

getPackageVersion() {
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	pacman -Q "$package" 2>/dev/null | awk '{print $2}'
}

isPackageInstalled() {
	[ $# != 1 ] || [ -z "$1" ] && return 1
	local package=$1
	pacman -Q "$package" &>/dev/null
}

# getPackageFileVersion
# Example:
# input: name1-name2-1.2.3-1-x86_64.pkg.tar.xz
# output: 1.2
getPackageFileVersion() {
	unset output
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	output=$(echo "$package" | sed -E -e "s/-$PACMAN_ARCH.pkg.tar.xz//" -e "s/.*-([[:digit:]]+.[[:digit:]]+).*/\1/")
}

installLocalPackage() {
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	pacman -U --noconfirm "$package" &>>"$LOG"
}

installPackage() {
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
    pacman -S --noconfirm "$package" &>>"$LOG"
}

uninstallPackage() {
    [ $# != 1 ] || [ -z "$1" ] && return 1
    local package=$1
    pacman --noconfirm -R "$package" &>>"$LOG"
}

