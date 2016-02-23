clock_cycles = [25000]
time = 4.1666666666667E-10
nano_snd = 1E-09

for cycle in clock_cycles:
    cycle_time =  (cycle * time) / nano_snd
    print("%s Cycles -> %s nano seconds\n" %(cycle, cycle_time))
