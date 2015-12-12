/* EasiOS kernel.c
 * ----------------
 * Author(s): Daniel (Easimer) Meszaros
 * ----------------
 * Description: Graphics, drawing, fonts
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <video.h>
#include <stdlib.h>
#include <stdio.h>

uint32_t* fb;
//uint32_t fbb[786432];
uint32_t fbb[1440000]; //maximum resolution: 1600x900 or equivalent
int64_t fbw = 0, fbh = 0, fbbpp = 0, fbp = 0;
uint8_t fbt;
rgb_t color;

uint16_t fb_font[128][16] = {
  [33] = {
    0,
    0,
    0x0F00,
    0x0F00,
    0x0F00,
    0x0F00,
    0x0F00,
    0x0F00,
    0x0F00,
    0,
    0x0F00,
    0x0F00,
    0, 0, 0
  },
  [34] = {
    0,0,0,
    0b1100110000000000,
    0b1100110000000000,
    0b1100110000000000,
    0b1100110000000000,
    0,0,0,0,0,0,0,0,0
  },
  [35] = {
    0,0,0,
    0b0011011000000000,
    0b0011011000000000,
    0b0111111100000000,
    0b0011011000000000,
    0b0011011000000000,
    0b0110110000000000,
    0b0110110000000000,
    0b1111111000000000,
    0b0110110000000000,
    0b0110110000000000,
    0,0,0
  },
  [36] = {
    0,0,0,
    0b0001100000000000,
    0b0011110000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110000000000000,
    0b0011110000000000,
    0b0000011000000000,
    0b0000011000000000,
    0b0110011000000000,
    0b0011110000000000,
    0b0001100000000000,
  },
  [37] = {
    0,0,0,
    0b0111000001100000,
    0b1101100011000000,
    0b1101100110000000,
    0b1101101100000000,
    0b0111011000000000,
    0b0000110111000000,
    0b0001101101100000,
    0b0011001101100000,
    0b0110001101100000,
    0b1100000111000000,
    0,0,0
  },
  [38] = {
    0,0,0,
    0b0001110000000000,
    0b0011011000000000,
    0b0011011000000000,
    0b0011011000000000,
    0b0001110000000000,
    0b0011100000000000,
    0b0110110100000000,
    0b0110011100000000,
    0b0110011000000000,
    0b0011111100000000,
    0,0,0
  },
  [39] = {
    0,0,0,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0,0,0,0,0,0,0,0,0
  },
  [40] = {
    0,0,0,
    0b0011000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0011000000000000,
  },
  [41] = {
    0,0,0,
    0b1100000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b1100000000000000,
  },
  [42] = {
    0,0,0,
    0b0011000000000000,
    0b1111110000000000,
    0b0011000000000000,
    0b0111100000000000,
    0b1000100000000000,
    0,0,0,0,0,0,0,0
  },
  [43] = {
    0,0,0,0,0,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0111111000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0,0,0,0
  },
  [44] = {
    0,0,0,0,0,0,0,0,0,0,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b1100000000000000,
    0,0
  },
  [45] = {
    0,0,0,0,0,0,0,0,
    0b1111000000000000,
    0,0,0,0,0,0,0
  },
  [46] = {
    0,0,0,0,0,0,0,0,0,0,0,
    0b0110000000000000,
    0b0110000000000000,
    0,0,0
  },
  [47] = {
    0,0,0,
    0b0011000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b1100000000000000,
    0b1100000000000000,
    0b1100000000000000,
    0b1100000000000000,
    0
  },
  [48] = {
    0,0,0,
    0b0011110000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0011110000000000,
    0,0,0
  },
  [49] = {
    0,0,0,
    0b0001100000000000,
    0b0111100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0,0,0
  },
  [50] = {
    0,0,0,
    0b0011110000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0000011000000000,
    0b0000110000000000,
    0b0001100000000000,
    0b0011000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0111111000000000,
    0,0,0
  },
  [51] = {
    0,0,0,
    0b0011110000000000,
    0b0110011000000000,
    0b0000011000000000,
    0b0000011000000000,
    0b0001110000000000,
    0b0000011000000000,
    0b0000011000000000,
    0b0000011000000000,
    0b0110011000000000,
    0b0011110000000000,
    0,0,0
  },
  [52] = {
    0,0,0,
    0b0000011000000000,
    0b0000111000000000,
    0b0001111000000000,
    0b0001111000000000,
    0b0011011000000000,
    0b0011011000000000,
    0b0110011000000000,
    0b0111111000000000,
    0b0000011000000000,
    0b0000011000000000,
    0,0,0
  },
  [53] = {
    0,0,0,
    0b0111111000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0111110000000000,
    0b0110011000000000,
    0b0000011000000000,
    0b0000011000000000,
    0b0110011000000000,
    0b0011110000000000,
  },
  [54] = {
    0,0,0,
    0b0011110000000000,
    0b0110011000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0111110000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0011110000000000,
    0,0,0
  },
  [55] = {
    0,0,0,
    0b0111111000000000,
    0b0000011000000000,
    0b0000110000000000,
    0b0000110000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0,0,0
  },
  [56] = {
    0,0,0,
    0b0011110000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0011110000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0011110000000000,
    0,0,0
  },
  [57] = {
    0,0,0,
    0b0011110000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0011111000000000,
    0b0000011000000000,
    0b0000011000000000,
    0b0110011000000000,
    0b0011110000000000,
    0,0,0
  },
  [58] = {
    0,0,0,0,0,
    0b0110000000000000,
    0b0110000000000000,
    0,0,0,0,
    0b0110000000000000,
    0b0110000000000000,
    0,0,0
  },
  [59] = {
    0,0,0,0,0,
    0b0110000000000000,
    0b0110000000000000,
    0,0,0,0,
    0b0110000000000000,
    0b0110000000000000,
    0b1100000000000000,
    0,0
  },
  [60] = {
    0,0,0,0,
    0b0000011000000000,
    0b0000110000000000,
    0b0001100000000000,
    0b0011000000000000,
    0b0110000000000000,
    0b0011000000000000,
    0b0001100000000000,
    0b0000110000000000,
    0b0000011000000000,
    0,0,0
  },
  [61] = {
    0,0,0,0,0,0,
    0b0111111000000000,
    0,0,
    0b0111111000000000,
    0,0,0,0,0,0
  },
  [62] = {
    0,0,0,0,
    0b0110000000000000,
    0b0011000000000000,
    0b0001100000000000,
    0b0000110000000000,
    0b0000011000000000,
    0b0000110000000000,
    0b0001100000000000,
    0b0011000000000000,
    0b0110000000000000,
    0,0,0
  },
  [63] = {
    0,0,0,
    0b0011110000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0000011000000000,
    0b0000110000000000,
    0b0001100000000000,
    0b0001100000000000,
    0,
    0b0001100000000000,
    0b0001100000000000,
    0,0,0
  },
  [64] = {
    0,0,
    0b0000011110000000,
    0b0001110011100000,
    0b0011000000110000,
    0b0011001110110000,
    0b0110011011011000,
    0b0110011011011000,
    0b0110110011011000,
    0b0110110110011000,
    0b0110110110011000,
    0b0011011011110000,
    0b0011000000000000,
    0b0001110001110000,
    0b0000011111000000,
    0
  },
  [65] = {
    0,0,0,
    0b0001100000000000,
    0b0001100000000000,
    0b0011110000000000,
    0b0011110000000000,
    0b0010010000000000,
    0b0100001000000000,
    0b0100001000000000,
    0b0111111000000000,
    0b1100001100000000,
    0b1100001100000000,
    0,0,0,
  },
  [66] = {
    0,0,0,
    0b0111111000000000,
    0b0110000100000000,
    0b0110000100000000,
    0b0110000100000000,
    0b0111111000000000,
    0b0110000100000000,
    0b0110000100000000,
    0b0110000100000000,
    0b0110000100000000,
    0b0111111000000000,
    0,0,0
  },
  [67] = {
    0,0,0,
    0b0001111000000000,
    0b0011001100000000,
    0b0110000100000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000100000000,
    0b0011001100000000,
    0b0001111000000000,
    0,0,0
  },
  [68] = {
    0,0,0,
    0b0111111000000000,
    0b0110001100000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110001100000000,
    0b0111111000000000,
    0,0,0
  },
  [69] = {
    0,0,0,
    0b0111111100000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0111111000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0111111100000000,
    0,0,0
  },
  [70] = {
    0,0,0,
    0b0111111100000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0111111000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0,0,0
  },
  [71] = {
    0,0,0,
    0b0001111100000000,
    0b0011000110000000,
    0b0110000010000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110011110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0011000110000000,
    0b0001111010000000,
    0,0,0
  },
  [72] = {
    0,0,0,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0111111110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0,0,0
  },
  [73] = {
    0,0,0,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0,0,0
  },
  [74] = {
    0,0,0,
    0b0000110000000000,
    0b0000110000000000,
    0b0000110000000000,
    0b0000110000000000,
    0b0000110000000000,
    0b0000110000000000,
    0b0000110000000000,
    0b1100110000000000,
    0b1100110000000000,
    0b0111100000000000,
    0,0,0
  },
  [75] = {
    0,0,0,
    0b0110001100000000,
    0b0110011000000000,
    0b0110110000000000,
    0b0111100000000000,
    0b0111000000000000,
    0b0111100000000000,
    0b0110110000000000,
    0b0110011000000000,
    0b0110001100000000,
    0b0110000110000000,
    0,0,0
  },
  [76] = {
    0,0,0,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0111111100000000,
    0,0,0
  },
  [77] = {
    0,0,0,
    0b0110000011000000,
    0b0110000011000000,
    0b0111000111000000,
    0b0111000111000000,
    0b0111101111000000,
    0b0111101111000000,
    0b0110111011000000,
    0b0110111011000000,
    0b0110010011000000,
    0b0110010011000000,
    0,0,0,
  },
  [78] = {
    0,0,0,
    0b0110000110000000,
    0b0111000110000000,
    0b0111100110000000,
    0b0111100110000000,
    0b0110110110000000,
    0b0110110110000000,
    0b0110011110000000,
    0b0110011110000000,
    0b0110001110000000,
    0b0110000110000000,
    0,0,0
  },
  [79] = {
    0,0,0,
    0b0001111000000000,
    0b0011001100000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0011001100000000,
    0b0001111000000000,
    0,0,0
  },
  [80] = {
    0,0,0,
    0b0111111100000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0111111100000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0,0,0
  },
  [81] = {
    0,0,0,
    0b0001111000000000,
    0b0011001100000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110011110000000,
    0b0011001100000000,
    0b0001111110000000,
    0,0,0
  },
  [82] = {
    0,0,0,
    0b0111111100000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0111111100000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000011000000,
    0,0,0
  },
  [83] = {
    0,0,0,
    0b0011111000000000,
    0b0110001100000000,
    0b0110001100000000,
    0b0110000000000000,
    0b0011100000000000,
    0b0000111000000000,
    0b0000001100000000,
    0b0110001100000000,
    0b0110001100000000,
    0b0011111000000000,
    0,0,0
  },
  [84] = {
    0,0,0,
    0b1111111100000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0001100000000000,
    0,0,0
  },
  [85] = {
    0,0,0,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0110000110000000,
    0b0011001100000000,
    0b0001111000000000,
    0,0,0
  },
  [86] = {
    0,0,0,
    0b1100001100000000,
    0b1100001100000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0010010000000000,
    0b0011110000000000,
    0b0011110000000000,
    0b0001100000000000,
    0b0001100000000000,
    0,0,0
  },
  [87] = {
    0,0,0,
    0b1100001100001100,
    0b1100001100001100,
    0b1100001100001100,
    0b0110011110011000,
    0b0110011110011000,
    0b0011010010110000,
    0b0011110011110000,
    0b0001100001100000,
    0b0001100001100000,
    0b0001100001100000,
    0,0,0
  },
  [88] = {
    0,0,0,
    0b1100000110000000,
    0b1100000110000000,
    0b0110001100000000,
    0b0011011000000000,
    0b0001110000000000,
    0b0001110000000000,
    0b0011011000000000,
    0b0110001100000000,
    0b1100000110000000,
    0b1100000110000000,
    0,0,0
  },
  [89] = {
    0,0,0,
    0b1100000011000000,
    0b1100000011000000,
    0b0110000110000000,
    0b0011001100000000,
    0b0001111000000000,
    0b0000110000000000,
    0b0000110000000000,
    0b0000110000000000,
    0b0000110000000000,
    0b0000110000000000,
    0,0,0
  },
  [90] = {
    0,0,0,
    0b1111111110000000,
    0b0000000110000000,
    0b0000001100000000,
    0b0000011000000000,
    0b0000110000000000,
    0b0001100000000000,
    0b0011000000000000,
    0b0110000000000000,
    0b1100000000000000,
    0b1111111110000000,
    0,0,0
  },
  [91] = {
    0,0,0,
    0b0111000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0111000000000000,
  },
  [92] = {
    0,0,0,
    0b1100000000000000,
    0b1100000000000000,
    0b1100000000000000,
    0b1100000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0,
  },
  [93] = {
    0,0,0,
    0b1110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b1110000000000000,
  },
  [94] = {
    0,0,0,
    0b0010000000000000,
    0b0111000000000000,
    0b1101100000000000,
  },
  [95] = {
    [15] = 0b1111111100000000
  },
  [96] = {
    [2] = 0b0111000000000000,
    [3] = 0b0011000000000000,
    [4] = 0b0001100000000000,
  },
  [97] = {
    0,0,0,0,0,0,
    0b0011111000000000,
    0b0110011000000000,
    0b0001111000000000,
    0b0011011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0011111000000000,
    0,0,0
  },
  [98] = {
    0,0,0,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0111110000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0111110000000000,
  },
  [99] = {
    0,0,0,0,0,0,
    0b0011110000000000,
    0b0110011000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110011000000000,
    0b0011110000000000,
  },
  [100] = {
    0,0,0,
    0b0000011000000000,
    0b0000011000000000,
    0b0000011000000000,
    0b0011111000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0011111000000000,
  },
  [101] = {
    0,0,0,0,0,0,
    0b0011110000000000,
    0b0110011000000000,
    0b0111111000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110011000000000,
    0b0111111000000000,
  },
  [102] = {
    0,0,0,
    0b0011000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b1111000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
  },
  [103] = {
    0,0,0,0,0,0,
    0b0011111000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0011111000000000,
    0b0000011000000000,
    0b0110011000000000,
    0b0011110000000000,
  },
  [104] = {
    0,0,0,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0111110000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
  },
  [105] = {
    0,0,0,
    0b0110000000000000,
    0b0110000000000000,
    0,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
  },
  [106] = {
    0,0,0,
    0b0110000000000000,
    0b0110000000000000,
    0,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b1100000000000000
  },
  [107] = {
    0,0,0,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110011000000000,
    0b0110110000000000,
    0b0111100000000000,
    0b0111000000000000,
    0b0111100000000000,
    0b0110110000000000,
    0b0110011000000000,
  },
  [108] = {
    0,0,0,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
  },
  [109] = {
    0,0,0,0,0,0,
    0b0111111111000000,
    0b0110011001100000,
    0b0110011001100000,
    0b0110011001100000,
    0b0110011001100000,
    0b0110011001100000,
    0b0110011001100000,
  },
  [110] = {
    0,0,0,0,0,0,
    0b0111110000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
  },
  [111] = {
    0,0,0,0,0,0,
    0b0011110000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0011110000000000,
  },
  [112] = {
    0,0,0,0,0,0,
    0b0111110000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0111110000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
  },
  [113] = {
    0,0,0,0,0,0,
    0b0011111000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0011111000000000,
    0b0000011000000000,
    0b0000011000000000,
    0b0000011000000000,
  },
  [114] = {
    0,0,0,0,0,0,
    0b0111100000000000,
    0b0111000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
  },
  [115] = {
    0,0,0,0,0,0,
    0b0011110000000000,
    0b0110011000000000,
    0b0110000000000000,
    0b0011110000000000,
    0b0000011000000000,
    0b0110011000000000,
    0b0011110000000000,
  },
  [116] = {
    0,0,0,0,
    0b0110000000000000,
    0b0110000000000000,
    0b1111000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0011000000000000,
  },
  [117] = {
    0,0,0,0,0,0,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0011110000000000
  },
  [118] = {
    0,0,0,0,0,0,
    0b1100001100000000,
    0b1100001100000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0011110000000000,
    0b0001100000000000,
    0b0001100000000000,
  },
  [119] = {
    0,0,0,0,0,0,
    0b1100000011000000,
    0b1100110011000000,
    0b0110110110000000,
    0b0110110110000000,
    0b0111111110000000,
    0b0011001100000000,
    0b0011001100000000,
  },
  [120] = {
    0,0,0,0,0,0,
    0b1100001100000000,
    0b0110011000000000,
    0b0011110000000000,
    0b0001100000000000,
    0b0011110000000000,
    0b0110011000000000,
    0b1100001100000000,
  },
  [121] = {
    0,0,0,0,0,0,
    0b1100001100000000,
    0b1100001100000000,
    0b0110011000000000,
    0b0110011000000000,
    0b0011110000000000,
    0b0011110000000000,
    0b0001100000000000,
    0b0001100000000000,
    0b0011000000000000,
    0b0110000000000000,
  },
  [122] = {
    0,0,0,0,0,0,
    0b0111111000000000,
    0b0000011000000000,
    0b0000110000000000,
    0b0001100000000000,
    0b0011000000000000,
    0b0110000000000000,
    0b0111111000000000,
  },
  [123] = {
    0,0,0,
    0b0001100000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0110000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0001100000000000,
  },
  [124] = {
    0,0,0,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
    0b0110000000000000,
  },
  [125] = {
    0,0,0,
    0b0110000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0001100000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0011000000000000,
    0b0110000000000000,
  },
  [126] = {
    [3] = 0b11101000000000,
    [4] = 0b10111000000000,
  }
};

void vinit(int64_t width, int64_t height, int64_t bpp, int64_t pitch, uint64_t addr)
{
  fb = (uint32_t*)(uint32_t)addr;
  //fbb = (uint32_t*)malloc(width * height * 4);
  puts("Back buffer at 0x");
  char b[16];
  puts(itoa((int)&fbb, b, 16));
  putc('\n');
  puts("Framebuffer at 0x"); puts(itoa((int)fb, b, 16)); putc('\n');
  fbw = width;
  fbh = height;
  fbbpp = bpp;
  fbp = pitch;
  color.r = 255;
  color.g = 255;
  color.b = 255;
  color.a = 255;
  /*puts("lo");
  for(int64_t y = 0; y < fbh; y++)
  {
    for(int64_t x = 0; x < fbw; x++)
    {
      fb[y * width + x] = 0;
      fbb[y * width + x] = 0;
    }
    putc(',');
  }
  puts("op\n");*/
}

void vdestroy()
{
  puts("vdestroy\n");
  //if(fbb) free(fbb);
  fb = NULL;
  fbw = 0; fbh = 0; fbbpp = 0; fbp = 0;
  color.r = 0; color.g = 0; color.b = 0; color.a = 0;
}

void vplot(int64_t x, int64_t y)
{
  if(fb == NULL) return;
  if(x < 0) return;
  if(y < 0) return;
  if(x >= fbw) return;
  if(y >= fbh) return;
  if(color.a == 0) return;
  if(color.a == 255)
  {
    fbb[y * fbw + x] = color.r << 16 | color.g << 8 | color.b;
  }
  else
  {
    uint8_t dr, dg, db, or, og, ob;
    dr = fbb[y * fbw + x] >> 16 & 0xFF;
    dg = fbb[y * fbw + x] >> 8 & 0xFF;
    db = fbb[y * fbw + x] & 0xFF;
    uint8_t a = color.a + 1;
    uint8_t ia = 256 - color.a;
    or = (uint8_t)((a * color.r + ia * dr) >> 8);
    og = (uint8_t)((a * color.g + ia * dg) >> 8);
    ob = (uint8_t)((a * color.b + ia * db) >> 8);
    fbb[y * fbw + x] = or << 16 | og << 8 | ob;
  }
}

void vsetcol(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
  color.r = r;
  color.b = b;
  color.g = g;
  color.a = a;
}

rgb_t vgetcol()
{
  return color;
}

void vcls()
{
  for(int64_t y = 0; y < fbh; y++)
    for(int64_t x = 0; x < fbw; x++)
    {
      uint32_t p = fb[y * fbw + x];
      if((p >> 16 & 0xFF) != color.r || (p >> 8 & 0xFF) != color.g || (p & 0xFF) != color.b)
        vplot(x, y);
    }
}

void vd_print(int64_t x, int64_t y, const char* str, int64_t* xe, int64_t* ye)
{
  int64_t x2 = x;
  int64_t y2 = y;
  int i = 0;
  while(str[i] != '\0')
  {
    if(str[i] < 32 && (str[i] != 9 || str[i] != 10 || str[i] != 11 || str[i] != 13) && str[i] != 127) continue;
    if(str[i] == 0 || str[i] > 127) break;
    switch(str[i])
    {
      case '\r':
        x2 = x;
        break;
      case '\n':
        x2 = x;
        y2++;
        break;
      case '\t':
        x2 += 24;
        break;
      default:
        if(fb_font[(int)str[i]] != NULL)
          vd_bitmap16(fb_font[(int)str[i]], x2, y2, 16);
        break;
    }
    i++;
    x2 += 8;
  }
  if(xe != NULL) *xe = x2;
  if(ye != NULL) *ye = y2;
}

int64_t vgetw()
{
  return fbw;
}

int64_t vgeth()
{
  return fbh;
}

void vd_rectangle(vdrawmode_t drawmode, int64_t x, int64_t y, int64_t w, int64_t h)
{
  if(drawmode == FILL)
    for(int64_t iy = y; iy < y + h; iy++)
      for(int64_t ix = x; ix < x + w; ix++)
        vplot(ix, iy);
  else if(drawmode == LINE)
  {
    for(int64_t ix = x; ix < x + w; ix++)
    {
      vplot(ix, y);
      vplot(ix, y + h);
    }
    for(int64_t iy = y; iy < y + h; iy++)
    {
      vplot(x, iy);
      vplot(x + w, iy);
    }
  }
}

void vd_line(int64_t x1, int64_t y1, int64_t x2, int64_t y2)
{
  float rx1, rx2, ry1, ry2;
  if(x2 >= x1)
  {
    rx1 = x1;
    rx2 = x2;
    ry1 = y1;
    ry2 = y2;
  }
  else
  {
    rx1 = x2;
    rx2 = x1;
    ry1 = y2;
    ry2 = y1;
  }
  float dx = rx2 - rx1;
  float dy = ry2 - ry1;
  for(float x = rx1; x < rx2; x += 0.5)
  {
    float y = ry1 + dy * (x - rx1) / dx;
    vplot((int64_t)x, (int64_t)y);
  }
}

//copy framebuffer buffer to framebuffer
void vswap()
{
  //asm volatile("cli");
  uint8_t* fbbyte = (uint8_t*)fb;
  uint8_t* fbbbyte = (uint8_t*)fbb;
  for(int64_t y = 0; y < fbh; y++)
    for(int64_t x = 0; x < fbp; x++)
    {
      fbbyte[y * fbp + x] = fbbbyte[y * fbp + x];
    }
  //asm volatile("sti");
}

void vd_bitmap16(uint16_t* bitmap, int64_t x, int64_t y, int64_t h)
{
  for(int64_t i = 0; i < h; i++)
  {
    for(int j = 15; j > 0; j--)
    {
      if(bitmap[i] >> j & 1) vplot(x + 16 - j, y + i);
    }
  }
}

void vd_bitmap32(uint32_t* bitmap, int64_t x, int64_t y, int64_t h)
{
  for(int64_t i = 0; i < h; i++)
  {
    for(int j = 31; j > 0; j--)
    {
      if(bitmap[i] >> j & 1) vplot(x + 32 - j, y + i);
    }
  }
}
