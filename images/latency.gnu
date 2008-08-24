set terminal postscript eps
plot 'latency.dat' using ($1*1000):4:6 with errorbars lt rgb "blue" t "VPMN"
