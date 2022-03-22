TMP_DIR="/tmp/asgn7"
rm -rf "$TMP_DIR"
mkdir -p "$TMP_DIR"
calculate() {
    echo "Running program with hash table of size $1."
    #paste solution from https://stackoverflow.com/questions/10303600/how-to-concatenate-lines-into-one-string#10303766
    read -ra output <<< $(./identify -d small.db -v -B $1 < $text | tail -n 6 | cut -f 2 -d ':' | sed 's/ //g' | paste -s -d ' ')
    echo "$1 ${output[0]}" >> "$TMP_DIR/hash-probes-per-insertion.dat"
    echo "$1 ${output[1]}" >> "$TMP_DIR/hash-probes-per-lookup.dat"
    echo "$1 ${output[2]}" >> "$TMP_DIR/hash-avg-table-load.dat"
    echo "$1 ${output[3]}" >> "$TMP_DIR/hash-max-table-load.dat"
    echo "$1 ${output[4]}" >> "$TMP_DIR/bloom-false-pos.dat"
    echo "$1 ${output[5]}" >> "$TMP_DIR/times.dat"
}

big_time() {
    min=500000
    max=100000000
    for (( ht_size=$min; ht_size < $max; ht_size += 500000 )); do
        calculate $ht_size
    done
}

smaller_time() {
    min=28000
    max=100000000
    for (( ht_size=$min; ht_size <= $max; ht_size += ht_size / 15 )); do
        calculate $ht_size
    done
}

smallest_time() {
    min=10
    max=3000001
    for (( ht_size=$min; ht_size <= $max; ht_size += ht_size / 10 )); do
        calculate $ht_size
    done
}

text=texts/baum.txt
[ $# -gt 0 ] && text=$1

make clean all

# warmup runs, as seen on hyperfine
echo -n "Warming up..."
for i in {1..5}; do
    echo -n "$i..."
    ./identify -v -d small.db < $text > /dev/null
done
echo

#big_time
#smaller_time
smallest_time

echo "It's all part of my evil plot."
PLOT_DIR=script_plots
mkdir -p "$PLOT_DIR"
gnuplot << END
    set terminal pdf
    set title "Time to Finish"
    set xlabel "Bloom Filter Size"
    set ylabel "Time (secs)"
    set log x
    set xrange [$min:$max]
    set key top left
    set output "$PLOT_DIR/time.pdf" 
    plot "$TMP_DIR/times.dat" with lines title ""
END
