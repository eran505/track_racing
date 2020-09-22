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


ConstSeed=81045917

for i in {1..30}
do
  echo "-->$i<--"
  cd "${PATHtoFileDir}" || exit
  seed="$(($ConstSeed+$i*31))"
    echo "${seed}"
    ./track_racing s ${seed}
done
