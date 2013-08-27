#! /bin/bash

LEN=$(echo $1 | wc -c)
LEN=$(expr $LEN - 5)
CUT=$(echo $1 | cut -c -$LEN)
mv $1 $CUT.amr
