#!/bin/sh
cd /home/pi/ectera/oscgateway
python udp2serial.py &
cd /home/pi/ectera/ETC_Mother
python2 main.py 
