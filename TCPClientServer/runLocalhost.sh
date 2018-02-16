#!/bin/bash
z=0
while [ $z -lt 10 ]
do
    bin/client "localost:12008" "testing.txt" "500"
    ((z++))
done
echo Finished
