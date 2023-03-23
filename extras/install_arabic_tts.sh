#!/bin/bash 

# Update package lists 
sudo apt update 

# Install eSpeak 
sudo apt install -y espeak 

# Install Mbrola and arabic voices 
sudo apt install mbrola mbrola-ar1 mbrola-ar2 

# Install speech dispatcher 
sudo apt-get install -y speech-dispatcher 

# Install Orca screen reader 
sudo apt-get install -y  orca 
# install git and python
 sudo apt install git python3

# Install Mishkal Arabic diacritizer 
# move to downlad foder
cd ~/Downloads
git clone https://github.com/linuxscout/mishkal.git 
cd mishkal 
sudo python3 setup.py  install 
