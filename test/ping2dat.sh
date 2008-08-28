#!/bin/sh
#plot 'file1.dat' using ($1*1000):4:6 with errorbars lt rgb "blue", 'file2.dat' using ($1*1000):4:6 with errorbars lt rgb "red"
COUNT=10
IP=$1
echo '#Test' $2
echo '#INT LOSS MIN AVG MAX MDEV'
for INT in 2 1.8 1.6 1.4 1.2 1 0.8 0.6 0.4 0.2; do
echo -n $INT' '
ping -q -c $COUNT -i $INT $IP | awk 'BEGIN{ORS=" "}/packet loss/{sub(/%/," "); print $6} /^rtt /{gsub(/\//," "); print $7,$8,$9,$10}'
echo
done
echo
