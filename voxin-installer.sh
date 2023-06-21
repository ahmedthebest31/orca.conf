#!/bin/bash

# Prompt for sudo password
sudo -v

# Exit immediately if any command fails
set -e

directory="/path/to/extras/Voxin-ENU-V3.3-RC4"

# Check if directory exists
if [ ! -d "$directory" ]; then
    echo "Directory $directory does not exist."
    exit 1
fi

# Change directory and modify permissions
cd "$directory"
chmod +x voxin-installer.sh

# Execute the installer script with sudo
sudo ./voxin-installer.sh
