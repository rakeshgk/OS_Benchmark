#!/bin/sh

fpath="/home/amit/os_dir"
cd $fpath
dd if=/dev/urandom of=384MB bs=134217728 count=3 iflag=fullblock
dd if=/dev/urandom of=640MB bs=134217728 count=5 iflag=fullblock
dd if=/dev/urandom of=768MB bs=134217728 count=6 iflag=fullblock
dd if=/dev/urandom of=896MB bs=134217728 count=7 iflag=fullblock
dd if=/dev/urandom of=1536MB bs=134217728 count=12 iflag=fullblock
dd if=/dev/urandom of=2560MB bs=134217728 count=20 iflag=fullblock
dd if=/dev/urandom of=3072MB bs=134217728 count=24 iflag=fullblock
dd if=/dev/urandom of=3584MB bs=134217728 count=28 iflag=fullblock
dd if=/dev/urandom of=5120MB bs=134217728 count=40 iflag=fullblock
dd if=/dev/urandom of=6144MB bs=134217728 count=48 iflag=fullblock
dd if=/dev/urandom of=7168MB bs=134217728 count=56 iflag=fullblock
