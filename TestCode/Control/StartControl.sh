#!/bin/sh

sudo mkfs -t ext3 -q /dev/ram1 8192
sudo [ ! -d ./ramdisk ] && mkdir -p ./ramdisk
sudo mount /dev/ram1 ./ramdisk

sudo chmod 777 ./ramdisk

sudo chmod 777 /dev/video1


sudo killall gpsd

sudo gpsd /dev/serial/by-id/usb-MTK_GPS_Receiver-if01

sudo chmod 777 /dev/serial/by-id/usb-Prolific_Technology_Inc._USB-Serial_Controller-if00-port0
sudo chmod 777 /dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_74134373733351609070-if00

User=`whoami`

sudo nice -n -19 sudo -u $User ./Control $1
