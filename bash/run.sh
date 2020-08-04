#!/bin/bash


PATHtoFileDir=${HOME}"/eran/repo/track_racing"
if [[ $PATHtoFileDir == *"/eranhe/"* ]]; then
  PATHtoFileDir=${HOME}"/eran/repo/track_racing/cmake-build-debug/"
fi
if [[ $PATHtoFileDir == *"/ERANHER/"* ]]; then
  PATHtoFileDir=${HOME}"/eran/repo/track_racing/cmake-build-debug/"
fi

ConstSeed=1594198815

for i in {1..100}
do
  echo "-->$i<--"
  cd "${PATHtoFileDir}" || exit
  seed="$(($ConstSeed+$i*10))"
  for j in {1..3}
   do
    echo "${seed}"
    echo "${j}"
    ./track_racing ${seed} ${j}
  done
done
