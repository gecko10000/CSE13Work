#!/bin/bash

mkdir -p /tmp
make clean && make collatz   # Rebuild the collatz executable.

lengths=()
maximums=()
for (( i=2; i <= 10000; i++ )); do
    nums=($(./collatz -n $i))
    length=$(echo "${nums[@]}" | wc -w)
    # appending information from https://stackoverflow.com/questions/55316852/append-elements-to-an-array-in-bash
    lengths+=($length)
    # printf statement to split array from https://stackoverflow.com/questions/15691942/print-array-elements-on-separate-lines-in-bash
    maximums+=($(printf '%s\n' "${nums[@]}" | sort -rn | head -n 1))
done
printf '%s\n' "${lengths[@]}" > /tmp/lengths.dat
printf '%s\n' "${maximums[@]}" > /tmp/maximums.dat
(printf '%s\n' "${lengths[@]}" | sort -rn | uniq -c | awk '{print $2 " " $1}') > /tmp/length_hist.dat

# This is the heredoc that is sent to gnuplot.
gnuplot << END
    set terminal pdf
    set output "collatz1.pdf"
    set title "Collatz Sequence Lengths"
    set xlabel "n"
    set ylabel "length"
    set zeroaxis
    set xrange [0:10000]
    set yrange [0:300]
    plot "/tmp/lengths.dat" with dots title ""
    set output "collatz2.pdf"
    set title "Maximum Collatz Sequence Value"
    set ylabel "value"
    set yrange [0:100000]
    plot "/tmp/maximums.dat" with dots title ""
    set output "collatz3.pdf"
    set title "Collatz Sequence Length Histogram"
    set xlabel "length"
    set ylabel "frequency"
    set xrange [0:225]
    set yrange [0:200]
    set boxwidth 0
    plot "/tmp/length_hist.dat" with boxes title ""
END
