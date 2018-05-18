/*
    CRC-16 Generator polynomial G(x) = x¹⁶ + x¹² + x⁵ + 1
    According to eMMC 5.1 JEDEC standard JESD84-B51
    Copyright (C) 2017 Robert V. <modrobert@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>

// #define REFTEST

#ifdef REFTEST
uint16_t crc16_mmc_upd(uint8_t data, uint16_t crc_old);
#endif

int main()
{
 unsigned char buf[1];
 unsigned char buscrc[8][16];
 unsigned char serialcrc[16];
 int i, j;
 uint16_t crc16 = 0, crctest = 0, crcx = 0;
 uint8_t ch, inv, crcy = 0;
 unsigned long int len = 0;

 /* clear buffers for each bus line */
 for (i = 0; i < 8; i++)
 { 
  for (j = 0; j < 16; j++) buscrc[i][j] = 0;
 }
 for (j = 0; j < 16; j++) serialcrc[j] = 0;

 while (read(STDIN_FILENO, buf, sizeof(buf)) > 0)
 {
  len++;
  ch = buf[0];

#ifdef REFTEST
  /* reference test serial CRC16 round */
  crctest = crc16_mmc_upd(ch, crctest);
#endif

  /* do the serial CRC16 round */
  for (j = 0; j < 8; j++)
  {
   /* MSB to LSB order of ch */
   inv = ((ch >> (7 - j)) & 1) ^ serialcrc[15]; // XOR required?
   serialcrc[15] = serialcrc[14];
   serialcrc[14] = serialcrc[13];
   serialcrc[13] = serialcrc[12];
   serialcrc[12] = serialcrc[11] ^ inv;
   serialcrc[11] = serialcrc[10];
   serialcrc[10] = serialcrc[9];
   serialcrc[9] = serialcrc[8];
   serialcrc[8] = serialcrc[7];
   serialcrc[7] = serialcrc[6];
   serialcrc[6] = serialcrc[5];
   serialcrc[5] = serialcrc[4] ^ inv;
   serialcrc[4] = serialcrc[3];
   serialcrc[3] = serialcrc[2];
   serialcrc[2] = serialcrc[1];
   serialcrc[1] = serialcrc[0];
   serialcrc[0] = inv;
  }

  /* go through all "bits" on the bus and calculate CRC16 */
  for (i = 0; i < 8; i++)
  {
   /* MSB to LSB order of ch */
   inv = ((ch >> (7 - i)) & 1) ^ buscrc[i][15]; // XOR required?
   buscrc[i][15] = buscrc[i][14];
   buscrc[i][14] = buscrc[i][13];
   buscrc[i][13] = buscrc[i][12];
   buscrc[i][12] = buscrc[i][11] ^ inv;
   buscrc[i][11] = buscrc[i][10];
   buscrc[i][10] = buscrc[i][9];
   buscrc[i][9] = buscrc[i][8];
   buscrc[i][8] = buscrc[i][7];
   buscrc[i][7] = buscrc[i][6];
   buscrc[i][6] = buscrc[i][5];
   buscrc[i][5] = buscrc[i][4] ^ inv;
   buscrc[i][4] = buscrc[i][3];
   buscrc[i][3] = buscrc[i][2];
   buscrc[i][2] = buscrc[i][1];
   buscrc[i][1] = buscrc[i][0];
   buscrc[i][0] = inv;
  }

 } /* end while */

 printf("Data length: 0x%02lx bytes\n", len);

 if (len < 4 || len > 2048)
 {
  printf("Warning: The length of %lu bytes is not proper for eMMC 5.1 "
         "standard, \n", len);
  printf("         it should be in the interval [4,2048].\n");
 }

 /* print CRC16 for each data signal of the 8 bit bus */
 for (i = 0; i < 8; i++)
 {
  for (j = 0; j < 16; j++)
  {
   if (buscrc[i][j] == 1) // bits represented as bytes
   {
    crcx |= 1 << j;
   }
  }
  printf("CRC-16 (MMC) dat[%d]: 0x%04x\n", 7 - i, crcx);
  crcx = 0;
 }

 /* print CRC16 as 16 bytes on the 8 bit bus */
 printf("CRC-16 (MMC) LA bus output: 0x");
 for (j = 0; j < 16; j++)
 {
  for (i = 0; i < 8; i++)
  {
   /* CRC16 in this case MSB to LSB according to eMMC 5.1 */
   if (buscrc[i][15 - j] == 1) // bits represented as bytes
   {
    crcy |= 1 << i;
   }
  }
  printf("%02x ", crcy);
  crcy = 0;
 }
 printf("\n");

 /* print CRC16 as serial data */
 for (j = 0; j < 16; j++)
 {
  if (serialcrc[j] == 1) crc16 |= 1 << (j);
 }
 printf("CRC-16 (MMC) serial: 0x%04x\n", crc16);

#ifdef REFTEST
 /* print CRC16 serial reference test */
 printf("CRC-16 (MMC) serial ref test: 0x%04x\n", crctest);
#endif

 return 0;
}

#ifdef REFTEST
/* reference test: updates MMC CRC16 value crc_old with data */
uint16_t crc16_mmc_upd(uint8_t data, uint16_t crc_old)
{
 uint8_t cnt_bits, flag_xor;
 for (cnt_bits = 8; cnt_bits; cnt_bits--)
 {
  flag_xor = ((crc_old >> 8) & 0x80) ^ (data & 0x80);
  data <<= 1;
  crc_old <<= 1;
  if (flag_xor) crc_old ^= 0x1021;
 }
 return(crc_old);
}
#endif

