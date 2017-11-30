# gr-gadget

Some tools that are useful in either gnuradio simulation or USRP experiment

# Installation

```
cd gr-gadget
mkdir build
cd build/
cmake ..
make
sudo make install
```

# Supported dataset format

 - .csv: Each element represents a byte (0-255) and each row is treated as a packet. Example:
 
 x | c0 | c1 | c2 | c3
---- | ---- | ---- | ---- | ----
r0 | 1, | 255, | 135, | 8
r1 | 25, | 55, | 88, | 7
 
 reads two packets and each has 4 bytes as payload.
