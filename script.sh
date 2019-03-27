#!/bin/bash

NodeLetter0=$(cat /dev/urandom | tr -dc 'A-F' | fold -w 1 | head -n 1)

NodeLetter1=$(cat /dev/urandom | tr -dc 'A-F' | fold -w 1 | head -n 1)
while [ $NodeLetter1 == $NodeLetter0 ]
do
    NodeLetter1=$(cat /dev/urandom | tr -dc 'A-F' | fold -w 1 | head -n 1)
done

NodeLetter2=$(cat /dev/urandom | tr -dc 'A-F' | fold -w 1 | head -n 1)
while [ $NodeLetter2 == $NodeLetter1 -o $NodeLetter2 == $NodeLetter0 ]
do
    NodeLetter2=$(cat /dev/urandom | tr -dc 'A-F' | fold -w 1 | head -n 1)
done

NodeLetter3=$(cat /dev/urandom | tr -dc 'A-F' | fold -w 1 | head -n 1)
while [ $NodeLetter3 == $NodeLetter2 -o $NodeLetter3 == $NodeLetter1 -o $NodeLetter3 == $NodeLetter0 ]
do
    NodeLetter3=$(cat /dev/urandom | tr -dc 'A-F' | fold -w 1 | head -n 1)
done

NodeLetter4=$(cat /dev/urandom | tr -dc 'A-F' | fold -w 1 | head -n 1)
while [ $NodeLetter4 == $NodeLetter3 -o $NodeLetter4 == $NodeLetter2 -o $NodeLetter4 == $NodeLetter1 -o $NodeLetter4 == $NodeLetter0 ]
do
    NodeLetter4=$(cat /dev/urandom | tr -dc 'A-F' | fold -w 1 | head -n 1)
done

NodeLetter5=$(cat /dev/urandom | tr -dc 'A-F' | fold -w 1 | head -n 1)
while [ $NodeLetter5 == $NodeLetter4 -o $NodeLetter5 == $NodeLetter3 -o $NodeLetter5 == $NodeLetter2 -o $NodeLetter5 == $NodeLetter1 -o $NodeLetter5 == $NodeLetter0 ]
do
    NodeLetter5=$(cat /dev/urandom | tr -dc 'A-F' | fold -w 1 | head -n 1)
done

g++ host_Connected.cpp hostobj_Connected.cpp -o host

gnome-terminal -- ./host $NodeLetter0 
gnome-terminal -- ./host $NodeLetter1
gnome-terminal -- ./host $NodeLetter2
gnome-terminal -- ./host $NodeLetter3
gnome-terminal -- ./host $NodeLetter4
gnome-terminal -- ./host $NodeLetter5
