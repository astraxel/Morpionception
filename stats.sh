#!/bin/bash

loss=0
draw=0
victory=0

nb=$1
shift

for i in `seq $nb`
do
    ./minmax -q $@
    res=$?
    if [ $res = 0 ]
    then
        echo -n 'O'
        ((++loss))
    elif [ $res = 1 ]
    then
        echo -n '#'
        ((++draw))
    else
        echo -n 'X'
        ((++victory))
    fi
done

echo
echo "O: $loss"
echo "#: $draw"
echo "X: $victory"
