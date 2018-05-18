CRC-16 Generator polynomial G(x) = x¹⁶ + x¹² + x⁵ + 1
According to eMMC 5.1 JEDEC standard JESD84-B51.

I wrote this mainly to compare output from logic analyzer with the CRC-16
checksum both "bus" and "serial" wise.

Here's an example of usage:

<pre>
$ echo -n "test" | ./crc16mmc
Data length: 0x04 bytes
CRC-16 (MMC) dat[7]: 0x0000
CRC-16 (MMC) dat[6]: 0xf1ef
CRC-16 (MMC) dat[5]: 0xf1ef
CRC-16 (MMC) dat[4]: 0xb16b
CRC-16 (MMC) dat[3]: 0x0000
CRC-16 (MMC) dat[2]: 0xd1ad
CRC-16 (MMC) dat[1]: 0x2042
CRC-16 (MMC) dat[0]: 0x60c6
CRC-16 (MMC) LA bus output: 0x2e a6 ce 2e 00 00 00 2e a6 ce 2e 00 2e a6 ce 2e 
CRC-16 (MMC) serial: 0x9b06
</pre>

Compile with:  
gcc -Wpedantic crc16mmc.c -o crc16mmc

