#!/bin/sh
echo "---------------------------" >> ./RunFiles/scl.txt
echo $(date) >> ./RunFiles/scl.txt
echo "" >> ./RunFiles/scl.txt
sudo mkfs -t ext3 -q /dev/ram1 4096
sudo [ ! -d ./ramdisk ] && mkdir -p ./ramdisk >>./RunFiles/scl.txt  2>&1
sudo mount /dev/ram1 ./ramdisk >>./RunFiles/scl.txt 2>&1

sudo chmod 777 ./ramdisk >>./RunFiles/scl.txt 2>&1

#sudo chmod 777 /dev/video1 >>./RunFiles/scl.txt 2>&1


sudo killall gpsd >>./RunFiles/scl.txt 2>&1
sudo gpsd /dev/serial/by-id/usb-MTK_GPS_Receiver-if01 >>./RunFiles/scl.txt 2>&1

sudo chmod 777 /dev/serial/by-id/usb-FTDI_Single_RS232-HS-if00-port0
sudo chmod 777 /dev/serial/by-id/usb-Prolific_Technology_Inc._USB-Serial_Controller-if00-port0 >>./RunFiles/scl.txt 2>&1
sudo chmod 777 /dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_74134373733351609070-if00 >>./RunFiles/scl.txt 2>&1
sudo chmod 777 /dev/serial/by-id/usb-Xsens_Xsens_USB-serial_converter_XST8R2K9-if00-port0 >>./RunFiles/scl.txt 2>&1

wget -q --spider http://google.com

if [ $? -eq 0 ]; then
	echo "Online"
else
	sudo python M2Mconnect.py
fi

User=`whoami`
ulimit -c unlimited
sudo nice -n -19 sudo -u $User ./Control $1

