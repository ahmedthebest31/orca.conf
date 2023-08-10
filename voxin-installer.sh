#!/bin/bash

# Prompt for sudo password and prompt for password again if needed
sudo -v -A

# Exit immediately if any command fails
set -e

directory="/extras/Voxin-ENU-V3.3-RC4"

# Check if directory exists
if [ ! -d "$directory" ]; then
    echo "Directory $directory does not exist."
    exit 1
fi

# Change directory and modify permissions
cd "$directory"
if [ -e "voxin-installer.sh" ]; then
    chmod +x "voxin-installer.sh"
else
    echo "Installer script voxin-installer.sh not found."
    exit 1
fi

# Execute the installer script with sudo
sudo ./voxin-installer.sh
