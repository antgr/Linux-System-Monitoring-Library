# Simplemonitor:


The Simplemonitor gets the CPUload, Memory load and ethernet load from the __linuxmonitoring__ lib at a interval of 1 second
and prints the value. Signal handler are use to stop and shutdown the app.


#### example print:
    ----------------------------------------------
    current CPULoad:5.09119
    average CPULoad 10.0671
    Max     CPULoad 10.0822
    Min     CPULoad 1.74111
    CPU: : Intel(R) Core(TM) i7-10750H CPU @ 2.60GHz
    ----------------------------------------------
    network load: wlp0s20f3 : 1.9kBit/s : 920Bit/s : 1.0kBit/s :  RX Bytes Startup: 15.8mByte TX Bytes Startup: 833.5mByte
    ----------------------------------------------
    memory load: 28.4% maxmemory: 16133792 Kb used: 4581564 Kb  Memload of this Process 170408 KB
    ----------------------------------------------