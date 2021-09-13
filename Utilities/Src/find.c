#include "find.h"

uint8_t* find_ar(uint8_t const* str, size_t sizeStr,
                 uint8_t const* subStr, size_t sizeSubStr)
{
  for (size_t i = 0, j = 0; i < sizeStr; ++i) {
    if ((i + sizeSubStr) > sizeStr) {
      return NULL;
    }

    for (j = 0; j < sizeSubStr; ++j) {
      if (str[i + j] != subStr[j]) {
        break;
      }
    }

    if (j == sizeSubStr) {
      return (uint8_t*)&str[i + sizeSubStr];
    }
  }

  return NULL;
}


uint8_t* find_circular(uint8_t const* buffer, size_t sizeBuf,
                       uint8_t const* str, size_t sizeStr,
                       uint8_t const* subStr, size_t sizeSubStr)
{
  size_t index = str - buffer;
  for (size_t i = 0, j = 0; i < sizeStr; ++i) {
    if ((i + sizeSubStr) > sizeStr) {
      return NULL;
    }

    for (j = 0; j < sizeSubStr; ++j) {
      size_t nextIndex = index + j;
      size_t findIndex = (nextIndex < sizeBuf) ? nextIndex : nextIndex - sizeBuf;
      if (buffer[findIndex] != subStr[j]) {
        break;
      }
    }

    if (j == sizeSubStr) {
      size_t endIndex = index + sizeSubStr;
      if (endIndex < sizeBuf) {
        return (uint8_t*)&buffer[endIndex];
      }
      return (uint8_t*)&buffer[endIndex - sizeBuf];
    }

    ++index;
    if (index >= sizeBuf) {
      index = 0;
    }
  }

  return NULL;
}
