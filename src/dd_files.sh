#!/bin/sh

size=1073741824
fpath="/home/amit/os_dir"

cd $fpath

while [ $size -lt 10737418240 ]
do
    if [ $size -lt 1048576 ]
    then
        s=`expr $size / 1024`
	bs=262144
	cnt=`expr $size / $bs`
        fname=$(printf "%d%s" "$s" "KB")
    elif [ $size -lt 1073741824 ]
    then
        s=`expr $size / 1048576`
	bs=1048576
	cnt=`expr $size / $bs`
	fname=$(printf "%d%s" "$s" "MB")
    else
        s=`expr $size / 1073741824`
	bs=67108864
	cnt=`expr $size / $bs`
	fname=$(printf "%d%s" "$s" "GB")
    fi
    dd if=/dev/urandom of=$fname bs=$bs count=$cnt iflag=fullblock
    size=`expr $size \* 2`
done
