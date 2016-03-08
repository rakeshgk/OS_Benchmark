#!/bin/sh
i=1
size=1073741824
procs=10
path="/home/amit/os_dir/contention"
runtime=5
readsize=262144
runs=2
j=1
while [ $j -le $procs ]
do
    echo $(printf "Contention with %d procs" "$j")
    resfile=$(printf "/home/amit/acads/cse221/OS_Benchmark/data/file_read_contention_%d.txt" "$j")
    rm $resfile
    touch $resfile
    #cd $path
    sync
    echo 3 > /proc/sys/vm/drop_caches
    while [ $i -le $j ]
    do
        fpath=$(printf "%s/%d_1GB" "$path" "$i")
        #sleep 10
        /home/amit/acads/cse221/OS_Benchmark/src/a.out $fpath $size $runtime $readsize $j $runs &

        i=`expr $i + 1`
    done
    sync
    echo 3 > /proc/sys/vm/drop_caches
    sl=`expr $runtime \* $runs`
    sleep `expr $sl + 5`

    echo $(printf "Done for %d procs" "$j")
    echo ""
    j=`expr $j + 1`
    i=1
done
