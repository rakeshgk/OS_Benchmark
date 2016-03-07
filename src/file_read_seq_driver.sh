#!/bin/sh

size=524288
path="/home/amit/os_dir"
rm /home/amit/acads/cse221/OS_Benchmark/data/file_read_seq.txt
touch /home/amit/acads/cse221/OS_Benchmark/data/file_read_seq.txt
#cd $path

#while [ $size -lt 10737418240 ]
while [ $size -lt 1048576 ]
do
    if [ $size -lt 1048576 ]
    then
        s=`expr $size / 1024`
        fname=$(printf "%d%s" "$s" "KB")
        runtime=30
        readsize=262144
    elif [ $size -lt 1073741824 ]
    then
        s=`expr $size / 1048576`
        fname=$(printf "%d%s" "$s" "MB")
        runtime=30
        readsize=262144
    else
        s=`expr $size / 1073741824`
        fname=$(printf "%d%s" "$s" "GB")
        runtime=30
        readsize=262144
    fi

    sync
    echo 3 > /proc/sys/vm/drop_caches

    fpath=$(printf "%s/%s" "$path" "$fname")
    echo "Working on file: $fpath"

    /home/amit/acads/cse221/OS_Benchmark/src/a.out $fpath $size $runtime $readsize

    sync
    echo 3 > /proc/sys/vm/drop_caches
    #sleep 30

    size=`expr $size \* 2`
    echo "Done"
    echo ""
done
