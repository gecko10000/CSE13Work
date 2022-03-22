mkdir -p /tmp/
make clean && make

SIZE=10010
IFS=" ,"

SORT_NAMES=(insertion batcher heap quick)
PLOT_DIR=plots
plot_together () {
    echo "set output '$PLOT_DIR/$1.pdf'"
    echo -n "plot "
    for ((i = 0; i < 4; i++)); do
        s=${SORT_NAMES[i]}
        # https://stackoverflow.com/questions/34705166/gnuplot-multiple-files-in-one-plot
        # capitalization from https://linuxhint.com/bash_lowercase_uppercase_strings/
        echo -n "\"/tmp/$s-$1.dat\" with $2 title \"${s^} Sort\""
        if ((i != 3)); then
            echo -n ,
        fi
    done
}

plot_separately () {
    for ((i = 0; i < 4; i++)); do
        s=${SORT_NAMES[i]}
        echo "set output '$PLOT_DIR/$1-$s.pdf'"
        echo "plot \"/tmp/$s-$1.dat\" with $2 title \"${s^} Sort\""
    done
}

rm -rf $PLOT_DIR/*
mkdir -p $PLOT_DIR
# run each sort separately

do_sorts () {
    for ((i = 0; i < 4; i++)); do
        s=${SORT_NAMES[$i]}
        rm -f /tmp/$s*.dat
        echo "Doing $SIZE $s sorts"
        # increment by 1 until 10, at which point we start incrementing by 10^(length(j) - 2)
        # length of j from https://linuxhandbook.com/bash-string-length/
        for ((j = 2; j < $SIZE; j+=(j < 10 ? 1 : 10))); do #= 10**$((${#j}-(j < 10 ? 1 : j < 100 ? 2 : 3))))); do
            #if ((j % 250 == 0)); then
                echo -n "$j|"
            #fi
            # read output of sorting program into array, split with spaces and commas
            # s:0:1 format from https://reactgo.com/bash-get-first-character-of-string/
            read -ra output <<< $(./sorting -${s:0:1} -n $j -p 0)
            moves=${output[4]}
            compares=${output[6]}
            echo "$j $moves" >> /tmp/$s-moves.dat
            echo "$j $compares" >> /tmp/$s-compares.dat
            nn_moves=$(echo "$moves/$(($j*$j))" | bc -l)
            echo "$j $nn_moves" >> /tmp/$s-nn-moves.dat
            nn_compares=$(echo "$compares/$(($j*$j))" | bc -l)
            echo "$j $nn_compares" >> /tmp/$s-nn-compares.dat
            nlogn_moves=$(echo "$moves/($j*l($j))" | bc -l)
            echo "$j $nlogn_moves" >> /tmp/$s-nlogn-moves.dat
            nlogn_compares=$(echo "$compares/($j*l($j))" | bc -l)
            echo "$j $nlogn_compares" >> /tmp/$s-nlogn-compares.dat
        done
    echo $SIZE
    done
}

# only sort if argument is given
if [ $# -ne 0 ] && [ $1 == "-sort" ]; then
    do_sorts
fi

gnuplot << END
    set terminal pdf
    set title "Moves for Each Sort"
    set xlabel "Array Size"
    set ylabel "Moves"
    set log x
    set log y
    set key top left
    $(plot_together moves lines)
    set title "Compares for Each Sort"
    set ylabel "Compares"
    $(plot_together compares lines)
    set title "moves / n^2"
    set ylabel "c"
    unset log x
    unset log y
    set xrange [10:10000]
    $(plot_separately nn-moves dots)
    set title "compares / n^2"
    $(plot_separately nn-compares dots)
    set title "moves / nlogn"
    $(plot_separately nlogn-moves dots)
    set title "compares / nlogn"
    $(plot_separately nlogn-compares dots)
END
