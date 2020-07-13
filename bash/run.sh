#!/bin/bash


PATHtoFileDir=${HOME}"/eran/repo/track_racing"
if [[ $PATHtoFileDir == *"/eranhe/"* ]]; then
  PATHtoFileDir=${HOME}"/eran/repo/track_racing/cmake-build-debug/"
fi

for i in {1..1000}
do
  echo "-->$i<--"
  cd "${PATHtoFileDir}" || exit
  ./track_racing
done
