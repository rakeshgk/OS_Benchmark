for i in `seq 1 1000`
do
    # Use this command if you have root terminal access
    # free && sync && echo 3 > /proc/sys/vm/drop_caches && free

    # Use this command if you want to do a sudo in a normal terminal
    # echo "echo 1 > /proc/sys/vm/drop_caches" | sudo sh

    ./a.out nice -20
done
