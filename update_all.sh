#!/usr/bin/env bash

if [ $# -eq 0 ]
  then
    eacho "--push--"
    git add .
    git commit -m "$(date +"%m/%d %H:%M:%S")"
    git push
    exit
  fi
eacho "--pull--"
git pull
cmake --build . --config Release

