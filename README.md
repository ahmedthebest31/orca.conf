# orca.conf

This repository contains my orca screen Reader Configuration, which can be used to customize the behavior of the screen reader according to your specific requirements.

Usage
To use this configuration file, you can either clone the repository and run the install script or manually install the configuration file.

Automated Installation
To install the configuration file using the provided script, follow these steps:

Clone the repository by running the following command in your terminal:

```bash
git clone https://github.com/ahmedthebest31/orca.conf.git
```
Navigate to the cloned repository:

```bash
cd orca.conf
```
Run the installation script:

```bash
chmod +x ./install.sh
./install.sh
```

The installation script performs the following actions:

Checks if Orca screen reader is installed, and installs it if necessary.
Checks if the ~/.local/share/orca directory exists, and creates it if necessary.
Replaces the default user-settings.conf file with my customized configuration file from this  repository.
Sets Orca to auto-start with the system using gnome-session-properties.
Restart Orca if necessary.

Manual Installation
To manually install the configuration file, follow these steps:

Download the user-settings.conf file from the cloned repository.

Replace the default user-settings.conf file located in the following directory:

/home/user/.local/share/orca

Restart Orca if necessary.

You can activate Orca by using the keyboard shortcut Win + Control + S or by running the following command in a terminal:

```bash
orca -r
```

Benefits
This configuration file can be particularly useful for:

Users who need to customize the behavior of Orca screen reader according to their specific requirements.
System administrators who need to install Orca on multiple machines.
Users who need to reinstall Orca on their own machine.

### Contributions
If you would like to contribute to this project, feel free to open a pull request or an issue on the GitHub repository. Your feedback and contributions are always welcome