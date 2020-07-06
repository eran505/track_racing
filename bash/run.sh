#!/bin/bash


PATHtoFileDir=${HOME}"/eran/repo/track_racing/"

for i in {1..1000}
do
  echo "-->$i<--"
  cd "${PATHtoFileDir}" || exit
  ./track_racing s ${i}
done
