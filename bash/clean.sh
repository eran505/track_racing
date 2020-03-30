#!/bin/bash

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



