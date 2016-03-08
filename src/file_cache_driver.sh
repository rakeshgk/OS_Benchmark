#!/bin/sh

size=134217728
runs=4
runtime=5
readsize=262144
path="/home/amit/os_dir"
rm /home/amit/acads/cse221/OS_Benchmark/data/file_cache_size.txt
touch /home/amit/acads/cse221/OS_Benchmark/data/file_cache_size.txt
#cd $path

#while [ $size -lt 10737418240 ]
while [ $size -lt 10737418240 ]
do
    s=`expr $size / 1048576`
    fname=$(printf "%d%s" "$s" "MB")
    sync
    echo 3 > /proc/sys/vm/drop_caches
    fpath=$(printf "%s/%s" "$path" "$fname")
    echo "Working on file: $fpath"

    /home/amit/acads/cse221/OS_Benchmark/src/a.out $fpath $size $runtime $readsize $runs

    sync
    echo 3 > /proc/sys/vm/drop_caches
    sleep 10

    if [ $size -lt 1073741824 ]
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
