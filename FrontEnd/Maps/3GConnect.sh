#!/bin/sh

cd /home/pi/SAEAuto/Control
file="./M2MconnectLOCKfile.lock"
if [ -e "$file" ]
then
	sudo rm ./M2MconnectLOCKfile.lock
fi

wget -q --spider http://google.com

if [ $? -eq 0 ]; then
	echo "Online"
else
	echo "Connecting..."	
	sudo python M2Mconnect.py
fi