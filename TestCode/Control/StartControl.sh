#!/bin/sh

sudo chmod 777 /dev/ttyUSB0
sudo chmod 777 /dev/ttyACM0

sudo nice -n -19  ./Control
