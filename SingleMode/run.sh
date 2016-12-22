#!/bin/sh
echo "Hello, World!"
for i in noblindcount blindcount;
do
for j in moveenemy nomove;
do
for var in 3 7;
do
#make folda
#dir=$i/$j/$var; [ ! -e $dir ] && mkdir -p $dir
cd /home/s1510037/ResrachData/$i/$j/$var/
pwd
icpc -o a.o -O3 -std=c++11 Source.cpp Header.h
./a.o &
printf "$i/$j/$var finish\n"
done
done
done
printf "finish"
