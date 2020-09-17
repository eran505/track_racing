#!/bin/bash


PATHtoFileDir=${HOME}"/eran/repo/track_racing"
if [[ $PATHtoFileDir == *"/eranhe/"* ]]; then
  PATHtoFileDir=${HOME}"/eran/"
fi
if [[ $PATHtoFileDir == *"/ERANHER/"* ]]; then
  PATHtoFileDir=${HOME}"/eran/"
fi

ConstSeed=15941915

for i in {1..100}
do
  echo "-->$i<--"
  cd "${PATHtoFileDir}" || exit
  seed="$(($ConstSeed+$i*31))"
    echo "${seed}"
    ./track_racing s ${seed}
done
