#!/usr/bin/env bash

git add -A
git commit -m "$(date +"%m/%d %H:%M:%S")"
git push
