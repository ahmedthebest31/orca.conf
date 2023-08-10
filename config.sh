#!/bin/bash

# Check if Orca is installed
if ! command -v orca &> /dev/null; then
    # Install Orca
    sudo apt update
    sudo apt install -y orca
fi

# Create the directory if it doesn't exist
if [ ! -d ~/.local/share/orca ]; then
    mkdir -p ~/.local/share/orca
fi

# Replace the user-settings.conf file
cp ./extras/user-settings.conf ~/.local/share/orca/

# Set Orca to auto start with the system
if [ -f ~/.config/autostart/orca.desktop ]; then
    echo "Autostart configuration already exists."
else
    echo "[Desktop Entry]
Name=Orca Screen Reader
GenericName=Screen Reader
Comment=Start Orca screen reader on login
Exec=orca
Terminal=false
Type=Application
X-GNOME-Autostart-enabled=true" > ~/.config/autostart/orca.desktop

    echo "Autostart configuration created."
fi
