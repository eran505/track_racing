#!/bin/bash

find "$HOME/car_model/exp/" -type f -delete

if [ -d "$HOME/car_model/exp/data" ]; then
    echo "Exists"
else
  mkdir "$HOME/car_model/exp/data"
fi


if [ -z "$(ls -A ~/car_model/debug/)" ]; then
   echo "Empty debug"
else
   echo "Not Empty"
   rm ~/car_model/debug/*
fi

if [ -z "$(ls -A ~/car_model/exp/buffer/)" ]; then
   echo "Empty buffer"
else
   echo "Not Empty"
   rm  ~/car_model/exp/buffer/*
fi

if [ -z "$(ls -A ~/car_model/exp/out/)" ]; then
   echo "Empty out"
else
   echo "Not Empty"
   rm  ~/car_model/exp/out/*
fi

if [ -z "$(ls -A ~/car_model/exp/data/)" ]; then
   echo "Empty out"
else
   echo "Not Empty"
   rm  ~/car_model/exp/data/*
fi
echo "in"
if [ -z "$1" ];then
  echo "No config copy"
else
  cp $1 $HOME/car_model/exp/
fi



