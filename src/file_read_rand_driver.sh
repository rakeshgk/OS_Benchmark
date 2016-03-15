#!/bin/sh

size=33554432
runs=1
readsize=262144
path="/home/amit/os_dir/remote"
rm /home/amit/acads/cse221/OS_Benchmark/data/file_read_rand_1.csv
touch /home/amit/acads/cse221/OS_Benchmark/data/file_read_rand_1.csv
#cd $path

#while [ $size -lt 10737418240 ]
while [ $size -le 4294967296 ]
do
    s=`expr $size / 1048576`
    fname=$(printf "%d%s" "$s" "MB")
    sync
    echo 3 > /proc/sys/vm/drop_caches
    fpath=$(printf "%s/%s" "$path" "$fname")
    echo "Working on file: $fpath"

    /home/amit/acads/cse221/OS_Benchmark/src/a.out $fpath $size $readsize $runs nice -20

    sync
    echo 3 > /proc/sys/vm/drop_caches
    sleep 10

    if [ $size -lt 134217728 ]
    then
        size=`expr $size + 33554432`
    elif [ $size -lt 1073741824 ]
    then
        size=`expr $size + 134217728`
    elif [ $size -ge 1073741824 -a $size -lt 4294967296 ]
    then
        size=`expr $size + 536870912`
    else
        size=`expr $size + 1073741824`
    fi

    echo "Done"
    echo ""
done
