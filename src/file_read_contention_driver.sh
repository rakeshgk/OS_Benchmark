#!/bin/sh
i=1
size=268435456
procs=10
path="/home/amit/os_dir/contention"
readsize=262144
runs=1
j=10
runtime=60
while [ $j -le $procs ]
do
    echo $(printf "Contention with %d procs" "$j")
    #resfile=$(printf "/home/amit/acads/cse221/OS_Benchmark/data/file_read_contention_%d.csv" "$j")
    #rm $resfile
    #touch $resfile
    #cd $path
    sync
    echo 3 > /proc/sys/vm/drop_caches
    while [ $i -le $j ]
    do
        fpath=$(printf "%s/%d_1GB" "$path" "$i")
        #sleep 10
        /home/amit/acads/cse221/OS_Benchmark/src/a.out $fpath $size $readsize $j $runs nice -20 &

        i=`expr $i + 1`
    done

    #sl=`expr $runtime \* $runs`
    #sleep `expr $sl + 5`
    sleep 120

    sync
    echo 3 > /proc/sys/vm/drop_caches

    echo $(printf "Done for %d procs" "$j")
    echo ""
    j=`expr $j + 1`
    i=1
done
