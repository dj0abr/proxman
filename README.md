# proxman
Proxmox Manager, starting VMs and Remote-Viewer with only ONE mouse click, for Linux only

![alt text](https://github.com/dj0abr/proxman/blob/main/pm7.png)

# 3rd party program required
* cv4pve-pepper must be copied into this directory. Use the version that fits to your computer (amd64, ARM...)\
this program is available here: \
https://github.com/Corsinvest/cv4pve-pepper/releases
the version supplied here is running on 64 bit PCs (amd64).

* remote-viewer, which is included in the package virt-viewer which can be installed using apt.

* uses my websocket implementation which is also in my github repos:
https://github.com/dj0abr/kmclib

# compilation

simply enter: make\
this generates the program "proxman"

# usage

1) start the daemon by entering ./proxman
2) load the file proxman.html with a web-browser
3) click on a VM to open (or start+open) it in remote-viewer

