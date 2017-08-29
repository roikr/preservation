#!/bin/bash

while true
do
if [ ! `pgrep preservation` ] ; then
DISPLAY=:0.0 /home/ubuntu/openFrameworks/apps/preservation/preservation/bin/preservation
fi
done