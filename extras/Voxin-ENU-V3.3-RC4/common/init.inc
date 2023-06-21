LOG="$BASE/log/voxin.log"
TEMPLATE=voxin-installer.XXXXXXXXXX
TMPDIR=/tmp
# Only version > 0.7 are compatible
SPEECHD_MIN_VERSION=0.7
VOXIN_VERSION=3.3rc4

unset WITH_TTS
ls $BASE/packages/all/voxin-viavoice-all* &> /dev/null && WITH_TTS=viavoice
ls $BASE/packages/all/voxin-nve-all* &> /dev/null && WITH_TTS=nve
case "$WITH_TTS" in
	viavoice) postInstTarball=postInstViavoiceTarball;;
	*) postInstTarball=noOp;;
esac




rm -f "$LOG"

unset outputArray
declare -a outputArray

leave() {
	echo -e "$1"
	exit $2
}

init_gettext() {
    export TEXTDOMAINDIR="$1"
    export TEXTDOMAIN=voxin
}

yes() {
    local a
    read a
    [ -z "$a" ]
    return $?
}

ok() {
    local status=1
    local a
	echo -n " "
    read a
    case "$a" in [oO][kK]*) status=0;; esac
	if [ "$status" = "0" ]; then		
		_gettext "Please wait..."
	else
		_gettext "You do not authorize the installer to perform this operation"
	fi	
    return $status
}

usage () {
    cat >&2 <<END
Usage: voxin-installer.sh [options]
Options:
     -h, --help       print this message
     -l, --lang       install the language
     -u, --uninstall  uninstall the TTS
     -v, --verbose    verbose
END
}

uninstall() {
    [ $# != 1 ] || [ -z "$1" ] && return 1
    local rfsdir="$1"
    spd_conf_unset voxin	
    uninstallSystem "$rfsdir"
    _gettext "Operation completed. "
    return 0
}

check_distro() {
    local status=0
    local ID=$(awk -F= '/^ID_LIKE=/{print $2}' /etc/os-release | tr -d '"')
    [ -z "$ID" ] && ID=$(awk -F= '/^ID=/{print $2}' /etc/os-release | tr -d '"')
    [ -z "$ID" ] && return 1
    
    status=0
    case "$ID" in
	arch*) source common/install-arch-linux.inc;;
	debian*|ubuntu*) source common/install-deb.inc;;
	fedora*) source common/install-rpm-dnf.inc;;
	gentoo*) source common/install-gentoo.inc;;
	opensuse*|suse*) source common/install-rpm-zypper.inc;;
	rhel*) source common/install-rpm-yum.inc;;
	slackware*|slint*) source common/install-slackware.inc;;
	*) status=1;;
    esac
    
    return $status
}

# if speech-dispatcher is already installed, check if this installer
# provides the compatible voxin output module
check_speech_dispatcher_voxin() {
	local installedVersion=$(getPackageVersion speech-dispatcher)
	isVersionGreater "$installedVersion" "$SPEECHD_MIN_VERSION"
}

# getTarballVersion
# Example:
# input=name_1.2.3.x86_64.txz
# output=1.2.3
getTarballVersion() {
	[ $# != 1 ] || [ -z "$1" ] && return
	echo "$1" | sed 's+.*_\(.*\)\..*\..*+\1+'
}

# getMajMinVersion
# Example:
# input=1.2.3-4.fc29
# output=1.2
getMajMinVersion() {
	[ $# != 1 ] || [ -z "$1" ] && return
	echo "$1" | cut -f1,2 -d.
}

getRootfilesystemAllTarball() {
    local tarball=$(find packages/all -name "rfs*.all.txz")

    if [ -z "$tarball" ]; then
	_gettext "rfs all tarball not found!" >> "$LOG"
	return 1
    fi
    echo "$tarball"
    return 0
}

getRootfilesystemArchTarball() {
    local tarball=$(find packages/all -name "rfs*.$ARCH.txz")

    if [ -z "$tarball" ]; then
	_gettext "rfs all tarball not found!" >> "$LOG"
	return 1
    fi
    echo "$tarball"
    return 0
}

getPrevSpeechdModulesTarball() {
    local tarball=$(find packages/all -name "sd_voxin_*.$ARCH.txz")

    if [ -z "$tarball" ]; then
#	_gettext "prev speechd modules tarball not found!" >> "$LOG"
	return 1
    fi
    echo "$tarball"
    return 0
}

noOp() {
	return 0
}

getVoiceAllTarball() {
	find packages/all -name "voxin-*-all_*.txz"
}

getVoiceTarball() {
	find packages/all -name "voxin-*txz" ! -name "voxin-*-all_*.txz"
}

getVoxindTarball() {
	find packages/all -name "voxind-*.$ARCH.txz"
}

postInstViavoiceTarball() {
	[ $# != 2 ] && return 1
	[ -z "$1" ] || [ -z "$2" ] && return 1

	local rfsdir="$1"
	local destdir="$2"
	local inidir="$rfsdir/opt/IBM/ibmtts/etc"
	local newconf="$rfsdir/var/opt/IBM/ibmtts/cfg/eci.ini"
	local rfs32="$rfsdir/opt/oralux/voxin/rfs32"
	local LANG
	local i

	if [ ! -f "$inidir/all.ini" ]; then
		_gettext "Notice: no $inidir/all.ini"
		return 0
	fi

	# get list of installed languages
	LANG=$(find "$inidir/../lib" -regex ".*/...50.so" | sed "s+.*/\(.*\)50.so+\1+")

	if [ -z "$LANG" ]; then
		_gettext "No language found in $inidir/../lib"
		return 1
	fi

	cp "$inidir/all.ini" "$newconf" 
	if [ $? != 0 ]; then
		_gettext "Write error: $inidir/all.ini"
		return 1
	fi

	for i in $LANG; do
		if [ -e "$inidir/$i.ini" ]; then
			cat "$inidir/$i.ini" >> "$newconf"
			if [ $? != 0 ]; then
				_gettext "Write error: $inidir/all.ini"
				return 1
			fi
		else
			_gettext "Notice: no $inidir/$i.ini"		
			return 1
		fi
	done

	sed -i "s#=/opt/#=$destdir/opt/#" "$newconf"
	if [ $? != 0 ]; then
		_gettext "Write error: $newconf"
		return 1
	fi

	return 0
}

installLang() {
	echo installLang >> "$LOG"
	[ $# != 1 ] || [ -z "$1" ] && return 1
	local rfsdir="$1"
	local status
	local i

    "$askLicense" || return 1

	i=$(getVoiceAllTarball)
	if [ -z "$i" ]; then
		_gettext "Error: no 'all' tarball" >> "$LOG"
		return 1
	fi
	tar -C "$rfsdir" --no-overwrite-dir -xf "$i"
	status=$?
	if [ "$status" != 0 ]; then
		_gettext "Error: untar failure ($i)" >> "$LOG"
		return $status
	fi

	if [ "$WITH_TTS" = nve ]; then
		i=$(getVoxindTarball)
		if [ -z "$i" ]; then
			_gettext "Error: no 'voxind' tarball" >> "$LOG"
			return 1
		fi
		tar -C "$rfsdir" --no-overwrite-dir -xf "$i"
		status=$?
		if [ "$status" != 0 ]; then
			_gettext "Error: untar failure ($i)" >> "$LOG"
			return $status
		fi
	fi
	
	getVoiceTarball | while read i; do
		tar -C "$rfsdir" --no-overwrite-dir -xf "$i"
		status=$?
		if [ "$status" != 0 ]; then
			_gettext "Error: untar failure ($i)" >> "$LOG"
			return $status
		fi
	done

	"$postInstTarball" "$rfsdir" "/"
	
    return 0
}

getArch() {
    ARCH=$(uname -m)
    PACMAN_ARCH=$ARCH
    case "$ARCH" in
	x86_64|ia64)
	    DEBIAN_ARCH=amd64
    	    ;;
	arm*)
	    DEBIAN_ARCH=armhf
	    PACMAN_ARCH=armv7h
	    ;;
	aarch64)	
	    DEBIAN_ARCH=arm64
	    ;;
	*)
	    DEBIAN_ARCH=i386
    	    ;;
    esac
}

uninstallOldVoxin()
{	
    echo uninstallOldVoxin >> "$LOG"
    [ $# != 1 ] || [ -z "$1" ] && return 1
    local rfsdir="$1"
    
    local DIR="$rfsdir/opt/oralux/voxin"
    if [ -e "$DIR" ]; then
	rm -rf "$DIR.old"
	[ -e "$DIR.old" ] && { echo "error: DIR.old=$DIR.old" >> "$LOG"; return 1; }
	mv "$DIR" "$DIR.old" || { echo "error: DIR=$DIR" >> "$LOG"; return 1; }
    fi
    isPackageInstalled voxin && { uninstallPackage voxin || return 1; }
    return 0
}

installOldVoxin()
{	
    echo installOldVoxin >> "$LOG"
    [ $# != 1 ] || [ -z "$1" ] && return 1
    local rfsdir="$1"
    isPackageInstalled voxin || { installPackage voxin || return 1; }
    local DIR_NEW="$rfsdir/opt/oralux/voxin"
    local DIR_OLD="$DIR_NEW.old"
    if [ -e "$DIR_OLD" ]; then
	rm -rf "$DIR_NEW"
	[ -e "$DIR_NEW" ] && { echo "error: DIR_NEW=$DIR_NEW" >> "$LOG"; return 1; }
	mv "$DIR_OLD" "$DIR_NEW"
    fi
    return 0
}

uninstallSystem()
{	
	echo uninstallSystem >> "$LOG"
	[ $# != 1 ] || [ -z "$1" ] && return 1
	local rfsdir="$1"
	isPackageInstalled voxin && { uninstallPackage voxin || return 1; }
	isPackageInstalled voxin-speechd && { uninstallPackage voxin-speechd || return 1; }
	isPackageInstalled voxin-speechd-conf && { uninstallPackage voxin-speechd-conf || return 1; }
	for i in "$rfsdir/opt/oralux/voxin" "$rfsdir/opt/oralux/voxin.old" "$rfsdir/var/opt/oralux/voxin" "$rfsdir/opt/IBM/ibmtts" "$rfsdir/var/opt/IBM/ibmtts" "$rfsdir/opt/oralux/nve" ; do
		rm -rf $i
	done
	for i in "$rfsdir/opt/oralux" "$rfsdir/var/opt/oralux" "$rfsdir/opt/IBM" "$rfsdir/var/opt/IBM"; do
		[ -e "$i" ] && [ -z "$(ls -A "$i")" ] && rm -rf "$i"
	done
	return 0
}

is_voxin_conf_installed() {
    ls -lL /etc/speech-dispatcher/modules/voxin.conf &> /dev/null
}

installSystem() {	
	echo installSystem >> "$LOG"
	[ $# != 1 ] || [ -z "$1" ] && return 1
	local rfsdir="$1"
	local status=0
	local voxinPackage=$(getVoxinPackage)
	local voxinSpeechdPackage=$(getVoxinSpeechdPackage)
	local voxinSpeechdConfPackage=$(getVoxinSpeechdConfPackage)
	local rfsAllTarball=$(getRootfilesystemAllTarball) || return 1
	local rfsArchTarball=$(getRootfilesystemArchTarball) || return 1
	local prevSpeechdModulesTarball=$(getPrevSpeechdModulesTarball) || unset prevSpeechdModulesTarball
	local voxin_conf_installed=0
	local sd_voxin_installed=0
	
	if [ ! -d "$rfsdir" ]; then
		_gettext "Install directory not found: $rfsdir" >> "$LOG"
		return 1
	fi
	
	if [ -z "$voxinPackage" ]; then
		_gettext "voxin package not found!" >> "$LOG"
		return 1
	fi
	
	if [ -z "$voxinSpeechdPackage" ]; then
		_gettext "voxin-speechd package not found!" >> "$LOG"
		return 1
	fi

	if [ -z "$voxinSpeechdConfPackage" ]; then
		_gettext "voxin-speechd-conf package not found!" >> "$LOG"
		return 1
	fi

	for i in voxin voxin-speechd voxin-speechd-conf; do
	    isPackageInstalled $i && { uninstallPackage $i || return 1; }
	done

	is_sd_voxin_installed && sd_voxin_installed=1
	is_voxin_conf_installed && voxin_conf_installed=1
	    
	local i
	for i in $rfsAllTarball $rfsArchTarball $prevSpeechdModulesTarball; do
	    [ -z "$i" ] && continue
	    tar -C "$rfsdir" -xf "$i" 
	    status=$?
	    if [ "$status" != 0 ]; then
		_gettext "Error: untar failure ($i)" >> "$LOG"
		return "$status"
	    fi	
	done

	# install voxin.ini globally
	local ref_voxin_ini=/opt/oralux/voxin/share/conf/voxin.ini
	local global_voxin_ini=/var/opt/oralux/voxin/voxin.ini	
	[ ! -e $global_voxin_ini ] && install -m 644 -D $ref_voxin_ini $global_voxin_ini

	# install voxin package
	installLocalPackage "$voxinPackage"
	status=$?
	[ "$status" != 0 ] && { _gettext "Error: package install failure ($voxinPackage)!" >> "$LOG"; return '$status'; }	

	# install voxin.conf (if not yet installed)
	if [ "$voxin_conf_installed" = 0 ]; then
	    local conf_dir=/opt/oralux/voxin/share/speech-dispatcher
	    local conf="$conf_dir/last/voxin.conf"
	    local ver=$(/usr/bin/speech-dispatcher -v | head -1 | cut -f2 -d" ")	    
	    [ -n "$ver" ] && [ -e "$conf_dir/$ver/voxin.conf" ] && conf="$conf_dir/$ver/voxin.conf"
	    spd_conf_install_voxin_conf "$conf"

	    # install voxin-speechd-conf package
	    installLocalPackage "$voxinSpeechdConfPackage"
	fi

	# install sd_voxin (if not yet provided by the distro)
	if [ "$sd_voxin_installed" = 0 ]; then
	    # install voxin-speechd package
	    installLocalPackage "$voxinSpeechdPackage"
	fi

	return "$status"
}

install_gettext() {
    installPackage gettext
    . gettext.sh
}

# getMajorMinorMicro
# The result is supplied in the outputArray (index 0:major, 1:minor, 2:micro)
#
# major.minor.micro
# major: n digits
# minor: n digits
# micro: n digits (optionally followed by non digits characters which will be filtered)
# e.g.
# 0.7.1_abce
# gives major=0, minor=7, micro=1
#
# Return 0 if ok, 1 otherwise
getMajorMinorMicro() {
	[ $# != 1 ] && return 1
	local version=$1
	local major
	local minor
	local micro
	
	major=$(echo "$version" | sed -r "s/([0-9]+)\.[0-9]+.*/\1/")	
	[ "$major" = "$version" ] && return 1

	minor=$(echo "$version" | sed -r "s/[0-9]+\.([0-9]+).*/\1/")	
	[ "$minor" = "$version" ] && return 1
	
	micro=$(echo "$version" | sed -r "s/[0-9]+\.[0-9]+\.([0-9]+).*/\1/")
	[ "$micro" = "$version" ] && micro=0

	outputArray[0]=$major
	outputArray[1]=$minor
	outputArray[2]=$micro
	
	return 0
}

# compare version1 to version2
#
# RETURN
# 0: ver1 >= ver2
# 1: ver1 < ver2
# 2: ver1 with unexpected format (see getMajorMinorMicro)
# 3: ver2 with unexpected format
isVersionGreater() {
	[ $# != 2 ] && return 1
	local ver1=$1
	local ver2=$2

	getMajorMinorMicro "$ver1" || return 2
	ver1=("${outputArray[@]}") 

	getMajorMinorMicro "$ver2"
	ver2=("${outputArray[@]}") || return 3

	i=0
	while [ "$i" -le 2 ]; do
		[ "${ver1[$i]}" -gt "${ver2[$i]}" ] && return 0
		[ "${ver1[$i]}" -lt "${ver2[$i]}" ] && return 1
	i=$((i+1))	
	done
	return 0
}

