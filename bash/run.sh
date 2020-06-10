#!/bin/bash


PATHtoFileDir="/home/ERANHER/eran/repo/track_racing/cmake-build-debug/"


for i in {0..14}
do
  cd ${PATHtoFileDir}
  ./track_racing
done