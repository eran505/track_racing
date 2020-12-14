#!/bin/bash

SERVER=$(hostname)
PATHtoFileDir=${HOME}"/eran/repo/track_racing"
if [[ $PATHtoFileDir == *"/eranhe/"* ]]; then
  PATHtoFileDir=${HOME}"/eran/repo/track_racing/cmake-build-debug/"
fi
if [[ $PATHtoFileDir == *"/ERANHER/"* ]]; then
  PATHtoFileDir=${HOME}"/eran/repo/track_racing/cmake-build-debug/"
fi
if [[ $SERVER == *"ubuntu-eranh"* ]]; then
  PATHtoFileDir=${HOME}"/eran/repo/track_racing/cmake-build-debug/"
fi
if [[ ${HOME} == *"/home/lab2"* ]]; then
  PATHtoFileDir=${HOME}"/eranher/repo/track_racing"
fi

ConstSeed=$RANDOM

for i in {0..3}
do
  echo "-->$i<--"
  cd "${PATHtoFileDir}" || exit
  seed="$((ConstSeed+$i*31))"
    echo "${seed}"
    ./track_racing s ${seed}
done
