
case "$WITH_TTS" in
	viavoice)
		askLicense=askLicenseViavoice
		;;
	# nve)
	# 	;;
	*)
		askLicense=askLicenseNve
		;;
esac

askContinue() {
    local status
	_gettext "Your version of speech-dispatcher is too old (required version >= $SPEECHD_MIN_VERSION)"
	_gettext "If the installation continues, orca will not be able to use voxin, but emacspeak will"
	_gettext "Do you really want to continue?"
	_gettext "If yes, press the ENTER key."
	yes
	status=$?
    return $status
}

askInstallLang()
{
    local status
	local list=$(getVoiceTarball)
    if [ -z "list" ]; then
		status=1
    else
		_gettext "Do you want to install a new language?"
		_gettext "If yes, press the ENTER key."
		yes
		status=$?
    fi
    return $status
}

askUpdateConfAuthorization() {
    local status
	_gettext "The voxin module must be added to the configuration files"
	_gettext "Do you authorize the update of the configuration files (speechd.conf)?"
    _gettext "If you agree please type OK and press the ENTER key.";
    ok
    return $?
}

askUninstall()
{
    _gettext "Do you want to uninstall the TTS?"
    _gettext "If you agree please type OK and press the ENTER key.";
    ok
    return $?
}

askLicenseViavoice()
{
	local VIAVOICE_ARCHIVE=$(getVoiceAllTarball)
	local VIAVOICE_AGREEMENT=opt/oralux/voxin/share/doc/voxin-viavoice-all/AGREEMENT
	local VIAVOICE_LICENSE=opt/IBM/ibmtts/doc/IBM.txt
	local RFS_ARCHIVE=$(getRootfilesystemAllTarball)
	local LIBVOXIN_LGPL=opt/oralux/voxin/share/doc/libvoxin/LGPL.txt
	local LESS
	local CLEAR
	
    if [ "$TERM" = "dumb" ]; then
		LESS=cat
		CLEAR=
    else
		LESS="less -e"
		CLEAR=clear
    fi

	_gettext "Do you agree the licenses?"
    _gettext "If yes, press the ENTER key."    
    yes && return 0

    _gettext "1. Do you want to read again the End-User Agreement?"
    _gettext "If yes, press the ENTER key."    
    yes && {
		if [ "$TERM" != "dumb" ]; then
			_gettext "Please use the space bar to scroll through the document."
		fi
		
		case "$LANG" in
			fr*) VIAVOICE_AGREEMENT="${VIAVOICE_AGREEMENT}-fr";;
			*) ;;
		esac
		
		$CLEAR
		tar -Oxf "$VIAVOICE_ARCHIVE" "$VIAVOICE_AGREEMENT" 2>/dev/null || tar -Oxf "$VIAVOICE_ARCHIVE" ./"$VIAVOICE_AGREEMENT" | $LESS 
    }

    $CLEAR
    _gettext "If you agree to the End-User Agreement, press the ENTER key"
    yes || return 1

    $CLEAR
    _gettext "2. Do you want to read again the IBM License Agreement?"
    _gettext "If yes, press the ENTER key."    
    yes && {
		$CLEAR
		tar -Oxf "$VIAVOICE_ARCHIVE" "$VIAVOICE_LICENSE" 2>/dev/null || tar -Oxf "$VIAVOICE_ARCHIVE" ./"$VIAVOICE_LICENSE" | $LESS 
    }

    $CLEAR
    _gettext "If you agree to the IBM license agreement, press the ENTER key"
    yes || return 1

    $CLEAR
    _gettext "3. Do you want to read again the LGPL License?"
    _gettext "If yes, press the ENTER key."    
    yes && {
		$CLEAR
		tar -Oxf "$RFS_ARCHIVE" "$LIBVOXIN_LGPL" 2>/dev/null || tar -Oxf "$RFS_ARCHIVE" ./"$LIBVOXIN_LGPL" | $LESS 
    }	

    $CLEAR
    _gettext "If you agree to the LGPL license, press the ENTER key"
    yes || return 1
    
    $CLEAR
    return 0
}

askLicenseNve()
{
	local NVE_ARCHIVE=$(getVoxindTarball)
	local NVE_AGREEMENT=opt/oralux/nve/share/doc/voxind-nve/AGREEMENT
	local RFS_ARCHIVE=$(getRootfilesystemAllTarball)
	local LIBVOXIN_LGPL=opt/oralux/voxin/share/doc/libvoxin/LGPL.txt
	local LESS
	local CLEAR
	
    if [ "$TERM" = "dumb" ]; then
		LESS=cat
		CLEAR=
    else
		LESS="less -e"
		CLEAR=clear
    fi

	_gettext "Do you agree the licenses?"
    _gettext "If yes, press the ENTER key."    
    yes && return 0

    _gettext "1. Do you want to read again the End-User Agreement?"
    _gettext "If yes, press the ENTER key."    
    yes && {
		if [ "$TERM" != "dumb" ]; then
			_gettext "Please use the space bar to scroll through the document."
		fi
				
		$CLEAR
		tar -Oxf "$NVE_ARCHIVE" "$NVE_AGREEMENT" 2>/dev/null || tar -Oxf "$NVE_ARCHIVE" ./"$NVE_AGREEMENT" | $LESS 
    }

    $CLEAR
    _gettext "If you agree to the End-User Agreement, press the ENTER key"
    yes || return 1

    $CLEAR
    _gettext "2. Do you want to read again the LGPL License?"
    _gettext "If yes, press the ENTER key."    
    yes && {
		$CLEAR
		tar -Oxf "$RFS_ARCHIVE" "$LIBVOXIN_LGPL" 2>/dev/null || tar -Oxf "$RFS_ARCHIVE" ./"$LIBVOXIN_LGPL" | $LESS 
    }	

    $CLEAR
    _gettext "If you agree to the LGPL license, press the ENTER key"
    yes || return 1
    
    $CLEAR
    return 0
}

# init
