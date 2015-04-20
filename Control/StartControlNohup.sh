#!/bin/sh
export TERM=xterm # elevate webservers 'dumb' terminal to xterm
cd /opt/SAE/git/SAEAuto/Control # change to control directory
sudo killall Control #kill any running processes
sleep 1

sudo mkfs -t ext3 -q /dev/ram1 8192
sudo [ ! -d ./ramdisk ] && mkdir -p ./ramdisk
sudo mount /dev/ram1 ./ramdisk

sudo chmod 777 ./ramdisk

sudo chmod 777 /dev/video1


sudo killall gpsd

sudo gpsd /dev/serial/by-id/usb-MTK_GPS_Receiver-if01

sudo chmod 777 /dev/serial/by-id/usb-Prolific_Technology_Inc._USB-Serial_Controller-if00-port0
sudo chmod 777 /dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_74134373733351609070-if00
sudo chmod 777 /dev/serial/by-id/usb-Xsens_Xsens_USB-serial_converter_XST8R2K9-if00-port0

User=`whoami`

sudo nohup nice -n -19 sudo -u $User ./Control $1 > whatever.stdout 2> whatever.stderr < /dev/null &
sleep 2
pidof Control


