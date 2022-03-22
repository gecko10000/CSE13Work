chars=(a b c d e f g h i j)
lows=(0 2 -10 -3.141593 -3.141593 2 -12.566371 1 0 0)
highs=(1 3 10 3.141593 3.141593 10 12.566371 10 1 3.141593)
table_values=(0.87401918476405 1.118424814549702 1.772453850905508 1.545303425380133 1.131387027213366 3.952914142858876 2.984322451168924 0.2193797774265986 6.316563839027766 3.141592653589797)

mkdir -p /tmp
make clean && make integrate

IFS=','
SIZE=25
for i in {0..9}; do
    filename="integrate$i"
    ./integrate -${chars[$i]} -p ${lows[$i]} -q ${highs[$i]} -n $(($SIZE*2)) > /tmp/$filename.dat
    read -ra info <<< $(cat /tmp/$filename.dat | head -n 1) # https://www.javatpoint.com/bash-split-string
    if [[ $i -eq 2 ]]; then
        info[0]="e^{-x^2}"
    elif [[ $i -eq 7 ]]; then
        info[0]="e^{-x} / x"
    elif [[ $i -eq 8 ]]; then
        info[0]="e^{e^x}"
    fi
    cat /tmp/$filename.dat | tail -n $SIZE > .tmp && mv .tmp /tmp/$filename.dat
    gnuplot << END
        set terminal pdf
        set output "$filename.pdf"
        set title "${info[0]} from ${info[1]} to ${info[2]} = ${table_values[$i]}"
        set xlabel "Partitions"
        set ylabel "Approximation"
        set xrange [2:${info[3]}]
        set datafile separator ","
        plot "/tmp/$filename.dat" using 1:2 with lines title ""
END
# http://gnuplot.sourceforge.net/docs_4.2/node173.html for datafile separator
done
