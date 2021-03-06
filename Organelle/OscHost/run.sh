#!/bin/bash

export USER_DIR=${USER_DIR:="/usbdrive"}
# PATCH_DIR=${PATCH_DIR:="/usbdrive/Patches"}
# FW_DIR=${FW_DIR:="/root"}
# SCRIPTS_DIR=$FW_DIR/scripts


sudo systemctl stop serial-getty@ttymxc0.service
#sudo dmesg -n 1

export DIR=`dirname $0`
echo $DIR
cd $DIR

#~/scripts/killpatch.sh

oscsend localhost 4001 /oled/line/1 s "running OscProxy"
oscsend localhost 4001 /oled/line/2 s "mother : 4001"
oscsend localhost 4001 /oled/line/3 s "process : 4000"
oscsend localhost 4001 /oled/line/4 s " "
oscsend localhost 4001 /oled/line/5 s " "
oscsend localhost 4001 /oled/setscreen i 3

./OscProxy 4000 & echo $! > /tmp/pids/oscproxy.pid


