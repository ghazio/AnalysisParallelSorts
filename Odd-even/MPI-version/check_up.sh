#!/bin/bash

if [ "$#" -ne 1 ]
then
  echo "usage ./checkhosts.sh hostfilename"
  exit 1
fi

for  i in `cat $1`
do
  echo "checking $i"
  ssh $i uptime
done
