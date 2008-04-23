#!/bin/sh

for f in `ls -l *.Zspc | awk '{print $8 " " $5}'`; do  if [ -z $filename ]; then filename=$f; else  if [ $f -le 10000 ]; then svn rm $filename ; fi; unset filename; fi; done
