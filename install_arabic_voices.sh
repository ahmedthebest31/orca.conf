#!/bin/bash

set -e

# Function to check if a package is installed
is_package_installed() {
    dpkg -l | grep -w $1 &> /dev/null
}

# Update package lists
sudo apt update

# Install eSpeak if not installed
if ! is_package_installed espeak; then
    sudo apt install -y espeak
fi

# Install Mbrola and Arabic voices if not installed
if ! (is_package_installed mbrola && is_package_installed mbrola-ar1 && is_package_installed mbrola-ar2); then
    sudo apt install -y mbrola mbrola-ar1 mbrola-ar2
fi

# Install speech dispatcher if not installed
if ! is_package_installed speech-dispatcher; then
    sudo apt-get install -y speech-dispatcher
fi

# Install Orca screen reader if not installed
if ! is_package_installed orca; then
    sudo apt-get install -y orca
fi

# Install git and python if not installed
if ! (is_package_installed git && is_package_installed python3); then
    sudo apt install -y git python3
fi

# Install Mishkal Arabic diacritizer
# Move to the Downloads folder
cd ~/Downloads
if [ ! -d "mishkal" ]; then
    git clone https://github.com/linuxscout/mishkal.git
fi
cd mishkal
sudo python3 setup.py install


orca -r