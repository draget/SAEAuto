#!/bin/sh
export TERM=xterm # elevate webservers 'dumb' terminal to xterm
cd /home/pi/saeauto/Control # change to control directory
sudo killall Control #kill any running processes
sleep 1

sudo mkfs -t ext3 -q /dev/ram1 4096
sudo [ ! -d ./ramdisk ] && mkdir -p ./ramdisk
sudo mount /dev/ram1 ./ramdisk

sudo chmod 777 ./ramdisk

#sudo chmod 777 /dev/video1
sudo killall gpsd
sudo gpsd /dev/serial/by-id/usb-MTK_GPS_Receiver-if01

sudo rm /dev/ttyX0
sudo ln -s /dev/serial/by-id/usb-Xsens_Xsens_USB-serial_converter_XST8R2K9-if00-port0 /dev/ttyX0

sudo chmod 777 /dev/serial/by-id/usb-FTDI_Single_RS232-HS-if00-port0
sudo chmod 777 /dev/serial/by-id/usb-Prolific_Technology_Inc._USB-Serial_Controller-if00-port0
sudo chmod 777 /dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_74134373733351609070-if00
sudo chmod 777 /dev/serial/by-id/usb-Xsens_Xsens_USB-serial_converter_XST8R2K9-if00-port0

wget -T 6 -q --spider http://google.com

if [ $? -eq 0 ]; then
	echo "Online"
else
	sudo python M2Mconnect.py
fi

User=`whoami`

ulimit -c unlimited
sudo nohup nice -n -19 sudo -u $User ./Control $1 > Control.stdout 2> Control.stderr < /dev/null &
sleep 2
pidof Control
echo Success


