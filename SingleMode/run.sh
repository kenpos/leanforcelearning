#!/bin/sh
#スパコンで実行する場合,windowsとlinuxマシンでは改行文字が違うため,一度変換かます必要がある.
#chomd +x runrun.sh　で実行権の付与も忘れずに
#awk '{ sub("\r$",""); print}' run.sh > runrun.sh
echo "Hello, World!"
THIS_FILE_PATH="$PWD/$0"
echo ${THIS_FILE_PATH%/*}

for i in noblindcount blindcount;
do
for j in moveenemy nomove;
do
for var in 3 7;
do
#make folda
#dir=$i/$j/$var; [ ! -e $dir ] && mkdir -p $dir
#cd /home/s1510037/ResrachData/$i/$j/$var/
#cd ${THIS_FILE_PATH%/*}/$i/$j/$var/
#g++ -o a.o -std=c++11 Source.cpp Header.h
icpc -O3 -o a.o -std=c++11 Source.cpp Header.h

./a.o &
printf "$i/$j/$var finish\n"
done
done
done
printf "ALL job finish"
