#!/bin/sh

sudo chmod 777 /dev/ttyUSB0
sudo chmod 777 /dev/ttyACM0

User=`whoami`

sudo nice -n -19 sudo -u $User ./Control $1
