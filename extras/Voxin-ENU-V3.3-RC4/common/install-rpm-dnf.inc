getVoxinPackage() {
	[ -z "$ARCH" ] && return 1
	find packages/rpm -name "voxin-[0-9]*.$ARCH.rpm"
}

getVoxinSpeechdPackage() {
	[ -z "$ARCH" ] && return 1
	find packages/rpm -name "voxin-speechd-[0-9]*$ARCH.rpm"
}

getVoxinSpeechdConfPackage() {
	[ -z "$ARCH" ] && return 1
	find packages/rpm -name "voxin-speechd-conf-[0-9]*$ARCH.rpm"
}

is_sd_voxin_installed() {
    #TODO
    #    { dpkg -L speech-dispatcher-voxin | grep sd_voxin || { dpkg -L speech-dispatcher | grep sd_voxin; }; } &> /dev/null
    return 1
}

getPackageVersion() {
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	dnf list installed "$package" 2>/dev/null | awk "/$package/{print \$2}"	
}

isPackageInstalled() {
	[ $# != 1 ] || [ -z "$1" ] && return 1
	local package=$1
	dnf list installed "$package" &>/dev/null
}

# getPackageFileVersion
# Example:
# input: name1-name2-1.2.3-1.x86_64.rpm
# output: 1.2
getPackageFileVersion() {
	unset output
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	output=$(echo "$package" | sed -E -e "s/.*-([[:digit:]]+.[[:digit:]]+).*/\1/")
}

installLocalPackage() {
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	dnf install -q -y "$package" &>> "$LOG"
}

installPackage() {
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	dnf install -q -y "$package" &>> "$LOG"
}

uninstallPackage() {
    [ $# != 1 ] || [ -z "$1" ] && return 1
    local package=$1
    dnf remove -y "$package" &>> "$LOG"	
}
