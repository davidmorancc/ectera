#!/bin/sh
cd /home/pi/ectera/udp2serial
python udp2serial.py &
cd /home/pi/ectera/ETC_Mother
python2 main.py 
