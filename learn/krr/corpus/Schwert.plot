##### output file options #####
set terminal pngcairo size 1536,512 enhanced font 'Verdana,14'
set output '/home/patrick/Documents/Diplomarbeit/03_Software/04_Release/qtalearnf0/learn/krr/training/plots/Schwert.png'

##### general settings #####
set datafile missing '0'
stats 'Schwert.qtaf0' using 2:3 nooutput
set xrange [0.347646:1.0129]
set yrange [STATS_min_y-2:STATS_max_y+2]
set title 'F0 - Schwert (rmse=8.54983)'
set xlabel 'Time [sec]'
set ylabel 'Frequency [st]'

##### plot syllable bounds #####
set arrow from 0.347646,STATS_min_y-2 to 0.347646,STATS_max_y+2 nohead dt 3 lt -1
set arrow from 1.012901,STATS_min_y-2 to 1.012901,STATS_max_y+2 nohead dt 3 lt -1

###### plot targets #####
set arrow from 0.347646,96.493899 to 1.012901,104.162386 nohead dt 4

###### plot curves from data points #####
plot 'Schwert.origf0' using 2:3 title 'Original F0' with points lt 7 lc '#0000FF' lw 1,\
 'Schwert.qtaf0' using 2:3 title 'Resynthesized F0' with linespoints lt -1 lc '#000000' lw 2
