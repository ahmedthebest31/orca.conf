#!/bin/bash

# Check if Orca is installed
if ! command -v orca &> /dev/null
then
    # Install Orca
    sudo apt update
    sudo apt install -y orca
fi

# Create the directory if it doesn't exist
if [ ! -d ~/.local/share/orca ]
then
    mkdir -p ~/.local/share/orca
fi

# Replace the user-settings.conf file
cp user-settings.conf ~/.local/share/orca/

# Set Orca to auto start with the system
gnome-session-properties <<EOF
[Desktop Entry]
Type=Application
Name=Orca
Exec=/usr/bin/orca
NoDisplay=true
EOF
