#include "FlipDisplay_Util.h"

void printBinary(byte b) {
  for (int i = 7; i >= 0; i--) {
    Serial.print((b >> i) & 0X01);//shift and select first bit
  }
  Serial.println();
}
