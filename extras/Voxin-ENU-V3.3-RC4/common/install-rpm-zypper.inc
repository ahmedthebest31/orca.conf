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
    # sd_voxin not yet supplied by distro 
    return 1
}

getPackageVersion() {
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	local a
	a=$(zypper -q search -xis "$package")
	# e.g. speech-dispatcher on Leap 15.0:
	# echo $a
	# S | Name | Type | Version | Arch | Repository ---+-------------------+---------+-----------------+--------+----------------------- i+ | speech-dispatcher | package | 0.8.8-lp150.1.2 | x86_64 | openSUSE-Leap-15.0-Oss
	#
	# return: 0.8.8-lp150.1.2
	#
	[ $? != 0 ] && return
	echo "$a" | tail -n1 | sed 's/.*package | \([^ ]*\) .*/\1/'
}

isPackageInstalled() {
	[ $# != 1 ] || [ -z "$1" ] && return 1
	local package=$1
	local a
	a=$(zypper -q search -xi "$package")
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
	zypper -q --no-gpg-checks install -y "$package" &>> "$LOG"
}

installPackage() {
	[ $# != 1 ] || [ -z "$1" ] && return
	local package=$1
	zypper -q install -y "$package" &>> "$LOG"
}

uninstallPackage() {
    [ $# != 1 ] || [ -z "$1" ] && return 1
    local package=$1
    zypper remove -y "$package" &>> "$LOG"	
}
