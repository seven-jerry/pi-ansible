#!/bin/bash
source /home/pi/src/lightcontrol-version.sh

lxterminal -e sudo  java -jar /home/pi/src/lightcontrol/lightcontrol-$version.jar --spring.profiles.active=client,local --server.port=80 --base.folder=/home/pi/lightcontrol-config