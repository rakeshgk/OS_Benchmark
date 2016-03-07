#!/bin/sh
i=1
size=1073741824
procs=2
path="/home/amit/os_dir/contention"
runtime=10
readsize=262144

while [ $i -le $procs ]
do
    echo $(printf "Contention with %d procs" "$i")
    resfile=$(printf "/home/amit/acads/cse221/OS_Benchmark/data/file_read_contention_%d.txt" "$i")
    rm $resfile
    touch $resfile
    #cd $path
    sync
    echo 3 > /proc/sys/vm/drop_caches
    #sleep 10
    /home/amit/acads/cse221/OS_Benchmark/src/a.out $path $size $runtime $readsize $i $resfile
    sync
    echo 3 > /proc/sys/vm/drop_caches
    #sleep 10

    echo "Done"
    echo ""
    i=`expr $i + 1`
done
