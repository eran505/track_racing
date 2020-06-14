#!/bin/bash


PATHtoFileDir="/home/eranhe/eran/repo/track_racing/cmake-build-debug/"


for i in {0..100}
do
  cd ${PATHtoFileDir}
  ./track_racing
done