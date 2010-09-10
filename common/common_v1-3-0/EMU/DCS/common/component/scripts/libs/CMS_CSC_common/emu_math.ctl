/**@file

This package contains general purpose utility functions.

@author Evaldas Juska (PH/UCM)
@date   May 2009
*/

const float EMUMATH_PI = 3.1415926535897932384626433832795;

//utility (math) functions
string emu_decToHex(int value, int byteCount = 2) {
  blob tmpBlob;
  blobAppendValue(tmpBlob, value, byteCount, true);
  string ret = tmpBlob;
  
  return tmpBlob;
}

int emu_hexToDec(string value, int byteCount = 2) {
  blob tmpBlob = value;
  int ret;
  blobGetValue(tmpBlob, 0, ret, byteCount, true);
  
  return ret;
}

//decimal to binary
string emu_decToBin(int value, int bitCount) {
  bit32 bits;
  
  unsigned valueUnsigned = emu_signedToUnsigned(value, bitCount);
  bits = valueUnsigned;

  string ret;
  for(int i = bitCount - 1; i >= 0; i--) {
    ret += getBit(bits, i);
  }
    
  return ret;
}

unsigned emu_signedToUnsigned(int value, int bitCount) {
  bit32 maxValueBits;
  int maxValueInt;
  unsigned ret;
  
  if (value < 0) {
    for(int i=0; i < bitCount; i++) {
      setBit(maxValueBits, i, true);
    }
    maxValueInt = maxValueBits;
    
    ret = maxValueInt + value + 1;
  } else {
    ret = value;
  }
  
  return ret;
}

// @return absolute value
float emu_abs(float value) {
  if (value < 0) {
    value = value * -1;
  }
  return value;
}

//@return maximum value
float emu_max(dyn_float values) {
  float ret = minFLOAT();
  for (int i=1; i <= dynlen(values); i++) {
    if (values[i] > ret) {
      ret = values[i];
    }
  }
  return ret;
}
