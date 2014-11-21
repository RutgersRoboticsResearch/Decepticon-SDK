#!/bin/bash
if [ ! -d .config ]; then mkdir .config; fi
cd .config
if [ ! -d autostart ]; then mkdir autostart; fi
cd autostart
echo -e "[Desktop Entry]\nType=Application\nExec=lxterminal" >autoterm.desktop
