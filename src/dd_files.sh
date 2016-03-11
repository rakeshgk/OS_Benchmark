#!/bin/sh

size=33554432
step=33554432
bs=134217728
fpath="/home/amit/os_dir"

cd $fpath

#    if [ $size -lt 1048576 ]
#    then
#        s=`expr $size / 1024`
#	bs=262144
#	cnt=`expr $size / $bs`
#        fname=$(printf "%d%s" "$s" "KB")

while [ $size -lt 10737418240 ]
do
    if [ $size -lt 134217728 ]
    then
        s=`expr $size / 1048576`
        bs=33554432
        cnt=`expr $size / $bs`
        fname=$(printf "%d%s" "$s" "MB")
        step=33554432
    elif [ $size -lt 1073741824 ]
    then
        s=`expr $size / 1048576`
        bs=134217728
        cnt=`expr $size / $bs`
        fname=$(printf "%d%s" "$s" "MB")
        step=134217728
    elif [ $size -ge 1073741824 -a $size -lt 4294967296 ]
    then
        s=`expr $size / 1048576`
        bs=268435456
        cnt=`expr $size / $bs`
        fname=$(printf "%d%s" "$s" "MB")
        step=536870912
    else
        s=`expr $size / 1048576`
        bs=536870912
        cnt=`expr $size / $bs`
        fname=$(printf "%d%s" "$s" "MB")
        step=1073741824
    fi
    dd if=/dev/urandom of=$fname bs=$bs count=$cnt iflag=fullblock
    #fi
    #size=`expr $size \* 2`
    size=`expr $size + $step`
done
