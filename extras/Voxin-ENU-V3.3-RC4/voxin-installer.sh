#!/bin/bash
# This file is under the LGPL license
# 2007-2020, Gilles Casse <gcasse@oralux.org>
#

BASE=$(dirname $(readlink -f $0))
cd $BASE

source ./common/init.inc
source ./common/spdconf.inc

_gettext() {
    if [ -n "$GETTEXT" ]; then
		echo; gettext "$1"
    else
		echo "$1"
    fi
}

display_error_msg() {
    _gettext "Error: more details in $LOG"
    _gettext "For support, email to contact at oralux.org "
}

getArch

unset GETTEXT
which gettext >> "$LOG"
if [ "$?" = "0" ]; then
    init_gettext "$BASE"/locale
    GETTEXT=0
fi

[ "$UID" != "0" ] && _gettext "Please run voxin-installer as root. " && exit 0

check_distro
if [ "$?" != "0" ]; then
	_gettext "Sorry, this distribution is not yet supported. "
	_gettext "For support, email to contact at oralux.org "
    exit 1
fi

if [ -h /opt ]; then
	_gettext "Sorry, this installer does not expect a symbolic link /opt"
	_gettext "For support, email to contact at oralux.org "
    exit 1
fi

TEMP=`getopt -o hluv --long help,lang,uninstall,verbose -- "$@"`
if [ $? != 0 ] ; then
    usage
    exit 1
fi

eval set -- "$TEMP"

with_silent=0
with_lang=0
with_uninstall=0
with_verbose=0

while true ; do
    case "$1" in
		-l|--lang) with_silent=1; with_lang=1; shift;;
		-u|--uninstall) with_uninstall=1; shift;;
		-v|--verbose) with_verbose=1; shift;;
		--) shift ; break;;
		*) usage; exit 1;;
    esac
done

if [ "$with_uninstall" = 0 ]; then
	with_sd=1
	check_speech_dispatcher_voxin
	case $? in
		1)
			_gettext "Your version of speech-dispatcher is too old (required version >= $SPEECHD_MIN_VERSION)"
			_gettext "If the installation continues, orca will not be able to use voxin, but emacspeak will"
			_gettext "Do you really want to continue?"
			_gettext "If yes, press the ENTER key."
			yes
			if [ $? = 1 ]; then
				_gettext "Good Bye. "
				exit 0
			fi
			with_sd=0
			;;
		2)
			_gettext "Your version of speech-dispatcher has not been recognized."
			_gettext "If the installation continues, voxin will use its module for speech-dispatcher $SPEECHD_MAX_VERSION"
			_gettext "Do you really want to continue?"
			_gettext "If yes, press the ENTER key."
			yes
			if [ $? = 1 ]; then
				_gettext "Good Bye. "
				exit 0
			fi
			with_sd=0
	esac
fi

if [ "$with_verbose" = "1" ]; then
    set -x
fi

if [ "$with_silent" = "1" ]; then
    source common/askSilent.inc
else
    source common/ask.inc
fi

if [ -z "$GETTEXT" ]; then
    install_gettext
    init_gettext "$(pwd)/locale"
    GETTEXT=0
fi

installDir=/

if [ "$with_uninstall" = "1" ]; then
    askUninstall && uninstall "$installDir"
    exit 0
fi

_gettext "Logs written in log/voxin.log"
_gettext "Initialization; please wait... "

# remove packages from voxin < 2.0
#for i in speech-dispatcher-ibmtts speech-dispatcher-voxin voxind libvoxin libvoxin1; do
for i in voxind libvoxin libvoxin1; do
	isPackageInstalled $i && uninstallPackage $i
done

uninstallOldVoxin "$installDir" || { installOldVoxin "$installDir"; display_error_msg; exit 1; }
installSystem "$installDir" || { installOldVoxin "$installDir"; display_error_msg; exit 1; }

installed=0
askInstallLang && {
    installLang "$installDir" || { display_error_msg; exit 1; }
    installed=1

    unset PLAY OPT
    PLAY=$(which aplay 2>/dev/null)
    if [ -z "$PLAY" ]; then
		PLAY=$(which paplay 2>/dev/null)
    fi

    export LD_LIBRARY_PATH="$installDir"/opt/oralux/voxin/lib
    if [ -n "$PLAY" ]; then
		"$installDir"/opt/oralux/voxin/bin/voxin-say "Voxin: OK" | "$PLAY" &>> "$LOG"
    fi
}

[ "$with_sd" = 1 ] && spd_conf_is_update_required && askUpdateConfAuthorization && spd_conf_set voxin

if [ "$installed" = "0" ]; then
    askUninstall && {
		uninstall "$installDir" || { display_error_msg; exit 1; }
		exit 0
    }
fi

_gettext "The changes will be taken into account on next boot. "
_gettext "Good Bye. "
